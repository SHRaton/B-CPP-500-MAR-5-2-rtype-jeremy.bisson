# R-Type 🚀

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Architecture](#architecture)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
- [Usage](#usage)
- [Technologies](#technologies)
- [Documentation](#documentation)

## Description

R-Type is a modern recreation of the famous 1987 arcade shoot'em up game. This project implements a networked multiplayer version, allowing players to collaborate or compete in intense space battles.

### 🎮 Game Overview
- Real-time multiplayer mode
- Graphics inspired by the original game with a modern touch
- Dynamic combat system
- Immersive sound and visual effects

## Features

### 🌟 Key Features
- **Cross-platform**: Compatible with Windows, Linux, and MacOS
- **Multiplayer**: Support for up to 4 simultaneous players
- **Client-Server Architecture**: UDP communication for minimal latency
- **ECS (Entity Component System)**: Modular and performant architecture
- **SFML Graphics**: Smooth and optimized rendering

### 🛠️ Gameplay Systems
- Different types of spaceships with unique characteristics
- Power-ups and upgrade system
- Varied enemy patterns
- Scoring and ranking system

## Architecture

### 📐 Project Structure
```
📦 R-Type
┣ 📂 src/
┃ ┣ 📂 client/      # Client code
┃ ┣ 📂 server/      # Server code
┃ ┣ 📂 components/  # ECS components
┃ ┗ 📂 systems/     # Game systems
┣ 📂 assets/        # Graphics and sound resources
┗ 📂 docs/          # Documentation
```

## Installation

### Prerequisites

#### 🖥️ Development Environment
- CMake (3.24+)
- C++ compiler with C++17 support
- Conan 2.0 (package manager)

#### 📚 Main Dependencies
- SFML (graphics, audio, window, system)
- Boost (asio, system)
- nlohmann/json

### Building

#### Windows
```bash
# Configuration
conan profile detect --force
conan install . --output-folder=build --build=missing
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"

# Compilation
cmake --build . --config Release
```

#### Linux/MacOS
```bash
# Configuration
conan profile detect --force
conan install . --output-folder=build --build=missing
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Release

# Compilation
make
```

## Usage

### 🎮 Running the Game

1. Start the server:
```bash
# Windows
./Release/server.exe <port>

# Linux/MacOS
./server <port>
```

2. Launch the client:
```bash
# Windows
./Release/r-type.exe

# Linux/MacOS
./r-type
```

### 🎯 Game Controls
- **↑↓←→**: Movement
- **Space**: Shoot
- **S**: Special shot
- **R**: Flip

## Technologies

- 🔧 C++17
- 🎮 SFML
- 🌐 Boost.Asio
- 📦 CMake/Conan
- 🐳 Docker

## Documentation

For more technical details, check out:
- [Technical Documentation](docs/technical.md)
- [Developer Guide](docs/developer-guide.md)
- [Network Protocol](docs/network-protocol.md)

## 👥 Authors

| Name | Role |
|------|------|
| Jérémy Bisson | Project Lead & Developer |
| Meddi Gueran | Core Developer |
| Jérémy Delfino | Network Developer |
| Luca Giglio | Graphics Developer |
| Alexandre Vittenet | Engine Developer |

## 🤝 Contributing

Contributions are welcome! Feel free to:
1. Fork the project
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

## ⚖️ License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

Maintained by the R-Type Team 🚀