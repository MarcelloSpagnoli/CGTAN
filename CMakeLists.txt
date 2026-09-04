cmake_minimum_required(VERSION 3.14)
project(CGTAN)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/W0)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include(FetchContent)

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

FetchContent_Declare(freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-14-3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(freetype)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.7
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

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
