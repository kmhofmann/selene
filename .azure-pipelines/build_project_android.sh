#!/usr/bin/env bash

set -e
cd selene
selene_dir=$(pwd)

echo "--------------------------------------------------"
echo "COMMIT $(git rev-parse HEAD)"
echo "--------------------------------------------------"
cmake --version;
echo "--------------------------------------------------"

if [[ -n "${BUILD_TYPE}" ]]; then
  echo "Build type: ${BUILD_TYPE}"
  local_build_type="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
fi

export ANDROID_NDK_HOME=$(find "$(dirname "$(pwd)")" -maxdepth 1 -type d -name "android*")
echo "ANDROID_NDK_HOME = ${ANDROID_NDK_HOME}"
boost_path=$(find /home -maxdepth 1 -type d -name "boost*")

echo "Patching wrappers/fs/CMakeLists.txt..."
cp ${selene_dir}/wrappers/fs/CMakeLists_build_boost_fs.txt ${selene_dir}/wrappers/fs/CMakeLists.txt

# CMake invocation
rm -rf build && mkdir -p build && cd build

cmake -G Ninja \
  ${local_build_type} \
  -DVCPKG_TARGET_TRIPLET="arm64-android" \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=/home/vcpkg/scripts/toolchains/android.cmake \
  -DCMAKE_TOOLCHAIN_FILE=/home/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-27 \
  -DSELENE_BUILD_TESTS=ON \
  -DSELENE_BUILD_EXAMPLES=ON \
  -DSELENE_BUILD_BENCHMARKS=OFF \
  -DSELENE_WARNINGS_AS_ERRORS=ON \
  -DBOOST_SRC_PATH=${boost_path} \
  ..

echo "--------------------------------------------------"
echo "Build all targets..."

cmake --build . -j
export SELENE_DATA_PATH=${selene_dir}/data

echo "--------------------------------------------------"

echo "FINISHED."