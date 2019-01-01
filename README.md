# Selene

[![Build Status](https://dev.azure.com/kmhofmann/selene/_apis/build/status/kmhofmann.selene?branchName=master)](https://dev.azure.com/kmhofmann/selene/_build/latest?definitionId=1)

**Selene** is a C++17 image representation, processing, and I/O library, focusing on ease of use and a clean, modern,
type-safe API.

  * [Overview](docs/overview.md): Brief summary of the current feature set.
  * [Rationale](docs/rationale.md): Why implement a new library for image representation, processing, and I/O?
  * [Building the library](docs/building.md): How to build from source.
  * [Dependencies](docs/dependencies.md): Description of optional dependencies.
  * [Installation](docs/installation.md): Installation using package managers.
  * [API documentation and status](docs/documentation.md): API documentation, examples, supported compilers.
  * [Known issues](docs/known_issues.md): Known issues and possible workarounds.
  * [Contribution guidelines](CONTRIBUTING.md): How to contribute to the library.

### Summary

  * Strongly typed image and multi-channel pixel representations, and functions for image data access.
  * Easy-to-use APIs to read and write images in JPEG and PNG formats (leveraging *libjpeg* and *libpng*).
  * Basic image processing algorithms such as color conversions, pixel-wise operations, convolutions, rotation, etc.
  * Lightweight and easy to build using *CMake* on Linux, MacOS, Windows.

### Example

A first impression of the API:

```
// Decode JPEG image data from disk
DynImage img_data = read_image(FileReader("example.jpg"));
assert(img_data.nr_channels() == 3 && img_data.nr_bytes_per_channel() == 1);

// Convert to strongly typed RGB image
Image<PixelRGB_8u> img_rgb = to_image<PixelRGB_8u>(std::move(img_data));
assert(img_rgb.width() > 400_px && img_rgb.height() > 350_px);

// Create non-owning view on part of the image
MutableImageView<PixelRGB_8u> img_part = view(img_rgb, {100_idx, 100_idx, 300_px, 250_px});

// Darken this part
for_each_pixel(img_part, [](auto& px){ px /= 4; });

// Flip this part horizontally
flip_horizontally_in_place(img_part);

// Apply a convolution to this part (1-D Gaussian kernel in x-direction, sigma=5.0, range: 3 std. deviations)
Kernel<double> kernel = gaussian_kernel(5.0, 3.0);
const auto img_part_copy = convolution_x<BorderAccessMode::Unchecked>(img_part, kernel);

// And copy the result back to the original image (i.e. to the view).
clone(img_part_copy, img_part);

// Convert whole image to RGBA, adding semi-transparent alpha channel
const Image<PixelRGBA_8u> img_rgba =
  convert_image<PixelFormat::RGBA>(img_rgb, std::uint8_t{192});

// Encode in-memory to PNG
std::vector<std::uint8_t> encoded_png_data;
write_image(to_dyn_image_view(img_rgba), ImageFormat::PNG,
          VectorWriter(encoded_png_data));

// Write encoded binary data to disk (or do something else with it...)
write_data_contents("example_out.png", encoded_png_data);
```
<!---
### Introductory presentation

Here is a [PDF format presentation](https://selene-lib.org/selene_presentation_20180524.pdf) that introduces Selene
(~v.0.1.0) and some of its features, given at a meetup of the
[Dutch C++ Group](https://www.meetup.com/The-Dutch-Cpp-Group/).

[![Selene: Image representation and I/O in C++14](docs/selene_presentation_preview_small.png)](https://selene-lib.org/selene_presentation_20180524.pdf)
--->

### Package managed

Building from source "at head" using *CMake* is the recommended way to use **Selene**
(see [Building the library](docs/building.md)).
But the library is also available as a

  * [Conan package](https://bintray.com/kmhofmann/conan-repo/selene%3Aselene) on Bintray
  * [vcpkg](https://github.com/Microsoft/vcpkg) port

**Selene**'s own dependencies can be installed using

  * [Conan](https://conan.io/)
  * [vcpkg](https://github.com/Microsoft/vcpkg)

besides OS level package managers or from source.

### Feedback encouraged

Please [send me a message](CONTRIBUTING.md) if you're using **Selene**.
I would love to know what you think of it, and what your use cases are!
Contributions are also very encouraged; see [here](CONTRIBUTING.md) for more information.