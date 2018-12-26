#!/usr/bin/env bash

set -x
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_DIR=${SCRIPT_DIR}/..

export CC=clang
export CXX=clang++

function build_everything {
  CMK_GENERATOR=""
  if [ $(which ninja) ]; then
    CMK_GENERATOR="-G Ninja"
  fi

  rm -f CMakeCache.txt
  rm -rf CMakeFiles
  cmake ${CMK_GENERATOR} -DSELENE_BUILD_TESTS=ON -DSELENE_BUILD_EXAMPLES=ON ..
  cmake --build . -j --clean-first
}

function run_all_binaries {
  ./test/selene_tests -d yes

  # https://stackoverflow.com/questions/4458120/unix-find-search-for-executable-files
  EXAMPLE_BINARIES=$(find -L ./examples/ -maxdepth 1 -type f \( -perm -u=x -o -perm -g=x -o -perm -o=x \))
  for file in ${EXAMPLE_BINARIES}
  do
    ${file}
  done
}

# -------------------------------------------------------------------
# Build and test with AddressSanitizer and UndefinedBehaviorSanitizer
# -------------------------------------------------------------------

mkdir -p ${REPO_DIR}/_build_with_asan_ubsan
cd ${REPO_DIR}/_build_with_asan_ubsan

SAN_FLAGS="-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -O1"
export CXXFLAGS=${SAN_FLAGS}
export LDFLAGS=${SAN_FLAGS}
build_everything
run_all_binaries

# -----------------------------------
# Build and test with ThreadSanitizer
# (Currently disabled, since there is no multithreading in the library...)
# -----------------------------------

#mkdir -p ${REPO_DIR}/_build_with_tsan
#cd ${REPO_DIR}/_build_with_tsan

#SAN_FLAGS="-fsanitize=thread -g -O1"
#export CXXFLAGS=${SAN_FLAGS}
#export LDFLAGS=${SAN_FLAGS}
#export TSAN_OPTIONS="second_deadlock_stack=1"

#build_everything
#run_all_binaries

# --------
# Clean up
# --------

rm -rf ${REPO_DIR}/_build_with_*
