// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file  spatial_overlap_region.hpp
 *
 *  Contains the definition of \overlap_region_iterator. These iterators walk
 *  through all items in the container that are contained within an orthogonal
 *  region bounded by a \c low and a \c high value.
 */

#ifndef SPATIAL_OVERLAP_REGION_HPP
#define SPATIAL_OVERLAP_REGION_HPP

#include "spatial_region.hpp"

namespace spatial
{
  /**
   *  This class is a model of \region_predicate that matches any keys that is
   *  overlapping with a given test box. In this predicate, keys should be
   *  representing boxes, and not points.
   *
   *  In this predicate, the Compare functor is expected to be a model of
   *  \generalized_compare, in order to compare information on different
   *  dimensions.
   *
   *  Additionally, in order to interpret the boxe's coordinates appropriately,
   *  overlap_bounds expects a Layout template argument. Layout is one of:
   *  \li \ref llhh_layout_tag,
   *  \li \ref lhlh_layout_tag,
   *  \li \ref hhll_layout_tag,
   *  \li \ref hlhl_layout_tag.
   *
   *  Each layout provides information on how to interpret the coordinates
   *  returned for each dimension of the boxes keys.
   *
   *  For a given target box \f$_P{(x, y)}\f$, this region predicate matches any
   *  box \f$_B{(x, y)}\f$ in a space of rank \f$r\f$ such as:
   *
   *  \f[
   *  \_sum{i=1}^{r} \left( _B{x_i} \le _P{x_i} \le _B{y_i}
   *                        \; or \; _B{x_i} \le _P{y_i} \le _B{y_i} \right)
   *  \f]
   *
   *  This predicate is used together with \region_iterator to make up
   *  \overlap_region_iterator.
   *
   *  \tparam Key A key type representing boxes.
   *  \tparam Compare A model of \generalized_compare
   *  \tparam Layout One of \ref llhh_layout_tag, \ref lhlh_layout_tag, \ref
   *  hhll_layout_tag or \ref hlhl_layout_tag.
   *  \concept_region_predicate
   */
  template <typename Key, typename Compare,
            typename Layout = llhh_layout_tag>
  class overlap_bounds
    : private Compare
  {
  public:
    /**
     *  The default constructor leaves everything un-initialized
     */
    overlap_bounds() : Compare(), _target() { }

    /**
     *  Set the target box and the comparator to the appropriate value.
     */
    overlap_bounds(const Compare& compare, const Key& target)
      : Compare(compare), _target(target)
    { }

    /**
     *  The operator that tells wheather the point is in region or not.
     */
    relative_order
    operator()(dimension_type dim, dimension_type rank, const Key& key) const
    {
      return overlap_bounds_impl(dim, rank, key, Layout());
    }

  private:
    /**
     *  The box value that will be used for overlaping comparison.
     */
    Key _target;

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, llhh_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim, key, dim + (rank >> 1), _target)
           ? matching : above)
        : (Compare::operator()(dim - (rank >> 1), _target, dim, key)
           ? matching : below);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type, const Key& key, lhlh_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim, key, dim + 1, _target) ? matching : above)
        : (Compare::operator()(dim - 1, _target, dim, key) ? matching : below);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type rank, const Key& key, hhll_layout_tag)
    const
    {
      return (dim < (rank >> 1))
        ? (Compare::operator()(dim + (rank >> 1), _target, dim, key)
           ? matching : below)
        : (Compare::operator()(dim, key, dim - (rank >> 1), _target)
           ? matching : above);
    }

    relative_order overlap_bounds_impl
    (dimension_type dim, dimension_type, const Key& key, hlhl_layout_tag)
    const
    {
      return ((dim % 2) == 0)
        ? (Compare::operator()(dim + 1, _target, dim, key) ? matching : below)
        : (Compare::operator()(dim, key, dim - 1, _target) ? matching : above);
    }
  };

  /**
   *  Overlap bounds factory that takes in a \c container, a \c key and
   *  returns an \ref overlap_bounds type.
   *
   *  This factory also checks that box \c key is valid, meaning: all its lower
   *  coordinates are indeed lower or equal to its higher coordinates.
   */
  ///@{
  template <typename Tp, typename Layout>
  overlap_bounds<typename container_traits<Tp>::key_type,
                 typename container_traits<Tp>::key_compare,
                 Layout>
  make_overlap_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target,
   Layout tag)
  {
    ::spatial::except::check_box(container, target, tag);
    return overlap_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare, Layout>
      (container.key_comp(), target);
  }

  template <typename Tp>
  overlap_bounds<typename container_traits<Tp>::key_type,
                 typename container_traits<Tp>::key_compare>
  make_overlap_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& target)
  { return make_overlap_bounds(container, target, llhh_layout_tag()); }
  ///@}

  template<typename Ct, typename Layout = llhh_layout_tag>
  struct overlap_region_iterator
    : region_iterator
      <Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                          typename container_traits<Ct>::key_compare,
                          Layout> >
  {
    /// defctor
    overlap_region_iterator() { }

    overlap_region_iterator
    (const region_iterator
    <Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                        typename container_traits<Ct>::key_compare,
                        Layout> >& other)
      : region_iterator
        <Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                            typename container_traits<Ct>::key_compare,
                            Layout> >(other) { }
  };

  template<typename Ct, typename Layout>
  struct overlap_region_iterator<const Ct, Layout>
    : region_iterator
      <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                                typename container_traits<Ct>::key_compare,
                                Layout> >
  {
    /// defctor
    overlap_region_iterator() { }

    overlap_region_iterator
    (const region_iterator
     <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                               typename container_traits<Ct>::key_compare,
                               Layout> >& other)
      : region_iterator
        <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                                  typename container_traits<Ct>::key_compare,
                                  Layout> >
        (other) { }

    overlap_region_iterator
    (const region_iterator
     <Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                         typename container_traits<Ct>::key_compare,
                         Layout> >& other)
      : region_iterator
        <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                                  typename container_traits<Ct>::key_compare,
                                  Layout> >
        (other) { }
  };

  template<typename Ct, typename Layout = llhh_layout_tag>
  struct overlap_region_iterator_pair
    : region_iterator_pair
      <Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                          typename container_traits<Ct>::key_compare,
                          Layout> >
  {
    /// defctor
    overlap_region_iterator_pair() { }

    overlap_region_iterator_pair
    (const region_iterator
     <Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                         typename container_traits<Ct>::key_compare,
                         Layout> >& a,
     const region_iterator
     <Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                         typename container_traits<Ct>::key_compare,
                         Layout> >& b)
      : region_iterator_pair
        <Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                            typename container_traits<Ct>::key_compare,
                            Layout> >
    (a, b) { }
  };

  template<typename Ct, typename Layout>
  struct overlap_region_iterator_pair<const Ct, Layout>
    : region_iterator_pair
      <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                                typename container_traits<Ct>::key_compare,
                                Layout> >
  {
    /// defctor
    overlap_region_iterator_pair() { }

    overlap_region_iterator_pair
    (const region_iterator
     <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                               typename container_traits<Ct>::key_compare,
                               Layout> >& a,
     const region_iterator
     <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                               typename container_traits<Ct>::key_compare,
                               Layout> >& b)
      : region_iterator_pair
        <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                                  typename container_traits<Ct>::key_compare,
                                  Layout> >
    (a, b) { }

    overlap_region_iterator_pair
    (const overlap_region_iterator_pair<Ct>& other)
      : region_iterator_pair
        <const Ct, overlap_bounds<typename container_traits<Ct>::key_type,
                                  typename container_traits<Ct>::key_compare,
                                  Layout> >
        (other) { }
  };

  template <typename Ct>
  inline overlap_region_iterator<Ct>
  overlap_region_end(Ct& container,
                      const typename container_traits<Ct>::key_type& target)
  {
    return region_end
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator<Ct, Layout>
  overlap_region_end(Ct& container,
                      const typename container_traits<Ct>::key_type& target,
                      const Layout& layout)
  {
    return region_end
      (container, make_overlap_bounds(container, target, layout));
  }

  template <typename Ct>
  inline overlap_region_iterator<const Ct>
  overlap_region_end(const Ct& container,
                      const typename container_traits<Ct>::key_type& target)
  {
    return _regionend
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator<const Ct, Layout>
  overlap_region_end(const Ct& container,
                      const typename container_traits<Ct>::key_type& target,
                      const Layout& layout)
  {
    return region_end
      (container, make_overlap_bounds(container, target, layout));
  }

  template <typename Ct>
  inline overlap_region_iterator<const Ct>
  overlap_region_cend(const Ct& container,
                       const typename container_traits<Ct>::key_type& target)
  {
    return region_cend
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator<const Ct, Layout>
  overlap_region_cend(const Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       const Layout& layout)
  {
    return region_cend
      (container, make_overlap_bounds(container, target, layout));
  }

  template <typename Ct>
  inline overlap_region_iterator<Ct>
  overlap_region_begin(Ct& container,
                  const typename container_traits<Ct>::key_type& target)
  {
    return region_begin
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator<Ct, Layout>
  overlap_region_begin(Ct& container,
                        const typename container_traits<Ct>::key_type& target,
                        const Layout& layout)
  {
    return region_begin
      (container, make_overlap_bounds(container, target, layout));
  }

  template <typename Ct>
  inline overlap_region_iterator<const Ct>
  overlap_region_begin(const Ct& container,
                        const typename container_traits<Ct>::key_type& target)
  {
    return _regionbegin
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator<const Ct, Layout>
  overlap_region_begin(const Ct& container,
                        const typename container_traits<Ct>::key_type& target,
                        const Layout& layout)
  {
    return region_begin
      (container, make_overlap_bounds(container, target, layout));
  }

  template <typename Ct>
  inline overlap_region_iterator<const Ct>
  overlap_region_cbegin(const Ct& container,
                   const typename container_traits<Ct>::key_type& target)
  {
    return region_cbegin
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator<const Ct, Layout>
  overlap_region_cbegin(const Ct& container,
                   const typename container_traits<Ct>::key_type& target,
                   const Layout& layout)
  {
    return region_cbegin
      (container, make_overlap_bounds(container, target, layout));
  }

  template <typename Ct>
  inline overlap_region_iterator_pair<Ct>
  overlap_region_range(Ct& container,
                        const typename container_traits<Ct>::key_type& target)
  {
    return region_range
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator_pair<Ct, Layout>
  overlap_region_range(Ct& container,
                        const typename container_traits<Ct>::key_type& target,
                        const Layout& layout)
  {
    return region_range
      (container, make_overlap_bounds(container, target, layout));
  }

  template <typename Ct>
  inline overlap_region_iterator_pair<const Ct>
  overlap_region_range(const Ct& container,
                        const typename container_traits<Ct>::key_type& target)
  {
    return region_range
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator_pair<const Ct, Layout>
  overlap_region_range(const Ct& container,
                        const typename container_traits<Ct>::key_type& target,
                        const Layout& layout)
  {
    return region_range
      (container, make_overlap_bounds(container, target, layout));
  }

  template <typename Ct>
  inline overlap_region_iterator_pair<const Ct>
  overlap_region_crange(const Ct& container,
                         const typename container_traits<Ct>::key_type& target)
  {
    return region_crange
      (container, make_overlap_bounds(container, target, llhh_layout_tag()));
  }

  template <typename Ct, typename Layout>
  inline overlap_region_iterator_pair<const Ct, Layout>
  overlap_region_crange(const Ct& container,
                         const typename container_traits<Ct>::key_type& target,
                         const Layout& layout)
  {
    return region_crange
      (container, make_overlap_bounds(container, target, layout));
  }

} // namespace spatial

#endif // SPATIAL_OVERLAP_REGION_HPP
