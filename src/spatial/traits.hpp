// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   traits.hpp
 *  Define several trait classes for types in the library, including
 *  containers and metric types.
 */

#ifndef SPATIAL_TRAITS_HPP
#define SPATIAL_TRAITS_HPP

namespace spatial
{
  /*
   *  The linking mode traits of the trees in spatial.
   *
   *  The linking mode represents how nodes are related to the link. The link
   *  binds together in the same object the node information, the key and the
   *  value type. The linking mode allows to retrieve each of these objects from
   *  the node, and allows to retrieve the node from the link itself.
   *
   *  \tparam Mode A model of \linkmode.
   */
  template<typename Mode>
  struct mode_traits
  {
    //! The invarient category for the linking mode.
    typedef typename Mode::invariant_category invariant_category;
  };

  /**
   *  The traits type for all containers in the spatial
   *  namespace. Defines all the types that must be published or resolved from
   *  the spatial container types.
   */
  template<typename Tp>
  struct container_traits
  {
    /**
     *  The type representing the key managed by the container. \c key_type
     *  holds the coordinates that keeps its instances ordered in the container.
     */
    typedef typename Tp::key_type               key_type;

    /**
     *  When mapped_type and key_type are different, value_type holds a pair
     *  formed by key_type (first) and mapped_type (second); this is the case
     *  for \point_multimap or \box_multimap containers. In \point_multiset and
     *  \box_multiset containers, \c key_type, \c mapped_type and \c value_type
     *  are all similar types.
     */
    typedef typename Tp::value_type             value_type;

    /**
     *  Represent a pointer to a \c value_type. This type is used by the
     *  iterators of the container.
     */
    typedef typename Tp::pointer                pointer;

    //! Represent a pointer to a key_type. key_type is always constant.
    typedef typename Tp::const_pointer          const_pointer;

    /**
     *  Represent a reference to a key_type. May or may not be mutable,
     *  depending on the type of \c key_type.
     */
    typedef typename Tp::reference              reference;

    //! Represent a reference to a key_type. key_type is always constant.
    typedef typename Tp::const_reference        const_reference;

    /**
     *  The \linkmode type that is associated with the container. Accessing
     *  this type allows external iterators to tie the nodes in the container
     *  with the proper linking type.
     */
    typedef typename Tp::mode_type              mode_type;

    //! The size type is used to list the number of element in the tree.
    typedef typename Tp::size_type              size_type;

    //! The difference type is used when 2 iterators from the tree are substracted.
    typedef typename Tp::difference_type        difference_type;

    //! The allocator used in the container.
    typedef typename Tp::allocator_type         allocator_type;

    /**
     *  Comparison functor used to compare two instances of \c key_type. It is
     *  also the type provided by the user in the various containers,
     *  \point_multiset, \box_multiset, \point_multimap or \box_multimap.
     */
    typedef typename Tp::key_compare            key_compare;

    /**
     *  Comparison functor used to compare two instances of \c value_type. This
     *  type is deduced from the key_compare in all containers.
     */
    typedef typename Tp::value_compare          value_compare;

    /**
     *  The type used to represent the rank of the container. Either
     *  \static_rank or \dynamic_rank. It is not a number but a wrapper around a
     *  dimension_type value.
     */
    typedef typename Tp::rank_type              rank_type;

    /**
     *  The type used to iterate a container. Every container must have an
     *  iterator.
     */
    typedef typename Tp::iterator               iterator;

    /**
     *  The type used to iterate a constant container. Every container must have
     *  a constant iterator.
     */
    typedef typename Tp::const_iterator         const_iterator;
  };

  /**
   *  The traits type for all metrics in the spatial namespace. Defines all the
   *  types that must be published or resolved from objects that are a model of
   *  \metric.
   *
   *  \tparam Tp A model of \metric.
   */
  template <typename Tp>
  struct metric_traits
  {
    /**
     *  The type used by the metric to represent distances.
     *
     *  Distance may be user defined in some metric such as \euclidian,
     *  \quadrance or \manhattan, but in this case, the type must follow
     *  specific rules to the metric, such as overload of specific operators.
     */
    typedef typename Tp::distance_type         distance_type;
  };

} //namespace spatial

#endif // SPATIAL_TRAITS_HPP
