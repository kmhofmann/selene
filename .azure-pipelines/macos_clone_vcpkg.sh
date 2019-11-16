#!/usr/bin/env bash

vcpkg_base_dir=$1
vcpkg_tag=$2

cd ${vcpkg_base_dir}
git clone https://github.com/Microsoft/vcpkg.git
git -C ${vcpkg_base_dir}/vcpkg checkout ${vcpkg_tag}
