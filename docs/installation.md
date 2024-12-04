# R-Type Installation Guide 📥

## Prerequisites by Platform 💻

### Linux 🐧
- Conan2 (via Python or package manager)
- gcc & g++
- cmake

### Windows (MSVC 2022) 🪟
**Note: Developed and tested with MSVC (cl), not tested with MinGW**
- Conan2 (via Chocolatey or Python)
- MSVC 2022 (verifiable with `cl --version`)
- CMake

### MacOS 🍎
- Conan2 (via Python or package manager)
- clang
- cmake

## Installation 🔧

### Conan Configuration
```bash
# If no existing Conan profile
conan profile detect --force

# Install dependencies
conan install . --output-folder=build --build=missing
```

### CMake Compilation Options 🛠️

Available flags to disable certain parts:
```bash
-DBUILD_CLIENT=OFF    # Disable client compilation
-DBUILD_SERVER=OFF    # Disable server compilation
-DBUILD_EDITOR=OFF    # Disable editor compilation
-DBUILD_TESTS=OFF     # Disable test compilation
```

### Compilation by Platform ⚙️

#### Windows (MSVC)
```bash
# In build/ folder
cmake ..

# Release Mode
cmake --build . --config Release

# Debug Mode
cmake --build . --config Debug
```

#### UNIX (MacOS / Linux)
```bash
# Release Mode
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Debug Mode
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

## Execution 🚀

### Binary Structure 📁

```
📦 R-Type
┣ 📂 Release/
┃ ┣ 📜 r-type_client.exe
┃ ┗ 📜 r-type_server.exe
┗ 📂 Debug/
  ┣ 📜 r-type_client.exe
  ┗ 📜 r-type_server.exe
```

### Launch 🎮
```bash
# Client
./r-type_client.exe

# Server
./r-type_server.exe
```

## Docker 🐳

### Available Images
- Compilation image: [adamelm/r-type-compiler](https://hub.docker.com/r/adamelm/r-type-compiler)
  - Based on ubuntu:latest
  - Includes all necessary dependencies

### Docker Compilation
```bash
# Build image
docker build -t r-type .\Dockerfile.build
```

### Docker Compose Deployment
```bash
# Build and launch server
docker-compose up
```

## CMake Structure 📝

```cmake
cmake_minimum_required(VERSION 3.15)
project(r_type)

# C++17 Configuration
set(CMAKE_CXX_STANDARD 17)

# Required dependencies
find_package(SFML COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED)

# Server sources
file(GLOB SERVER_SOURCES "src/server/*.cpp")
add_executable(server ${SERVER_SOURCES} src/Systems.cpp src/server/Mediator.cpp)

# Client sources
file(GLOB CLIENT_SOURCES "src/client/*.cpp")
add_executable(client ${CLIENT_SOURCES} src/Systems.cpp src/server/Mediator.cpp)

# Library linking
target_link_libraries(server sfml-graphics boost::boost)
target_link_libraries(client sfml-graphics sfml-window sfml-system boost::boost)
```

## Main Dependencies 📚
- SFML (graphics, audio, window, system)
- Boost
- Conan2 for package management