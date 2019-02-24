// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TRANSFORMATION_DIRECTIONS_HPP
#define SELENE_IMG_TRANSFORMATION_DIRECTIONS_HPP

/// @file

namespace sln {

/// \addtogroup group-img-ops
/// @{

/** \brief Describes the flip direction. */
enum class FlipDirection
{
  Horizontal,  ///< Horizontal flip.
  Vertical,  ///< Vertical flip.
  Both,  ///< Combined horizontal and vertical flip.
};

/** \brief Describes the rotation direction. */
enum class RotationDirection
{
  Clockwise0,  ///< Rotation by 0 degrees clockwise.
  Clockwise90,  ///< Rotation by 90 degrees clockwise.
  Clockwise180,  ///< Rotation by 180 degrees clockwise.
  Clockwise270,  ///< Rotation by 270 degrees clockwise.
  Counterclockwise0,  ///< Rotation by 0 degrees counterclockwise.
  Counterclockwise90,  ///< Rotation by 90 degrees counterclockwise.
  Counterclockwise180,  ///< Rotation by 180 degrees counterclockwise.
  Counterclockwise270,  ///< Rotation by 270 degrees counterclockwise.
};

}  // namespace sln

#endif  // SELENE_IMG_TRANSFORMATION_DIRECTIONS_HPP
