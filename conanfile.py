from conans import ConanFile
from conans import CMake

class ConanConsolepp(ConanFile):
    name = "consolepp"
    url = "https://github.com/KazDragon/consolepp"
    author = "KazDragon"
    license = "MIT"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake"
    exports = "*.hpp", "*.in", "*.cpp", "CMakeLists.txt", "*.md", "LICENSE"
    description = "A library for interacting with the Console"
    #requires = ("boost/[>=1.69]")
    build_requires = ("gtest/[>=1.8.1]")
    options = {"shared": [True, False], "coverage": [True, False], "sanitize" : ["off", "address"]}
    default_options = {"shared": False, "coverage": False, "sanitize": "off"}

    #def configure(self):
        #self.options["boost"].without_test = True

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_SHARED_LIBS"] = self.options.shared
        cmake.definitions["CONSOLEPP_COVERAGE"] = self.options.coverage
        cmake.definitions["CONSOLEPP_SANITIZE"] = self.options.sanitize
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
