from conans import ConanFile
from conans import CMake

class ConanConsolepp(ConanFile):
    name = "consolepp"
    version = "0.0.1"
    url = "https://github.com/KazDragon/consolepp"
    author = "KazDragon"
    license = "MIT"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake"
    exports = "*.hpp", "*.in", "*.cpp", "CMakeLists.txt", "*.md", "LICENSE"
    description = "A library for managing a console window"
    requires = ("boost_asio/[>=1.69]@bincrafters/stable",
                "gsl-lite/[>=0.26]@nonstd-lite/stable")
    options = {"shared": [True, False], "withTests": [True, False], "coverage": [True, False], "sanitize" : ["off", "address"]}
    default_options = {"shared": False, "withTests": False, "coverage": False, "sanitize": "off"}

    def requirements(self):
        if (self.options.withTests):
            self.requires("gtest/[>=1.8.1]@bincrafters/stable")

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_SHARED_LIBS"] = self.options.shared
        cmake.definitions["SERVERPP_WITH_TESTS"] = self.options.withTests
        cmake.definitions["SERVERPP_COVERAGE"] = self.options.coverage
        cmake.definitions["SERVERPP_SANITIZE"] = self.options.sanitize
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="include")
        self.copy("*.lib", dst="lib", src="lib", keep_path=False)
        self.copy("*.dll", dst="bin", src="bin", keep_path=False)
        self.copy("*.dylib", dst="bin", src="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.so.*", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["consolepp"]
