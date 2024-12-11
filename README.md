<div align="center">

# R-Type 🚀

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![SFML](https://img.shields.io/badge/SFML-2.5.1-8CC445?logo=SFML&logoColor=white)](https://www.sfml-dev.org/)
[![Windows](https://img.shields.io/badge/Windows-0078D6?logo=windows&logoColor=white)](docs/installation.md)
[![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)](docs/installation.md)
[![macOS](https://img.shields.io/badge/macOS-000000?logo=apple&logoColor=white)](docs/installation.md)
[![CMake](https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=white)](https://cmake.org/)
[![Conan](https://img.shields.io/badge/Conan-6699CB?logo=conan&logoColor=white)](https://conan.io/)
[![GitHub stars](https://img.shields.io/github/stars/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson?style=social)](https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson)

*A modern multiplayer recreation of the legendary shoot'em up arcade game*

[Play Now](#quick-start) • [Documentation](docs/index.md) • [Report Bug](issues) • [Request Feature](issues)

</div>

## 🚀 Quick Start

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

- **Cross-platform**: Seamless gaming across Windows, Linux, and MacOS
- **Multiplayer**: Epic battles with up to 4 players simultaneously
- **Modern Architecture**: Powered by the Entity Component System pattern
- **Network**: Lightning-fast UDP-based client-server architecture
- **Graphics**: Stunning SFML-powered visuals and effects

## 🎯 Controls

- **↑↓←→**: Movement
- **A**: Shoot

## 🛠️ Tech Stack

<div align="center">

[![C++](https://img.shields.io/badge/C++-Solutions-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![SFML](https://img.shields.io/badge/SFML-Gaming-8CC445?style=for-the-badge&logo=SFML&logoColor=white)](https://www.sfml-dev.org/)
[![Boost](https://img.shields.io/badge/Boost-Network-F7901E?style=for-the-badge&logo=boost&logoColor=white)](https://www.boost.org/)
[![CMake](https://img.shields.io/badge/CMake-Build-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)

</div>

## 👥 Team

| Name | Role | GitHub |
|------|------|--------|
| Jérémy Bisson | Project Lead | [![GitHub](https://img.shields.io/github/followers/jeremybepitech?label=follow&style=social)](https://github.com/jeremybepitech) |
| Meddi Gueran | Core Developer | [![GitHub](https://img.shields.io/github/followers/MeddiGU?label=follow&style=social)](https://github.com/MeddiGU) |
| Jérémy Delfino | Network Developer | [![GitHub](https://img.shields.io/github/followers/jeremydelfino?label=follow&style=social)](https://github.com/jeremydelfino) |
| Luca Giglio | Graphics Developer | [![GitHub](https://img.shields.io/github/followers/LaJinxKonAime?label=follow&style=social)](https://github.com/LaJinxKonAime) |
| Alexandre Vittenet | Engine Developer | [![GitHub](https://img.shields.io/github/followers/SHRaton?label=follow&style=social)](https://github.com/SHRaton) |

## 📚 Documentation

[![Documentation](https://img.shields.io/badge/documentation-online-brightgreen.svg)](docs/index.md)

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

- Contact team members via GitHub
- [![Issues](https://img.shields.io/badge/Issues-Create-d73a4a?style=for-the-badge&logo=github)](https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson/issues)

</div>

## 📜 License

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

<div align="center">

Made with ❤️ by the R-Type Team

[![Maintainers](https://img.shields.io/badge/maintainers-R--Type%20Team-blue)](https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson/graphs/contributors)
[![Activity](https://img.shields.io/github/commit-activity/m/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson)](https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson/graphs/commit-activity)

</div>