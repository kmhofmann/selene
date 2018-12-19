#!/usr/bin/env bash

#set -e
#set -x

# TODO:
# - Add option to clean (or not to)
# - Compiler C++17 support check
#   (something like echo '' | g++ -x c++ -dM -E -std=c++17 - | grep __cplusplus | awk '{ print $3}' | cut -dL -f1)
# - CMake version check

function print_help {
  echo "..."
}

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_DIR=$(dirname ${SCRIPT_DIR})
VCPKG_DIR=$(dirname ${REPO_DIR})/selene_vcpkg
BUILD_DIR=${REPO_DIR}/build

while getopts ":cXh" opt; do
  case ${opt} in
    c) CLEAN=1 ;;
    X) DO_IT=1 ;;
    h) print_help; exit 0 ;;
    :) echo "Option -$OPTARG requires an argument."; ARGERR=1 ;;
    \?) echo "Invalid option -$OPTARG"; ARGERR=1 ;;
  esac
done

if [ ! "$DO_IT" ] ; then
  echo "This script will install all Selene dependencies via vcpkg, and then proceed to"
  echo "build Selene, including tests, examples, etc."
  echo ""
  echo "vcpkg will be cloned to ${VCPKG_DIR}."
  echo "Build output will go to ${BUILD_DIR}."
  exit 0
fi

CUR_DIR=$(pwd)

if [ ! -d "${VCPKG_DIR}" ]; then
  git -C ${REPO_DIR} clone https://github.com/Microsoft/vcpkg.git ${VCPKG_DIR}
else
  git -C ${VCPKG_DIR} pull --rebase
fi

if [ ! -e "${VCPKG_DIR}/vcpkg" ]; then
  ${VCPKG_DIR}/bootstrap-vcpkg.sh
fi

function install_vcpkg_package {
  ${VCPKG_DIR}/vcpkg list | grep -q $1
  if [ $? != 0 ]; then
    ${VCPKG_DIR}/vcpkg install $1
  else
    ${VCPKG_DIR}/vcpkg upgrade $1 --no-dry-run
  fi
}

install_vcpkg_package libjpeg-turbo
install_vcpkg_package libpng
install_vcpkg_package opencv
install_vcpkg_package boost-filesystem
install_vcpkg_package benchmark

mkdir -p ${BUILD_DIR}
if [ "$CLEAN" ] ; then
  rm -f ${BUILD_DIR}/CMakeCache.txt
  rm -f ${BUILD_DIR}/build.ninja
  rm -f ${BUILD_DIR}/rules.ninja
  rm -f ${BUILD_DIR}/cmake_install.cmake
  rm -f ${BUILD_DIR}/compile_commands.json
  rm -rf ${BUILD_DIR}/CMakeFiles
  rm -rf ${BUILD_DIR}/benchmark
  rm -rf ${BUILD_DIR}/examples
  rm -rf ${BUILD_DIR}/selene
  rm -rf ${BUILD_DIR}/test
  CLEAN_BUILD_ARG="--clean-first"
fi

GENERATOR=""
if [ $(which ninja) ]; then
  GENERATOR="-G Ninja"
fi

cd ${BUILD_DIR}
cmake ${GENERATOR} \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake \
  -DSELENE_BUILD_TESTS=ON \
  -DSELENE_BUILD_EXAMPLES=ON \
  -DSELENE_BUILD_BENCHMARKS=ON \
  ${REPO_DIR}
cmake --build ${BUILD_DIR} -j ${CLEAN_BUILD_ARG}

echo ""
echo "Done."
echo "- The unit test executable is in ${BUILD_DIR}/test/."
echo "- The example executables are in ${BUILD_DIR}/examples/."
echo "- You may need to call \"export SELENE_DATA_PATH=${REPO_DIR}/data\" before running either."
cd ${CUR_DIR}
