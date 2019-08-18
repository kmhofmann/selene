#!/usr/bin/env bash

set -e
cd selene
selene_dir=$(pwd)

echo "--------------------------------------------------"
echo "COMMIT $(git rev-parse HEAD)"
echo "--------------------------------------------------"
echo "CC = ${CC}"
echo "CXX = ${CXX}"
echo "CXXFLAGS = ${CXXFLAGS}"
echo "LDFLAGS = ${LDFLAGS}"
echo ""
echo "VCPKG_DIR = ${VCPKG_DIR}"
echo "BUILD_TYPE = " ${BUILD_TYPE}
echo "ASAN = ${ASAN}"
echo "--------------------------------------------------"
cmake --version;
echo "--------------------------------------------------"
${CC} --version;
echo "--------------------------------------------------"
${CXX} --version;
echo "--------------------------------------------------"

if [[ -n "${VCPKG_DIR}" ]]; then
  echo "Building using vcpkg..."
  local_vcpkg_toolchain="-DCMAKE_TOOLCHAIN_FILE=/home/${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake"
  full_vcpkg_dir=/home/${VCPKG_DIR}
fi

if [[ -n "${BUILD_TYPE}" ]]; then
  echo "Build type: ${BUILD_TYPE}"
  local_build_type="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
fi

if [[ -n "${ASAN}" ]]; then
  echo "Building with AddressSanitizer enabled..."
  local_enable_sanitizers="-DSELENE_ENABLE_SANITIZERS=ON"
fi

# CMake invocation
rm -rf build && mkdir -p build && cd build
cmake -G Ninja \
  ${local_vcpkg_toolchain} \
  ${local_build_type} \
  ${local_enable_sanitizers} \
  -DSELENE_BUILD_TESTS=ON \
  -DSELENE_BUILD_EXAMPLES=ON \
  -DSELENE_BUILD_BENCHMARKS=OFF \
  -DSELENE_WARNINGS_AS_ERRORS=ON \
  ..

echo "--------------------------------------------------"
echo "Build all targets..."

cmake --build . -j
export SELENE_DATA_PATH=${selene_dir}/data

echo "--------------------------------------------------"
echo "Run tests..."

./test/selene_tests -d yes

echo "--------------------------------------------------"
echo "Run all example binaries..."

# Run all example binaries
# https://stackoverflow.com/questions/4458120/unix-find-search-for-executable-files
example_binaries=$(find -L ./examples/ -maxdepth 1 -type f \( -perm -u=x -o -perm -g=x -o -perm -o=x \))
for file in ${example_binaries}
do
  echo "EXECUTING ${file}..."
  ${file}
done

echo "--------------------------------------------------"

if [[ -n "${VCPKG_DIR}" ]]; then
    echo "Build an example project using a vcpkg-installed version of Selene..."
    echo "- Copying portfiles"
    cp ${selene_dir}/package/vcpkg/* ${full_vcpkg_dir}/ports/selene

    echo "- Removing selene (shouldn't exist)"
    ${full_vcpkg_dir}/vcpkg remove selene
    echo "- Installing selene from HEAD"
    ${full_vcpkg_dir}/vcpkg install --head selene

    echo "- Invoking CMake on test project"
    cd ${selene_dir}/package/test_vcpkg
    rm -rf build && mkdir -p build && cd build
    cmake -G Ninja ${local_vcpkg_toolchain} ..

    echo "- Building test project"
    cmake --build . -j

    echo "- Running test project"
    ./example
fi

echo "--------------------------------------------------"

echo "FINISHED."