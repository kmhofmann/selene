# Selene

## Overview

**Selene** is a C++14 image representation, processing and I/O library.

It currently contains, among other things

  * Statically and dynamically typed in-memory pixel and image data representations.
  	* [Pixel\<T, n\>](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Pixel.hpp):
  	Statically typed multi-channel pixel value.
  	  * For example, `Pixel<std:uint8_t, 3>` (shortcut: `Pixel_8u3`) could represent an 8-bit RGB value, while
  	`Pixel<float, 1>` represents a single-channel floating point image.
  	* [Image\<T\>](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Image.hpp):
  	Statically typed class representing a 2-D image, pointing to either owned or non-owned memory.
  	  * Example: `Image<Pixel_8u1> img_gray(320_px, 240_px);  // 8-bit grayscale image of size (320 x 240)`
  	  * Example: `Image<Pixel<double, 10>> img(ptr, width, height);  // view onto 10-channel floating point image data`
  	* [ImageData](https://github.com/kmhofmann/selene/blob/master/src/selene/img/ImageData.hpp):
  	Dynamically typed class representing a 2-D image.
  	  * Its main use case is as an intermediate representation decoded image data (from disk or memory) before conversion
  	to a strongly typed `Image<T>`.
  	* Interoperability with [OpenCV](https://opencv.org/) `cv::Mat` matrices:
  	both wrapping (as view) or copying is supported, in both directions. 

  * Functions for reading and writing image data in JPEG and PNG formats (from/to files or memory). The implementation
  cleanly wraps the *libjpeg* and *libpng* APIs.
  	* [read_jpeg()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGRead.hpp),
  	[read_jpeg_header()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGRead.hpp),
  	[write_jpeg()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGWrite.hpp)
  	* [read_png()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGRead.hpp),
  	[read_png_header()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGRead.hpp),
  	[write_png()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGWrite.hpp)
  	* Convenience functions [read_image()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/IO.hpp)
  	and [write_image()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/IO.hpp), being able to handle
  	both formats.
  	  * Example: `auto img_data = read_image(FileReader("image.png"));`
  	  * Example: `auto img_data = read_image(MemoryReader(data_ptr, size_bytes));`

  * Basic image processing functionality, such as:
    * Image [pixel access](https://github.com/kmhofmann/selene/blob/master/src/selene/img/ImageAccess.hpp) using
    various [interpolation algorithms](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Interpolators.hpp)
    (nearest neighbor, bilinear) and
    [boundary handling strategies](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Accessors.hpp) (no
    check, replicate boundary, zero padding).
      * Example: `const auto u = get<ImageInterpolationMode::Bilinear>(img, 8.5, 10.2);`
      * Example: `const auto v = get<BorderAccessMode::Replicated>(img, -5_idx, 10_idx);`
      * Example: `const auto w = get<ImageInterpolationMode::Bilinear, BorderAccessMode::ZeroPadding>(img, x, y)`
    * [Algorithms](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Algorithms.hpp) to apply point-wise
    operations to images/views.
      * Example: `for_each_pixel(img, [](auto& px){ px += 1; });`
      * Example: `const auto img_2 = transform_pixels(img, [const auto& px]{ return px + 1; });`
    * [Pixel-level](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PixelConversions.hpp) and
    [image-level](https://github.com/kmhofmann/selene/blob/master/src/selene/img/ImageConversions.hpp) conversion
    functions between different pixel formats (e.g. RGB -> Grayscale, etc.).
      * Example: `const auto img_gray = convert_image<PixelFormat::RGB, PixelFormat::Y>(rgb_img);`
      * Example: `const auto img_bgra = convert_image<PixelFormat::RGB, PixelFormat::BGRA>(rgb_img, 255);`
    * [Transformations](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Transformations.hpp)
    such as transposing, flipping and rotating image data. 
      * Example: `const auto img_transposed = transpose(img);`
      * Example: `const auto img_flipped = flip<FlipDirection::Horizontal>(img);`
      * Example: `const auto img_rotated = rotate<RotationDirection::Clockwise90>(img);`

  * Functions for binary IO from and to files or memory. The type of source/sink can be transparent to users of this
  functionality, via static polymorphism.
    * [FileReader](https://github.com/kmhofmann/selene/blob/master/src/selene/io/FileReader.hpp) /
    [FileWriter](https://github.com/kmhofmann/selene/blob/master/src/selene/io/FileWriter.hpp):
    Reading/writing from and to files
    * [MemoryReader](https://github.com/kmhofmann/selene/blob/master/src/selene/io/MemoryReader.hpp) /
    [MemoryWriter](https://github.com/kmhofmann/selene/blob/master/src/selene/io/MemoryWriter.hpp):
    Reading/writing from and to memory (raw pointer locations)
    * [VectorReader](https://github.com/kmhofmann/selene/blob/master/src/selene/io/VectorReader.hpp) /
    [VectorWriter](https://github.com/kmhofmann/selene/blob/master/src/selene/io/VectorWriter.hpp):
    Reading/writing from and to `std::vector<std::uint8_t>`, extending as needed when writing

  * A simple thread pool.
    * [ThreadPool](https://github.com/kmhofmann/selene/blob/master/src/selene/thread/ThreadPool.hpp):
    Concurrent processing of tasks (function calls).
    Offers fairly basic functionality. For more complex tasks, prefer to use a dedicated task concurrency library such
    as [transwarp](https://github.com/bloomen/transwarp).