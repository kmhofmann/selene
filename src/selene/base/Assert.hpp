// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_ASSERT_HPP
#define SELENE_BASE_ASSERT_HPP

#include <cstdio>

#define SELENE_FORCED_ASSERT(condition) \
{ \
  if (!(condition)) \
  { \
    const int line = __LINE__; \
    const char* const file = __FILE__; \
    const char* const func = __func__; \
    fprintf(stderr, "SELENE_FORCED_ASSERT:\n  %s,\n  %s,\n  line %d\n", \
            file, func, line); \
    std::abort();  \
  } \
}

#ifndef NDEBUG

  #define SELENE_ASSERT(condition) \
  { \
    if (!(condition)) \
    { \
      const int line = __LINE__; \
      const char* const file = __FILE__; \
      const char* const func = __func__; \
      fprintf(stderr, "SELENE_ASSERT:\n  %s,\n  %s,\n  line %d\n", \
              file, func, line); \
      std::abort();  \
    } \
}

#else

  #define SELENE_ASSERT(cond) ((void)0)

#endif // #ifndef NDEBUG

#endif // SELENE_BASE_ASSERT_HPP