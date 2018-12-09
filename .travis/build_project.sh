#!/usr/bin/env bash

set -e

echo "CC = ${CC}"
echo "CXX = ${CXX}"
echo "ASAN = ${ASAN}"
cmake --version;
${CC} --version;
${CXX} --version;

echo "COMMIT $(git rev-parse HEAD)"
echo "BRANCHES"
git branch
echo "----------"

sudo apt-get update
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y libjpeg-turbo8-dev libpng-dev libopencv-dev libboost-filesystem-dev

if [ -n "${ASAN}" ]; then
  echo "Building with AddressSanitizer enabled..."
  SAN_FLAGS="-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -O1"
  export CXXFLAGS=${SAN_FLAGS}
  export LDFLAGS=${SAN_FLAGS}
fi

# CMake invocation
rm -rf build && mkdir -p build && cd build
cmake -G Ninja \
  -DSELENE_BUILD_TESTS=ON \
  -DSELENE_BUILD_EXAMPLES=ON \
  ..

# Build all targets
cmake --build .
export SELENE_DATA_PATH=../data

# Run tests
./test/selene_tests -d yes

# Run all example binaries
# https://stackoverflow.com/questions/4458120/unix-find-search-for-executable-files
EXAMPLE_BINARIES=$(find -L ./examples/ -maxdepth 1 -type f \( -perm -u=x -o -perm -g=x -o -perm -o=x \))
for file in ${EXAMPLE_BINARIES}
do
  ${file}
done
