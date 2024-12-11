<div align="center">

# R-Type 🚀

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![SFML](https://img.shields.io/badge/SFML-2.5.1-green.svg)](https://www.sfml-dev.org/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20MacOS-lightgrey.svg)](docs/installation.md)

[<img src="/api/placeholder/800/400" alt="R-Type Game Screenshot" width="800"/>](docs/index.md)

*A modern multiplayer recreation of the legendary shoot'em up arcade game*

[Play Now](#quick-start) • [Documentation](docs/index.md) • [Report Bug](issues) • [Request Feature](issues)

</div>

## 🚀 Quick Start

<div align="center">

[<img src="/api/placeholder/600/150" alt="Quick Start Banner" width="600"/>](docs/installation.md)

</div>

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

## 🎮 Features

<div align="center">

[<img src="/api/placeholder/200/200" alt="Multiplayer Icon" width="200"/>]() [<img src="/api/placeholder/200/200" alt="Graphics Icon" width="200"/>]() [<img src="/api/placeholder/200/200" alt="Network Icon" width="200"/>]()

</div>

- **Cross-platform**: Seamless gaming across Windows, Linux, and MacOS
- **Multiplayer**: Epic battles with up to 4 players simultaneously
- **Modern Architecture**: Powered by the Entity Component System pattern
- **Network**: Lightning-fast UDP-based client-server architecture
- **Graphics**: Stunning SFML-powered visuals and effects

## 🎯 Controls

<div align="center">

[<img src="/api/placeholder/400/200" alt="Game Controls" width="400"/>](docs/controls.md)

</div>

- **↑↓←→**: Movement
- **Space**: Shoot

## 🛠️ Tech Stack

<div align="center">

[<img src="/api/placeholder/100/100" alt="C++" width="100"/>](https://isocpp.org/)
[<img src="/api/placeholder/100/100" alt="SFML" width="100"/>](https://www.sfml-dev.org/)
[<img src="/api/placeholder/100/100" alt="Boost" width="100"/>](https://www.boost.org/)
[<img src="/api/placeholder/100/100" alt="CMake" width="100"/>](https://cmake.org/)

</div>

## 👥 Team

<div align="center">

| [<img src="/api/placeholder/100/100" alt="Jeremy" width="100"/>](https://github.com/jeremybepitech) | [<img src="/api/placeholder/100/100" alt="Meddi" width="100"/>](https://github.com/MeddiGU) | [<img src="/api/placeholder/100/100" alt="Jeremy D" width="100"/>](https://github.com/jeremydelfino) | [<img src="/api/placeholder/100/100" alt="Luca" width="100"/>](https://github.com/LaJinxKonAime) | [<img src="/api/placeholder/100/100" alt="Alexandre" width="100"/>](https://github.com/SHRaton) |
|:---:|:---:|:---:|:---:|:---:|
| **Jérémy Bisson** | **Meddi Gueran** | **Jérémy Delfino** | **Luca Giglio** | **Alexandre Vittenet** |
| Project Lead | Core Developer | Network Developer | Graphics Developer | Engine Developer |

</div>

## 📚 Documentation

<div align="center">

[<img src="/api/placeholder/700/200" alt="Documentation Banner" width="700"/>](docs/index.md)

</div>

- [Full Documentation](docs/index.md)
- [Server Documentation](docs/server.md)
- [Client Documentation](docs/client.md)
- [Installation Guide](docs/installation.md)
- [Network Protocol](docs/rfc.txt)
- [Accessibility](docs/Accessibility.md)
- [Technical Comparative](docs/TechnicalComparative.md)
- [Network Choice](docs/NetworkChoice.md)
- [Architecture Choice](docs/ArchitectureChoice.md)

## 📬 Support

<div align="center">

- Create an [Issue](https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson/issues)
- Contact team members via GitHub

</div>

## 📜 License

MIT License - See [LICENSE](LICENSE)

---
Maintained by the R-Type Team 🚀