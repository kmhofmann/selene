// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_WRITER_MODE_HPP
#define SELENE_IO_WRITER_MODE_HPP

/// @file

namespace selene {
namespace io {

/** \brief Describes the IO write mode: write from start, or append.
 */
enum class WriterMode : unsigned char
{
  Write = 0,  ///< Overwrite/destroy existing contents.
  Append = 1  ///< Append to existing contents.
};

}  // namespace io
}  // namespace selene

#endif  // SELENE_IO_WRITER_MODE_HPP
