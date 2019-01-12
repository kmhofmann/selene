#!/usr/bin/env bash

function print_help {
  echo "This script will install all Selene dependencies via vcpkg, and then proceed to build Selene, including tests, examples, etc."
  echo ""
  echo "Build output will go to ${BUILD_DIR}."
  echo "Auxiliary tools will be installed in ${TOOLS_DIR}."
  echo ""
  echo "Options:"
  echo "  -c    Clean before building"
  echo "  -t    Run unit tests after building"
  echo "  -u    Force vcpkg & dependencies update"
  echo "  -d    Download a local copy of Clang (on Ubuntu)"
  echo "  -X    Start the process"
  echo ""
  echo "To proceed, specify the '-X' option."
}

function check_for_linux {
  if [[ ! "$(uname -sm)" == "Linux x86_64" ]]; then
    echo "This script is meant for execution under 64-bit Linux, but 'uname -sm' returned '$(uname -sm)'."
    exit 1
  fi
}

function check_for_system_tools {
  if [[ ! -e "$(which $1)" ]]; then
    echo "This script requires $1. Please install $1 via your system's package manager."
    TOOLS_ERROR=1
  fi
}

function check_for_cxx17 {
  COMPILER_BIN=$(which c++)
  if [[ ! -z "${CXX}" ]]; then
    COMPILER_BIN=${CXX}
  fi
  CPP_VER=$(echo '' | ${COMPILER_BIN} -x c++ -dM -E -std=c++17 - | grep __cplusplus | awk '{print $3}' | cut -dL -f1)
  if [[ ! "${CPP_VER}" ]] || [[ "${CPP_VER}" -lt "201703" ]]; then
    return 1
  fi
  return 0
}

DISTR_ID=$(lsb_release -i | awk '{print $3}')
DISTR_RELEASE=$(lsb_release -r | awk '{print $2}')

# --- Some script-global variables ---

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR=$(dirname ${SCRIPT_DIR})
TOOLS_DIR=$(dirname ${REPO_DIR})/selene_easy_build_tools
VCPKG_DIR=${TOOLS_DIR}/vcpkg
BUILD_DIR=${REPO_DIR}/build

CMK_VERSION="3.13.2"
CMK_TARNAME="cmake-${CMK_VERSION}-Linux-x86_64.tar.gz"
CMK_URL="https://github.com/Kitware/CMake/releases/download/v${CMK_VERSION}/${CMK_TARNAME}"

if [[ "${DISTR_ID}" == "Ubuntu" ]]; then
  CLANG_VERSION="7.0.1"
  CLANG_DISTR_VERSION="18.04"
  if [[ $(echo "${DISTR_RELEASE} >= 18.04" | bc) -eq 1 ]]; then
    CLANG_DISTR_VERSION="18.04"
  elif [[ $(echo "${DISTR_RELEASE} >= 16.04" | bc) -eq 1 ]]; then
    CLANG_DISTR_VERSION="16.04"
  elif [[ $(echo "${DISTR_RELEASE} >= 14.04" | bc) -eq 1 ]]; then
    CLANG_DISTR_VERSION="14.04"
  fi
  CLANG_TARNAME="clang+llvm-${CLANG_VERSION}-x86_64-linux-gnu-ubuntu-${CLANG_DISTR_VERSION}.tar.xz"
  CLANG_URL="http://releases.llvm.org/${CLANG_VERSION}/${CLANG_TARNAME}"
fi

# --- Handle command line---

while getopts ":ctudXh" opt; do
  case ${opt} in
    c) CLEAN=1 ;;
    t) RUN_UNIT_TESTS=1 ;;
    u) FORCE_VCPKG_UPDATE=1 ;;
    d) DOWNLOAD_CLANG=1 ;;
    X) DO_IT=1 ;;
    h) print_help; exit 0 ;;
    :) echo "Option -$OPTARG requires an argument."; ARGERR=1 ;;
    \?) echo "Invalid option -$OPTARG"; ARGERR=1 ;;
  esac
done

if [[ ! "$DO_IT" ]] ; then
  print_help
  exit 0
fi

# --- Check some prerequisites ---

check_for_linux
check_for_system_tools wget
check_for_system_tools curl
check_for_system_tools unzip
check_for_system_tools tar
check_for_system_tools git
if [[ "${TOOLS_ERROR}" ]]; then
  exit 1
fi

# --- Check for C++17 support ---

if ! check_for_cxx17 && [[ ! "${DOWNLOAD_CLANG}" ]]; then
  echo "The default compiler/standard library does not support C++17."
  echo "There are two options to proceed:"
  echo ""
  echo "1) Install a compiler supporting C++17."
  echo "   a) On Ubuntu, try to install GCC 8 as follows:"
  echo "        $  sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y"
  echo "        $  sudo apt-get update -y"
  echo "        $  sudo apt-get install g++-8 -y"
  echo "      Since this does not change the default compiler, call this script"
  echo "      subsequently with:"
  echo "        $  CC=gcc-8 CXX=g++-8 ./scripts/easy_build.sh -X"
  echo "      Proceed similarly for other distributions/compilers."
  echo "   b) Install GCC or Clang from source, e.g. using the scripts provided here:"
  echo "      https://github.com/kmhofmann/build_stuff"
  echo ""
  echo "2) Let this script download a local copy of Clang. This is supported on Ubuntu."
  echo "   Both Selene and all its dependencies will then be built using Clang/libc++."
  echo "   The resulting libraries may not be ABI compatible with libraries built"
  echo "   using another compiler/standard library combination."
  echo "   To enable, call this script with the '-d' option."
  exit 1
fi

set -e

# --- Download and extract CMake ---

CMK_DIR=${TOOLS_DIR}/$(basename -s .tar.gz ${CMK_TARNAME})
CMK_BINARY=${CMK_DIR}/bin/cmake

if [[ ! -e "${TOOLS_DIR}/${CMK_TARNAME}" ]]; then
  echo "Downloading CMake ${CMK_VERSION}..."
  wget -q -P ${TOOLS_DIR} ${CMK_URL}
fi

if [[ ! -e "${CMK_BINARY}" ]]; then
  echo "Extracting CMake ${CMK_VERSION}..."
  tar xf ${TOOLS_DIR}/${CMK_TARNAME} -C ${TOOLS_DIR}
fi

# --- Download local copy of Clang, if specified ---

if [[ "${DOWNLOAD_CLANG}" ]]; then
  CLANG_DIR=${TOOLS_DIR}/$(basename -s .tar.xz ${CLANG_TARNAME})
  CLANG_BINARY=${CLANG_DIR}/bin/clang

  if [[ ! "${CLANG_TARNAME}" ]]; then
    echo "ERROR: Cannot determine correct version of Clang release to download."
    echo "(You might not be on Ubuntu?)"
    exit 1
  fi

  if [[ ! -e "${TOOLS_DIR}/${CLANG_TARNAME}" ]]; then
    echo "Downloading Clang ${CLANG_VERSION}..."
    wget -q -P ${TOOLS_DIR} ${CLANG_URL}
  fi

  if [[ ! -e "${CLANG_BINARY}" ]]; then
    echo "Extracting Clang ${CLANG_VERSION}..."
    tar xf ${TOOLS_DIR}/${CLANG_TARNAME} -C ${TOOLS_DIR}
  fi

  echo "Setting environment variables {CC, CXX, CXXFLAGS, LDFLAGS, LD_LIBRARY_PATH} to use Clang ${CLANG_VERSION}."
  export CC=${CLANG_DIR}/bin/clang
  export CXX=${CLANG_DIR}/bin/clang++
  export CXXFLAGS="-stdlib=libc++ ${CXXFLAGS}"
  export LDFLAGS="-stdlib=libc++ ${LDFLAGS}"
  export LD_LIBRARY_PATH="${CLANG_DIR}/lib:${LD_LIBRARY_PATH}"
  SET_CLANG_ENV_VARS=1
fi

# --- Download and bootstrap vcpkg ---

if [[ ! -d "${VCPKG_DIR}" ]]; then
  echo "Cloning vcpkg..."
  git -C ${REPO_DIR} clone https://github.com/Microsoft/vcpkg.git ${VCPKG_DIR}
elif [[ "${FORCE_VCPKG_UPDATE}" ]]; then
  echo "Updating vcpkg..."
  git -C ${VCPKG_DIR} reset HEAD --hard
  git -C ${VCPKG_DIR} pull --rebase
fi

if [[ ! -e "${VCPKG_DIR}/vcpkg" ]] || [[ "${FORCE_VCPKG_UPDATE}" ]]; then
  echo "Bootstrapping vcpkg..."
  if [[ "${SET_CLANG_ENV_VARS}" ]]; then
    # Make sure we link against libc++fs
    sed -i 's/c++experimental/c++fs/g' ${VCPKG_DIR}/toolsrc/CMakeLists.txt
  fi
  ${VCPKG_DIR}/bootstrap-vcpkg.sh
fi

# --- Install dependencies via vcpkg ---

echo "Installing dependencies via vcpkg..."
${VCPKG_DIR}/vcpkg install libjpeg-turbo libpng tiff opencv boost-filesystem benchmark

if [[ "${FORCE_VCPKG_UPDATE}" ]]; then
  ${VCPKG_DIR}/vcpkg upgrade --no-dry-run
fi

# --- Build Selene using CMake ---

mkdir -p ${BUILD_DIR}
if [[ "${CLEAN}" ]] ; then
  echo "Cleaning Selene build..."
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

CMK_GENERATOR=""
if [[ $(which ninja) ]]; then
  CMK_GENERATOR="-G Ninja"
fi

echo "Generating build files using CMake..."
${CMK_BINARY} ${CMK_GENERATOR} -B ${BUILD_DIR} \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake \
  -DSELENE_BUILD_TESTS=ON \
  -DSELENE_BUILD_EXAMPLES=ON \
  -DSELENE_BUILD_BENCHMARKS=ON \
  ${REPO_DIR}
echo "Building Selene..."
${CMK_BINARY} --build ${BUILD_DIR} -j ${CLEAN_BUILD_ARG}

# --- Run unit tests, if desired ---

if [[ "${RUN_UNIT_TESTS}" ]] ; then
  echo "Running unit tests..."
  SELENE_DATA_PATH=${REPO_DIR}/data ${BUILD_DIR}/test/selene_tests
fi

# --- Finish ---

echo ""
echo "Done."
echo "- The unit test executable is in ${BUILD_DIR}/test/."
echo "- The example executables are in ${BUILD_DIR}/examples/."
echo "- You may need to call \"export SELENE_DATA_PATH=${REPO_DIR}/data\" before running either."
if [[ "${SET_CLANG_ENV_VARS}" ]]; then
  echo "- You may need to set \"export LD_LIBRARY_PATH=${CLANG_DIR}/lib:\${LD_LIBRARY_PATH}\" before running any executable."
fi
