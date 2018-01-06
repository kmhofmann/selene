#!/usr/bin/env bash

set -x

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_DIR=${SCRIPT_DIR}/..

CLANG_TIDY_OPTS="-header-filter=.*"
COMPILER_INCLUDE_OPTS="-I${REPO_DIR}/src -I${REPO_DIR}/test -I${REPO_DIR}/external/Catch2/single_include"
COMPILER_PREPROC_OPTS="-DSELENE_WITH_LIBJPEG -DSELENE_WITH_LIBPNG -DSELENE_WITH_OPENCV"
COMPILER_OPTS="${COMPILER_INCLUDE_OPTS} ${COMPILER_PREPROC_OPTS} -std=c++14"

find ${REPO_DIR}/src/  -type f -iname *.cpp -print0 | \
    xargs -0 -t -I % clang-tidy ${CLANG_TIDY_OPTS} % -- ${COMPILER_OPTS}
find ${REPO_DIR}/test/ -type f -iname *.cpp -print0 | \
   xargs -0 -t -I % clang-tidy ${CLANG_TIDY_OPTS} % -- ${COMPILER_OPTS}