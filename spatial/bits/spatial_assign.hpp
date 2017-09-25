// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_assign.hpp
 *  Defines the \ref spatial::details::assign() function, which is pretty much
 *  the same thing as the C++11 std::tie() function. Hence when the library is
 *  moved to C++11 this function should disappear in place of the standard.
 */

#include <utility>

#ifndef SPATIAL_ASSIGN_HPP
#define SPATIAL_ASSIGN_HPP

namespace spatial
{
  namespace details
  {
    template <typename Type1, typename Type2>
    inline void
    assign(Type1& first, Type2& second, const std::pair<Type1, Type2>& pair)
    {
      first = pair.first;
      second = pair.second;
    }
  }
}

#endif // SPATIAL_ASSIGN_HPP
