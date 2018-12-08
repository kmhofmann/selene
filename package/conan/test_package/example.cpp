#include "selene/base/Allocators.hpp"
#include "selene/img/typed/Image.hpp"

int main() {
  using namespace sln::literals;
  sln::NewAllocator::allocate(256);
  sln::Image_8u1 img(20_px, 20_px);

  return 0;
}
