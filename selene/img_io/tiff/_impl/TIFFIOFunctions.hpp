// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_IMPL_TIFF_IO_FUNCTIONS_HPP
#define SELENE_IMG_IO_IMPL_TIFF_IO_FUNCTIONS_HPP

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/base/Assert.hpp>

#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <type_traits>

#include <tiff.h>
#include <tiffio.h>

namespace sln::impl::tiff {

template <typename Source> struct SourceStruct
{
  Source* source{nullptr};
  std::ptrdiff_t start_pos{0};

  SourceStruct() = default;

  explicit SourceStruct(Source* source_) : source(source_), start_pos(source->position())
  { }
};

template <typename Sink> struct SinkStruct
{
  Sink* sink{nullptr};
  std::ptrdiff_t start_pos{0};

  SinkStruct() = default;

  explicit SinkStruct(Sink* sink_) : sink(sink_), start_pos(sink->position())
  { }
};

// Functions to be passed to TIFFClientOpen for **reading**:
// ---------------------------------------------------------

template <typename Source>
tmsize_t r_read_func(thandle_t data, void* buf, tmsize_t size)
{
  auto ss = reinterpret_cast<SourceStruct<Source>*>(data);

  const auto nr_bytes_read =
      ss->source->template read<std::uint8_t>(static_cast<std::uint8_t*>(buf), static_cast<std::size_t>(size));
  SELENE_ASSERT(nr_bytes_read == static_cast<std::size_t>(size));
  return static_cast<tmsize_t>(nr_bytes_read);
}

template <typename Source>
toff_t r_seek_func(thandle_t data, toff_t offset, int mode) // mode one of {SEEK_SET, SEEK_CUR, SEEK_END}
{
  auto ss = reinterpret_cast<SourceStruct<Source>*>(data);

  switch (mode)
  {
    case SEEK_SET: ss->source->seek_abs(static_cast<ptrdiff_t>(offset)); break;
    case SEEK_CUR: ss->source->seek_rel(static_cast<ptrdiff_t>(offset)); break;
    case SEEK_END: ss->source->seek_end(static_cast<ptrdiff_t>(offset)); break;
    default: break;
  }
  return static_cast<toff_t>(ss->source->position() - ss->start_pos);
}

template <typename Source>
toff_t r_size_func(thandle_t data)
{
  auto ss = reinterpret_cast<SourceStruct<Source>*>(data);

  const auto cur_pos = ss->source->position();
  ss->source->seek_end(0);
  const auto end_pos = ss->source->position();
  ss->source->seek_abs(cur_pos);
  return static_cast<toff_t>(end_pos);
}

template <typename Source>
tmsize_t r_write_func([[maybe_unused]] thandle_t data, [[maybe_unused]] void* buf, [[maybe_unused]] tmsize_t size)
{
  return 0;
}

template <typename Source>
int r_close_func(thandle_t /*data*/)
{
  return 0;
}

template <typename Source>
int r_map_func([[maybe_unused]] thandle_t data, [[maybe_unused]] void** base, [[maybe_unused]] toff_t* size)
{
  return 0;
}

template <typename Source>
void r_unmap_func([[maybe_unused]] thandle_t data, [[maybe_unused]] void* base, [[maybe_unused]] toff_t size)
{
}


// Functions to be passed to TIFFClientOpen for **writing**:
// ---------------------------------------------------------

template <typename Sink>
tmsize_t w_write_func([[maybe_unused]] thandle_t data, [[maybe_unused]] void* buf, [[maybe_unused]] tmsize_t size)
{
  auto ss = reinterpret_cast<SinkStruct<Sink>*>(data);

  const auto nr_bytes_written =
      ss->sink->template write<std::uint8_t>(static_cast<std::uint8_t*>(buf), static_cast<std::size_t>(size));
  SELENE_ASSERT(nr_bytes_written == static_cast<std::size_t>(size));
  return static_cast<tmsize_t>(nr_bytes_written);
}

template <typename Sink>
toff_t w_seek_func(thandle_t data, toff_t offset, int mode) // mode one of {SEEK_SET, SEEK_CUR, SEEK_END}
{
  auto ss = reinterpret_cast<SinkStruct<Sink>*>(data);

  switch (mode)
  {
    case SEEK_SET: ss->sink->seek_abs(static_cast<ptrdiff_t>(offset)); break;
    case SEEK_CUR: ss->sink->seek_rel(static_cast<ptrdiff_t>(offset)); break;
    case SEEK_END: ss->sink->seek_end(static_cast<ptrdiff_t>(offset)); break;
    default: break;
  }
  return static_cast<toff_t>(ss->sink->position() - ss->start_pos);
}

template <typename Sink>
toff_t w_size_func(thandle_t data)
{
  auto ss = reinterpret_cast<SinkStruct<Sink>*>(data);

  const auto cur_pos = ss->sink->position();
  ss->sink->seek_end(0);
  const auto end_pos = ss->sink->position();
  ss->sink->seek_abs(cur_pos);
  return static_cast<toff_t>(end_pos);
}

template <typename Sink>
tmsize_t w_read_func([[maybe_unused]] thandle_t data, [[maybe_unused]] void* buf, [[maybe_unused]] tmsize_t size)
{
  auto ss = reinterpret_cast<SinkStruct<Sink>*>(data);

  const auto nr_bytes_read =
      ss->sink->template read<std::uint8_t>(static_cast<std::uint8_t*>(buf), static_cast<std::size_t>(size));
  SELENE_ASSERT(nr_bytes_read == static_cast<std::size_t>(size));
  return static_cast<tmsize_t>(nr_bytes_read);
}

template <typename Sink>
int w_close_func(thandle_t /*data*/)
{
  return 0;
}

template <typename Sink>
int w_map_func([[maybe_unused]] thandle_t data, [[maybe_unused]] void** base, [[maybe_unused]] toff_t* size)
{
  return 0;
}

template <typename Sink>
void w_unmap_func([[maybe_unused]] thandle_t data, [[maybe_unused]] void* base, [[maybe_unused]] toff_t size)
{
}

}  // namespace sln::impl::tiff

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_IMPL_TIFF_IO_FUNCTIONS_HPP
