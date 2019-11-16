#!/usr/bin/env bash

vcpkg_base_dir=$1
shift
vcpkg_libraries=$@

echo "vcpkg_base_dir=${vcpkg_base_dir}"
echo "vcpkg_libraries=${vcpkg_libraries}"

export BOOST_MODULAR_CMAKE="${vcpkg_base_dir}/vcpkg/ports/boost-modular-build-helper/CMakeLists.txt"
export LN=$(grep -n "WORKING_DIRECTORY" ${BOOST_MODULAR_CMAKE} | awk -F ":" '{print $1}')
gsed -i "${LN}i\        cxxstd=17" ${BOOST_MODULAR_CMAKE}

${vcpkg_base_dir}/vcpkg/bootstrap-vcpkg.sh
${vcpkg_base_dir}/vcpkg/vcpkg install ${vcpkg_libraries}
rm -rf ${vcpkg_base_dir}/vcpkg/buildtrees
rm -rf ${vcpkg_base_dir}/vcpkg/downloads
rm -rf ${vcpkg_base_dir}/vcpkg/packages
