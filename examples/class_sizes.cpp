// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/typed/ImageViewTypeAliases.hpp>

#include <cassert>
#include <iostream>
#include <string>

using namespace sln::literals;

int main(/*int argc, char** argv*/)
{
  using namespace sln;
  std::cout << "\n\n";
  std::cout << "sizeof(PixelLength)           == " << sizeof(PixelLength) << '\n';
  std::cout << "sizeof(PixelIndex)            == " << sizeof(PixelIndex) << '\n';
  std::cout << "sizeof(Stride)                == " << sizeof(Stride) << '\n';
  std::cout << "--------------------------------\n";
  std::cout << "sizeof(DataPtr<IM::Mutable>)  == " << sizeof(DataPtr<ImageModifiability::Mutable>) << '\n';
  std::cout << "sizeof(UntypedLayout)         == " << sizeof(UntypedLayout) << "  == PixelLength + PixelLength + int16 + int16 + Stride\n";
  std::cout << "sizeof(UntypedImageSemantics) == " << sizeof(UntypedImageSemantics) << "  == PixelFormat (" << sizeof(PixelFormat) << ") + SampleFormat (" << sizeof(SampleFormat) << ")\n";
  std::cout << '\n';
  std::cout << "sizeof(MutableDynImageView)   == " << sizeof(MutableDynImageView) << '\n';
  std::cout << "sizeof(DynImage<>)            == " << sizeof(DynImage<>) << '\n';
  std::cout << "--------------------------------\n";
  std::cout << "sizeof(DataPtr<IM::Mutable>)  == " << sizeof(DataPtr<ImageModifiability::Mutable>) << '\n';
  std::cout << "sizeof(TypedLayout)           == " << sizeof(TypedLayout) << "  == PixelLength + PixelLength + Stride\n";
  std::cout << '\n';
  std::cout << "sizeof(MutableImageViewY_8u)  == " << sizeof(MutableImageViewY_8u) << '\n';
  std::cout << "sizeof(ImageY_8u)             == " << sizeof(ImageY_8u) << '\n';
  std::cout << '\n';

  return 0;
}
