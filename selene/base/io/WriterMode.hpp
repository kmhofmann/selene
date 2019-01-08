// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_WRITER_MODE_HPP
#define SELENE_IO_WRITER_MODE_HPP

/// @file

namespace sln {

/** \brief Describes the IO write mode: write from start, or append.
 */
enum class WriterMode
{
  Write = 0,  ///< Overwrite/destroy existing contents.
  Append = 1  ///< Append to existing contents.
};

}  // namespace sln

#endif  // SELENE_IO_WRITER_MODE_HPP
