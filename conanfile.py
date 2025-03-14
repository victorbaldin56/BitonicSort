from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain

class MatrixRecipe(ConanFile):
  name = "bitonic_sort"
  version = "1.0"
  user = "victorbaldin56"
  settings = "os", "compiler", "build_type", "arch"
  generators = "CMakeDeps"

  def requirements(self):
    self.requires("opencl-clhpp-headers/2023.12.14")
    self.requires("benchmark/1.9.1")

  def generate(self):
    tc = CMakeToolchain(self)
    tc.generate()

  def build(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.build()
    if self.options.testing:
      cmake.test()