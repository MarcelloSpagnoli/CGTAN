# CGTAN

Progetto I di Computer Graphics.

## Requisiti

- [CMake](https://cmake.org/download/) (>= 3.10)
- Un compilatore C++17:
  - **Windows**: Visual Studio (workload "Sviluppo di applicazioni desktop con C++") oppure MinGW-w64
  - **macOS**: Xcode Command Line Tools (`xcode-select --install`)
  - **Linux**: GCC/Clang + pacchetti di sviluppo OpenGL (es. `libgl1-mesa-dev` su Debian/Ubuntu)

Le librerie di terze parti (GLFW, Freetype, GLM, ImGui) vengono scaricate e compilate automaticamente da CMake al primo `cmake ..` (tramite `FetchContent`): **serve una connessione a Internet** la prima volta. Solo glad è incluso direttamente in `libs/` (è codice generato, non una libreria da scaricare).

## Build

Da terminale, nella cartella del progetto:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Il generatore (Visual Studio, MinGW, Make, ecc.) viene scelto automaticamente da CMake in base a cosa trova installato sul sistema. `cmake --build .` funziona a prescindere dal generatore usato.

La prima build richiede qualche minuto in più per scaricare e compilare le dipendenze; le build successive sono rapide come su un progetto normale.

## Eseguire

L'eseguibile si chiama `CGTAN` (su Windows `CGTAN.exe`, tipicamente in `build/Debug/` o `build/Release/` a seconda del generatore; su macOS/Linux direttamente in `build/`).

```bash
./CGTAN
```
