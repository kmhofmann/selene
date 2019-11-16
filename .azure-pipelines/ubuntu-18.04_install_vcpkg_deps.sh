#!/usr/bin/env bash

vcpkg_base_dir=$1
shift
vcpkg_libraries=$@

echo "vcpkg_base_dir=${vcpkg_base_dir}"
echo "vcpkg_libraries=${vcpkg_libraries}"
echo "CC=${CC}"
echo "CXX=${CXX}"
echo "CXXFLAGS=${CXXFLAGS}"
echo "LDFLAGS=${LDFLAGS}"

CC=gcc-8 CXX=g++-8 CXXFLAGS="" LDFLAGS="" ${vcpkg_base_dir}/vcpkg/bootstrap-vcpkg.sh
${vcpkg_base_dir}/vcpkg/vcpkg install ${vcpkg_libraries}
rm -rf ${vcpkg_base_dir}/vcpkg/buildtrees
rm -rf ${vcpkg_base_dir}/vcpkg/downloads
rm -rf ${vcpkg_base_dir}/vcpkg/packages
