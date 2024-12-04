# 🎮 R-Type Project Documentation

Welcome to the official documentation of our R-Type project! This documentation will guide you through the installation, development, and understanding of our modern implementation of the classic R-Type game.

## 🚀 Quick Start

Get the project up and running:

```bash
# Clone the repository
git clone https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson.git

# Navigate to project directory
cd B-CPP-500-MAR-5-2-rtype-jeremy.bisson

# Install dependencies and build
conan install . --output-folder=build --build=missing
cd build
cmake ..
cmake --build . --config Release
```

## 🎯 Key Features

- **Multiplayer Gaming**: Real-time networked gameplay
- **Cross-Platform**: Runs on Windows, Linux, and MacOS
- **Modern Architecture**: Built with ECS pattern
- **Professional Networking**: UDP-based client-server architecture
- **Enhanced Graphics**: SFML-powered rendering

## 📚 Documentation Sections

### Technical Documentation
- [Architecture Overview](./technical/architecture.md)
- [Network Protocol](./technical/network.md)
- [Entity Component System](./technical/ecs.md)
- [Game Systems](./technical/systems.md)

### User Guides
- [Installation Guide](./guides/installation.md)
- [Build Instructions](./guides/building.md)
- [Game Controls](./guides/controls.md)
- [Multiplayer Guide](./guides/multiplayer.md)

### Development
- [Contributing Guidelines](./development/contributing.md)
- [Coding Standards](./development/coding-standards.md)
- [Testing Guide](./development/testing.md)
- [Docker Integration](./development/docker.md)

## 👥 Team

| Name | Role | Contact |
|------|------|---------|
| Jérémy Bisson | Project Lead & Developer | [GitHub](https://github.com/jeremybepitech) |
| Meddi Gueran | Core Developer | [GitHub](https://github.com/MeddiGU) |
| Jérémy Delfino | Network Developer | [GitHub](https://github.com/jeremydelfino) |
| Luca Giglio | Graphics Developer | [GitHub](https://github.com/LaJinxKonAime) |
| Alexandre Vittenet | Engine Developer | [GitHub](https://github.com/SHRaton) |

## 🛠️ Technology Stack

- **Language**: C++17
- **Graphics**: SFML
- **Networking**: Boost.Asio
- **Build System**: CMake
- **Package Manager**: Conan
- **Containerization**: Docker

## 📦 Project Structure

```
r-type/
├── docs/               # Documentation files
├── src/               # Source files
│   ├── client/        # Client implementation
│   ├── server/        # Server implementation
│   ├── components/    # ECS components
│   └── systems/       # Game systems
├── assets/            # Game resources
├── tests/             # Test files
└── docker/            # Docker configuration
```

## 🔄 Development Workflow

1. **Setup Environment**
   - Install required dependencies
   - Configure development environment
   - Set up build tools

2. **Development Process**
   - Follow coding standards
   - Write unit tests
   - Document new features

3. **Testing**
   - Run unit tests
   - Perform integration testing
   - Validate network functionality

4. **Deployment**
   - Build for target platforms
   - Package the application
   - Deploy using Docker (optional)

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](./development/contributing.md) for details on:
- Code style
- Commit messages
- Pull request process
- Development workflow

## 📫 Support

For support and questions:
- Open an issue on GitHub
- Contact the development team
- Check the documentation

---

📅 Last updated: December 2024