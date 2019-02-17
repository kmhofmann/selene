#!/usr/bin/env bash

set -e
cd selene
SELENE_DIR=$(pwd)

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
  export LOCAL_VCPKG_TOOLCHAIN="-DCMAKE_TOOLCHAIN_FILE=/home/${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake"
  FULL_VCPKG_DIR=/home/${VCPKG_DIR}
fi

if [[ -n "${BUILD_TYPE}" ]]; then
  echo "Build type: ${BUILD_TYPE}"
  export LOCAL_BUILD_TYPE="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
fi

if [[ -n "${ASAN}" ]]; then
  echo "Building with AddressSanitizer enabled..."
  export LOCAL_ENABLE_SANITIZERS="-DSELENE_ENABLE_SANITIZERS=ON"
fi

# CMake invocation
rm -rf build && mkdir -p build && cd build
cmake -G Ninja \
  ${LOCAL_VCPKG_TOOLCHAIN} \
  ${LOCAL_BUILD_TYPE} \
  ${LOCAL_ENABLE_SANITIZERS} \
  -DSELENE_BUILD_TESTS=ON \
  -DSELENE_BUILD_EXAMPLES=ON \
  -DSELENE_BUILD_BENCHMARKS=OFF \
  -DSELENE_WARNINGS_AS_ERRORS=ON \
  ..

echo "--------------------------------------------------"
echo "Build all targets..."

cmake --build . -j
export SELENE_DATA_PATH=${SELENE_DIR}/data

echo "--------------------------------------------------"
echo "Run tests..."

./test/selene_tests -d yes

echo "--------------------------------------------------"
echo "Run all example binaries..."

# Run all example binaries
# https://stackoverflow.com/questions/4458120/unix-find-search-for-executable-files
EXAMPLE_BINARIES=$(find -L ./examples/ -maxdepth 1 -type f \( -perm -u=x -o -perm -g=x -o -perm -o=x \))
for file in ${EXAMPLE_BINARIES}
do
  echo "EXECUTING ${file}..."
  ${file}
done

echo "--------------------------------------------------"

if [[ -n "${VCPKG_DIR}" ]]; then
    echo "Build an example project using a vcpkg-installed version of Selene..."
    echo "- Copying portfiles"
    cp ${SELENE_DIR}/package/vcpkg/* ${FULL_VCPKG_DIR}/ports/selene

    echo "- Removing selene (shouldn't exist)"
    ${FULL_VCPKG_DIR}/vcpkg remove selene
    echo "- Installing selene from HEAD"
    ${FULL_VCPKG_DIR}/vcpkg install --head selene

    echo "- Invoking CMake on test project"
    cd ${SELENE_DIR}/package/test_vcpkg
    rm -rf build && mkdir -p build && cd build
    cmake -G Ninja ${LOCAL_VCPKG_TOOLCHAIN} ..

    echo "- Building test project"
    cmake --build . -j

    echo "- Running test project"
    ./example
fi

echo "--------------------------------------------------"

echo "FINISHED."