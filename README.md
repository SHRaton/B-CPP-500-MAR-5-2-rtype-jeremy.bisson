# R-Type 🚀

## Table of Contents
- [Quick Start](#quick-start)
- [Description](#description)
- [Features](#-features)
- [Running the Game](#-running-the-game)
- [Controls](#-controls)
- [Tech Stack](#️-tech-stack)
- [Team](#-team)
- [Documentation](#-documentation)
- [Support](#-support)
- [License](#license)

## Quick Start

```bash
# Clone the repository
git clone https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson.git
cd B-CPP-500-MAR-5-2-rtype-jeremy.bisson

# Build using Conan and CMake
conan profile detect --force
conan install . --output-folder=build --build=missing
cd build

# Windows
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
cmake --build . --config Release

# Linux/MacOS
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Release
make
```

## Description

R-Type is a modern networked multiplayer recreation of the classic 1987 shoot'em up arcade game, featuring real-time battles, dynamic combat systems, and enhanced graphics.

## 🎮 Features

- **Cross-platform**: Windows, Linux, and MacOS support
- **Multiplayer**: Up to 4 players
- **Modern Architecture**: ECS pattern
- **Network**: UDP-based client-server
- **Graphics**: SFML-powered rendering

## 🚀 Running the Game

```bash
# Start Server
./r-type_server <port>

# Launch Client
./r-type_client
```

## 🎯 Controls
- **↑↓←→**: Movement
- **Space**: Shoot

## 🛠️ Tech Stack

- C++17
- SFML
- Boost.Asio
- CMake/Conan

## 👥 Team

| Name | Role | GitHub |
|------|------|--------|
| Jérémy Bisson | Project Lead | [jeremybepitech](https://github.com/jeremybepitech) |
| Meddi Gueran | Core Developer | [MeddiGU](https://github.com/MeddiGU) |
| Jérémy Delfino | Network Developer | [jeremydelfino](https://github.com/jeremydelfino) |
| Luca Giglio | Graphics Developer | [LaJinxKonAime](https://github.com/LaJinxKonAime) |
| Alexandre Vittenet | Engine Developer | [SHRaton](https://github.com/SHRaton) |

## 📚 Documentation

- [Full Documentation](docs/index.md)
- [Installation Guide](docs/guides/installation.md)
- [Network Protocol](docs/technical/network.md)
- [Contributing Guide](docs/development/contributing.md)

## 📬 Support

- Create an [Issue](https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson/issues)
- Contact team members via GitHub

## License

MIT License - See [LICENSE](LICENSE)

---
Maintained by the R-Type Team 🚀