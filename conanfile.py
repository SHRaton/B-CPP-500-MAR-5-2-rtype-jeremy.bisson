from conan import ConanFile

class MyConanFile(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "sfml/2.6.0",
        "boost/1.83.0"
    ]
    generators = "CMakeDeps", "CMakeToolchain"