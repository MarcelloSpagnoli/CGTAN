cmake_minimum_required(VERSION 3.14)
project(CGTAN)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/W0)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include(FetchContent)
find_package(Git REQUIRED)

# Alcune dipendenze (GLM, FreeType, ImGui) portano con se' cartelle enormi
# (doc/, test/, examples/) che non servono a compilare la libreria ma che un
# normale FetchContent con GIT_SHALLOW scarica comunque per intero, perche'
# non sono submodule ma file normali del repo.
# Questa funzione fa un clone parziale (--filter=blob:none) + sparse-checkout,
# scaricando solo le cartelle passate in SPARSE_PATHS (in modalita' "cone" i
# file alla radice del repo, tipo CMakeLists.txt, arrivano comunque gratis).
function(fetch_sparse name repo tag out_source_dir)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SPARSE_PATHS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(src_dir ${CMAKE_BINARY_DIR}/_deps/${name}-src)
    if(NOT EXISTS ${src_dir}/CMakeLists.txt AND NOT EXISTS ${src_dir}/imgui.cpp)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone --filter=blob:none --sparse --depth 1 --branch ${tag} ${repo} ${src_dir}
            COMMAND_ERROR_IS_FATAL ANY
        )
        execute_process(
            COMMAND ${GIT_EXECUTABLE} sparse-checkout set ${ARG_SPARSE_PATHS}
            WORKING_DIRECTORY ${src_dir}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    set(${out_source_dir} ${src_dir} PARENT_SCOPE)
endfunction()

# 1. LIBRERIE (scaricate e compilate da sorgente, stesse versioni usate finora)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

# FreeType (clone sparse: docs/, tests/, subprojects/ non servono alla build)
fetch_sparse(freetype https://github.com/freetype/freetype.git VER-2-14-3 freetype_SOURCE_DIR
    SPARSE_PATHS include src builds
)
add_subdirectory(${freetype_SOURCE_DIR} ${CMAKE_BINARY_DIR}/_deps/freetype-build)

# GLM (clone sparse: ~20MB di doc/ non necessari alla build)
fetch_sparse(glm https://github.com/g-truc/glm.git 1.0.3 glm_SOURCE_DIR
    SPARSE_PATHS glm cmake
)
add_subdirectory(${glm_SOURCE_DIR} ${CMAKE_BINARY_DIR}/_deps/glm-build)

# ImGui (clone sparse: examples/, docs/, misc/ non servono; i sorgenti .cpp/.h
# alla radice e backends/ arrivano comunque con il clone sparse)
fetch_sparse(imgui https://github.com/ocornut/imgui.git v1.92.7 imgui_SOURCE_DIR
    SPARSE_PATHS backends
)

include_directories(${glm_SOURCE_DIR} ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends libs/include ${freetype_SOURCE_DIR}/include)

# 2. SORGENTI
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# 3. COMPILAZIONE
add_executable(CGTAN ${SOURCES}
libs/src/glad.c
${imgui_SOURCE_DIR}/imgui.cpp
${imgui_SOURCE_DIR}/imgui_draw.cpp
${imgui_SOURCE_DIR}/imgui_widgets.cpp
${imgui_SOURCE_DIR}/imgui_tables.cpp
${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
# Sostituisci la vecchia riga con questa:
target_compile_definitions(CGTAN PRIVATE ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets")

# 4. LINK
target_link_libraries(CGTAN glfw freetype)
target_include_directories(CGTAN PRIVATE ${freetype_SOURCE_DIR}/include)

if(APPLE)
target_link_libraries(CGTAN "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(WIN32)
target_link_libraries(CGTAN opengl32)
elseif(UNIX)
target_link_libraries(CGTAN GL)
endif()
