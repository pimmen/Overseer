// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   equal_iterator.hpp
 *  Provides equal_iterator and all the functions around it.
 */

#ifndef SPATIAL_EQUAL_ITERATOR_HPP
#define SPATIAL_EQUAL_ITERATOR_HPP

#include "spatial.hpp"
#include "bits/spatial_equal.hpp"

namespace spatial
{
  /**
   *  This structure defines a pair of mutable equal iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see equal_iterator
   */
  template <typename Container>
  struct equal_iterator_pair
    : std::pair<equal_iterator<Container>,
                equal_iterator<Container> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<equal_iterator<Container>,
                      equal_iterator<Container> > Base;

    //! Empty constructor.
    equal_iterator_pair() { }

    //! Regular constructor that builds a equal_iterator_pair out of 2
    //! equal_iterators.
    equal_iterator_pair(const equal_iterator<Container>& a,
                        const equal_iterator<Container>& b)
          : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant equal iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see equal_iterator
   */
  template <typename Container>
  struct equal_iterator_pair<const Container>
    : std::pair<equal_iterator<const Container>,
                equal_iterator<const Container> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<equal_iterator<const Container>,
                      equal_iterator<const Container> > Base;

    //! Empty constructor.
    equal_iterator_pair() { }

    //! Regular constructor that builds a equal_iterator_pair out of 2
    //! equal_iterators.
    equal_iterator_pair(const equal_iterator<const Container>& a,
                        const equal_iterator<const Container>& b)
      : Base(a, b) { }

    //! Convert a mutable equal iterator pair into a const equal iterator
    //! pair.
    equal_iterator_pair(const equal_iterator_pair<Container>& p)
      : Base(p.first, p.second) { }
  };

  /**
   *  Creates a pair of iterator that represent the range of element in the
   *  container that are equal to the model given.
   *
   *  \tparam Container The type of the container iterated.
   *  \param container The container being iterated.
   *  \param model The key to find in \c container.
   */
  ///@(
  template <typename Container>
  inline equal_iterator_pair<Container>
  equal_range(Container& container,
              const typename equal_iterator<Container>::key_type& model)
  {
    return equal_iterator_pair<Container>
      (equal_begin(container, model), equal_end(container, model));
  }

  template <typename Container>
  inline equal_iterator_pair<const Container>
  equal_range(const Container& container,
              const typename equal_iterator<Container>::key_type& model)
  {
    return equal_iterator_pair<const Container>
      (equal_begin(container, model), equal_end(container, model));
  }

  template <typename Container>
  inline equal_iterator_pair<const Container>
  equal_crange(const Container& container,
               const typename equal_iterator<Container>::key_type& model)
  {
    return equal_iterator_pair<const Container>
      (equal_begin(container, model), equal_end(container, model));
  }
  ///@)

} // namespace spatial

#endif // SPATIAL_EQUAL_ITERATOR_HPP
