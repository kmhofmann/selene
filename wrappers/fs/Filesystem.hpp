// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_WRAPPERS_FS_FILESYSTEM_HPP
#define SELENE_WRAPPERS_FS_FILESYSTEM_HPP

/* This file abstracts the necessary filesystem library components used for test, examples, and benchmarks,
 * either using the std::filesystem or the boost::filesystem implementations. */

#if defined(SELENE_WITH_BOOST_FILESYSTEM)
#include <boost/filesystem.hpp>
namespace sln_fs = boost::filesystem;
#else
#include <filesystem>
namespace sln_fs = std::filesystem;
#endif

#endif  // SELENE_WRAPPERS_FS_FILESYSTEM_HPP
