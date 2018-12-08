# Selene

## Overview

**Selene** is a C++17 image representation, processing and I/O library.

It currently contains, among other things

  * Statically and dynamically typed in-memory pixel and image data representations.
  	* [Pixel\<T, n\>](../selene/img/pixel/Pixel.hpp):
  	Statically typed multi-channel pixel value.
  	  * For example, `Pixel<std:uint8_t, 3>` (shortcut: `Pixel_8u3`) could represent an 8-bit RGB value, while
  	`Pixel<float, 1>` represents a single-channel floating point image.
  	* [Image\<T\>](../selene/img/typed/Image.hpp):
  	Statically typed class representing a 2-D image, owning its data.
  	  * Example: `Image<Pixel_8u1> img_gray({320_px, 240_px});  // 8-bit grayscale image of size (320 x 240)`
  	* [ImageView\<T\>](../selene/img/typed/ImageView.hpp):
  	Statically typed class representing a 2-D image view, i.e. pointing to non-owned memory.
  	Can be either mutable or constant.
  	  * Example: `MutableImageView<Pixel<double, 10>> img(ptr, {width, height});  // view onto 10-channel floating point image data`
  	* [DynImage](../selene/img/dynamic/DynImage.hpp):
  	Dynamically typed class representing a 2-D image.
  	  * Its main use case is as an intermediate representation for decoded image data (from disk or memory) before
  	  conversion to a strongly typed `Image<T>`.
  	* [DynImageView](../selene/img/dynamic/DynImageView.hpp):
  	Dynamically typed class representing a 2-D image view.
  	Can be either mutable or constant.
  	* [Interoperability](../selene/img/interop/OpenCV.hpp) with [OpenCV](https://opencv.org/) `cv::Mat` matrices:
  	both wrapping (as view) or copying is supported, in both directions. 

  * Functions for reading and writing image data in JPEG and PNG formats (from/to files or memory). The implementation
  cleanly wraps the *libjpeg* and *libpng* APIs.
  	* [read_jpeg()](../selene/img_io/JPEGRead.hpp),
  	[read_jpeg_header()](../selene/img_io/JPEGRead.hpp),
  	[write_jpeg()](../selene/img_io/JPEGWrite.hpp)
  	* [read_png()](../selene/img_io/PNGRead.hpp),
  	[read_png_header()](../selene/img_io/PNGRead.hpp),
  	[write_png()](../selene/img_io/PNGWrite.hpp)
  	* Convenience functions [read_image()](../selene/img_io/IO.hpp)
  	and [write_image()](../selene/img_io/IO.hpp), being able to handle
  	both formats.
  	  * Example: `auto img_data = read_image(FileReader("image.png"));`
  	  * Example: `auto img_data = read_image(MemoryReader(data_ptr, size_bytes));`

  * Basic image processing functionality, such as:
    * Image [pixel access](../selene/img/typed/access/GetPixel.hpp) using
    various [interpolation algorithms](../selene/img/typed/access/Interpolators.hpp)
    (nearest neighbor, bilinear) and
    [boundary handling strategies](../selene/img/typed/access/BorderAccessors.hpp) (no
    check, replicate boundary, zero padding).
      * Example: `const auto u = get<ImageInterpolationMode::Bilinear>(img, 8.5, 10.2);`
      * Example: `const auto v = get<BorderAccessMode::Replicated>(img, -5_idx, 10_idx);`
      * Example: `const auto w = get<ImageInterpolationMode::Bilinear, BorderAccessMode::ZeroPadding>(img, x, y)`
    * [Algorithms](../selene/img_ops/Algorithms.hpp) to apply point-wise
    operations to images/views.
      * Example: `for_each_pixel(img, [](auto& px){ px += 1; });`
      * Example: `const auto img_2 = transform_pixels(img, [const auto& px]{ return px + 1; });`
    * [Pixel-level](../selene/img_ops/PixelConversions.hpp) and
    [image-level](../selene/img_ops/ImageConversions.hpp) conversion
    functions between different pixel formats (e.g. RGB -> Grayscale, etc.).
      * Example: `const auto img_gray = convert_image<PixelFormat::RGB, PixelFormat::Y>(rgb_img);`
      * Example: `const auto img_bgra = convert_image<PixelFormat::RGB, PixelFormat::BGRA>(rgb_img, 255);`
    * [Transformations](../selene/img_ops/Transformations.hpp)
    such as transposing, flipping and rotating image data. 
      * Example: `const auto img_transposed = transpose(img);`
      * Example: `const auto img_flipped = flip<FlipDirection::Horizontal>(img);`
      * Example: `const auto img_rotated = rotate<RotationDirection::Clockwise90>(img);`

  * Functions for binary IO from and to files or memory. The type of source/sink can be transparent to users of this
  functionality, via static polymorphism.
    * [FileReader](../selene/base/io/FileReader.hpp) /
    [FileWriter](../selene/base/io/FileWriter.hpp):
    Reading/writing from and to files
    * [MemoryReader](../selene/base/io/MemoryReader.hpp) /
    [MemoryWriter](../selene/base/io/MemoryWriter.hpp):
    Reading/writing from and to memory (raw pointer locations)
    * [VectorReader](../selene/base/io/VectorReader.hpp) /
    [VectorWriter](../selene/base/io/VectorWriter.hpp):
    Reading/writing from and to `std::vector<std::uint8_t>`, extending as needed when writing
