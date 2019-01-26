#include "selene/base/Allocators.hpp"
#include "selene/img/typed/Image.hpp"
#include "selene/img/typed/ImageTypeAliases.hpp"
#include "selene/img_ops/ImageConversions.hpp"

#include <iostream>

int main() {
  using namespace sln::literals;
  sln::NewAllocator::allocate(256);
  sln::Image_8u3 img0({20_px, 20_px});
  const auto img1 = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::Y>(img0);

  std::cout << "Done.\n";
  return 0;
}
