#!/usr/bin/env bash

set -e
cd selene
SELENE_DIR=$(pwd)

echo "--------------------------------------------------"
echo "COMMIT $(git rev-parse HEAD)"
echo "--------------------------------------------------"
cmake --version;
echo "--------------------------------------------------"

if [[ -n "${BUILD_TYPE}" ]]; then
  echo "Build type: ${BUILD_TYPE}"
  export LOCAL_BUILD_TYPE="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
fi

export ANDROID_NDK_HOME=$(find $(dirname $(pwd)) -maxdepth 1 -type d -name "android*")
echo "ANDROID_NDK_HOME = ${ANDROID_NDK_HOME}"

echo "Patching wrappers/fs/CMakeLists.txt..."
cp ${SELENE_DIR}/wrappers/fs/CMakeLists_build_boost_fs.txt ${SELENE_DIR}/wrappers/fs/CMakeLists.txt

# CMake invocation
rm -rf build && mkdir -p build && cd build

cmake -G Ninja \
  ${LOCAL_BUILD_TYPE} \
  -DVCPKG_TARGET_TRIPLET="arm64-android" \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=/home/vcpkg/scripts/toolchains/android.cmake \
  -DCMAKE_TOOLCHAIN_FILE=/home/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-27 \
  -DSELENE_BUILD_TESTS=ON \
  -DSELENE_BUILD_EXAMPLES=ON \
  -DSELENE_BUILD_BENCHMARKS=OFF \
  -DSELENE_WARNINGS_AS_ERRORS=ON \
  -DBOOST_SRC_PATH=/home/boost_1_69_0 \
  ..

echo "--------------------------------------------------"
echo "Build all targets..."

cmake --build . -j
export SELENE_DATA_PATH=${SELENE_DIR}/data

echo "--------------------------------------------------"

echo "FINISHED."