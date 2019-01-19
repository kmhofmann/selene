from conans import ConanFile, CMake


class SeleneConan(ConanFile):
    name = "selene"
    version = "0.3"
    license = "MIT"
    url = "https://github.com/kmhofmann/selene"
    description = "A C++17 image representation, processing and I/O library."

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"
    build_policy = "missing"

    requires = ("libjpeg-turbo/[>=1.5.0]@bincrafters/stable",
                "libpng/[>=1.2.0]@bincrafters/stable",
                "libtiff/[>=4.0.9]@bincrafters/stable")

    exports_sources = ("../../selene*",
                       "../../cmake*",
                       "../../CMakeLists.txt",
                       "../../conanfile.txt",
                       "../../LICENSE")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="")
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("license*", dst="licenses",  ignore_case=True, keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["selene_base",
                              "selene_base_io",
                              "selene_img",
                              "selene_img_io",
                              "selene_img_io_jpeg",
                              "selene_img_io_png",
                              "selene_img_io_tiff",
                              "selene_img_ops",
                              "selene_io"]
