# Guide d'Installation R-Type

## Prérequis par Plateforme

### Linux
- Conan2 (via Python ou gestionnaire de paquets)
- gcc & g++
- cmake

### Windows (MSVC 2022)
**Note: Développé et testé avec MSVC (cl), non testé avec MinGW**
- Conan2 (via Chocolatey ou Python)
- MSVC 2022 (vérifiable avec `cl --version`)
- CMake

### MacOS
- Conan2 (via Python ou gestionnaire de paquets)
- clang
- cmake

## Installation

### Configuration de Conan
```bash
# Si pas de profil Conan existant
conan profile detect --force

# Installation des dépendances
conan install . --output-folder=build --build=missing
```

### Options de Compilation CMake

Drapeaux disponibles pour désactiver certaines parties :
```bash
-DBUILD_CLIENT=OFF    # Désactive la compilation du client
-DBUILD_SERVER=OFF    # Désactive la compilation du serveur
-DBUILD_EDITOR=OFF    # Désactive la compilation de l'éditeur
-DBUILD_TESTS=OFF     # Désactive la compilation des tests
```

### Compilation par Plateforme

#### Windows (MSVC)
```bash
# Dans le dossier build/
cmake ..

# Mode Release
cmake --build . --config Release

# Mode Debug
cmake --build . --config Debug
```

#### UNIX (MacOS / Linux)
```bash
# Mode Release
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Mode Debug
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

## Exécution

### Structure des Binaires

```
📦 R-Type
┣ 📂 Release/
┃ ┣ 📜 r-type_client.exe
┃ ┗ 📜 r-type_server.exe
┗ 📂 Debug/
  ┣ 📜 r-type_client.exe
  ┗ 📜 r-type_server.exe
```

### Lancement
```bash
# Client
./r-type_client.exe

# Serveur
./r-type_server.exe
```

## Docker

### Images Disponibles
- Image de compilation : [adamelm/r-type-compiler](https://hub.docker.com/r/adamelm/r-type-compiler)
  - Basée sur ubuntu:latest
  - Inclut toutes les dépendances nécessaires

### Compilation avec Docker
```bash
# Construction de l'image
docker build -t r-type .\Dockerfile.build
```

### Déploiement avec Docker Compose
```bash
# Construction et lancement du serveur
docker-compose up
```

## Structure du CMake

```cmake
cmake_minimum_required(VERSION 3.15)
project(r_type)

# Configuration C++17
set(CMAKE_CXX_STANDARD 17)

# Dépendances requises
find_package(SFML COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED)

# Sources du serveur
file(GLOB SERVER_SOURCES "src/server/*.cpp")
add_executable(server ${SERVER_SOURCES} src/Systems.cpp src/server/Mediator.cpp)

# Sources du client
file(GLOB CLIENT_SOURCES "src/client/*.cpp")
add_executable(client ${CLIENT_SOURCES} src/Systems.cpp src/server/Mediator.cpp)

# Liaison des bibliothèques
target_link_libraries(server sfml-graphics boost::boost)
target_link_libraries(client sfml-graphics sfml-window sfml-system boost::boost)
```

## Dépendances Principales
- SFML (graphics, audio, window, system)
- Boost
- Conan2 pour la gestion des paquets