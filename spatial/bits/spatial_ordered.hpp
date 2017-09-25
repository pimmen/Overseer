// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_ordered.hpp
 *  Contains the definition of \ordered_iterator.
 */

#ifndef SPATIAL_ORDERED_HPP
#define SPATIAL_ORDERED_HPP

#include "../traits.hpp"
#include "spatial_bidirectional.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"

namespace spatial
{
  /**
   *  All elements returned by this iterator are ordered from the smallest to
   *  the largest value of their key's coordinate along a single dimension.
   *
   *  These iterators walk through all items in the container in order from the
   *  lowest to the highest value along a particular dimension. The key
   *  comparator of the container is used for comparision.
   */
  template<typename Ct>
  class ordered_iterator
    : public details::Bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Uninitialized iterator.
    ordered_iterator() { }

    /**
     *  The standard way to build this iterator: specify a ordered dimension, an
     *  iterator on a container, and that container.
     *
     *  \param container   The container to iterate.
     *  \param iter        Use the value of \c iter as the start point for the
     *                     iteration.
     */
    ordered_iterator(Ct& container,
                     typename container_traits<Ct>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _cmp(container.key_comp())
    { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this function saves some CPU
     *  cycle, by comparison to the other.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref spatial::details::modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     *
     *  \param container The container to iterate.
     *  \param dim The dimension of the node pointed to by iterator.
     *  \param ptr Use the value of node as the start point for the
     *             iteration.
     */
    ordered_iterator(Ct& container, dimension_type dim,
                     typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _cmp(container.key_comp())
    { except::check_dimension(container.dimension(), dim); }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<Ct>& operator++()
    { return increment_ordered(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    ordered_iterator<Ct> operator++(int)
    {
      ordered_iterator<Ct> x(*this);
      increment_ordered(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<Ct>& operator--()
    { return decrement_ordered(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    ordered_iterator<Ct> operator--(int)
    {
      ordered_iterator<Ct> x(*this);
      decrement_ordered(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return _cmp; }

  private:
    //! The related data for the iterator.
    key_compare _cmp;
  };

  /**
   *  All elements returned by this iterator are ordered from the smallest to
   *  the largest value of their key's coordinate along a single dimension,
   *  called the ordered dimension.
   *
   *  Object deferenced by this iterator are always constant.
   */
  template<typename Ct>
  class ordered_iterator<const Ct>
    : public details::Const_bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Alias for the key_compare type used by the iterator.
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Build an uninitialized iterator.
    ordered_iterator() { }

    /**
     *  The standard way to build this iterator: specify a ordered dimension,
     *  an iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param iter Use the iterator \c iter as the start point for the
     *  iteration.
     */
    ordered_iterator(const Ct& container,
                     typename container_traits<Ct>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _cmp(container.key_comp())
    { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this function saves some CPU
     *  cycle, by comparison to the other.
     *
     *  \param container The container to iterate.
     *  \param dim The dimension of the node pointed to by iterator.
     *  \param ptr Use \c node as the start point for the iteration.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref spatial::details::modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     */
    ordered_iterator
    (const Ct& container, dimension_type dim,
     typename container_traits<Ct>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim), _cmp(container.key_comp())
    { except::check_dimension(container.dimension(), dim); }

    //! Convertion of mutable iterator into a constant iterator is permitted.
    ordered_iterator(const ordered_iterator<Ct>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim), _cmp(iter.key_comp())
    { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<const Ct>& operator++()
    { return increment_ordered(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    ordered_iterator<const Ct> operator++(int)
    {
      ordered_iterator<const Ct> x(*this);
      increment_ordered(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    ordered_iterator<const Ct>& operator--()
    { return decrement_ordered(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    ordered_iterator<const Ct> operator--(int)
    {
      ordered_iterator<const Ct> x(*this);
      decrement_ordered(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return _cmp; }

  private:
    //! The related data for the iterator.
    key_compare _cmp;
  };

  namespace details
  {
    /**
     *  Move the pointer given in parameter to the next element in the ordered
     *  iteration of values along the ordered dimension.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter The reference iterator that points to the current node.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  along \c iter's \c ordered_dim, and among the children of the node
     *  pointed to by \c iter.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking to increment an
     *  \ordered_iterator via the overloaded \c operator++().
     *
     *  Since Container is based on a \kdtree and \kdtrees exhibit good locality
     *  of reference (for arranging values in space, not for values location in
     *  memory), the function will run with time complexity close to \Olog in
     *  practice.
     *
     *  \fractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter);

    /**
     *  Move the pointer given in parameter to the previous element in the
     *  ordered iteration of values along the ordered dimension.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter The reference iterator that points to the current node.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  along \c iter's \c ordered_dim, and among the children of the node
     *  pointed to by \c iter.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking to decrement an
     *  \ordered_iterator via the overloaded \c operator--().
     *
     *  Since Container is based on a \kdtree and \kdtrees exhibit good locality
     *  of reference (for arranging values in space, not for values location in
     *  memory), the function will run with time complexity close to \Olog in
     *  practice.
     *
     *  \fractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the minimum value along the
     *  iterator's ordered dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  smallest coordinate along \c iter's \c ordered_dim, and among the
     *  children of the node pointed to by \c iter.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \fractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    minimum_ordered(ordered_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the maximum value along the
     *  iterator's ordered dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  largest coordinate along \c iter's \c ordered_dim, among the children of
     *  the node pointed to by \c iter.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \fractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter);
  } // namespace details

  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_end(Container& container)
  {
    return ordered_iterator<Container>
      // At header (dim = rank - 1)
      (container, container.dimension() - 1, container.end().node);
  }

  ///@{
  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   */
  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_end(const Container& container)
  {
    return ordered_iterator<const Container>
      // At header (dim = rank - 1)
      (container, container.dimension() - 1, container.end().node);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_cend(const Container& container)
  { return ordered_end(container); }
  ///@}

  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c ordered_dim.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *
   *  \fractime
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_begin(Container& container)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<Container>
      it(container, 0, container.end().node->parent);
    return details::minimum_ordered(it);
  }

  ///@{
  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c ordered_dim.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *
   *  \fractime
   */
  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_begin(const Container& container)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<const Container>
      it(container, 0, container.end().node->parent);
    return details::minimum_ordered(it);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_cbegin(const Container& container)
  { return ordered_begin(container); }
  ///@}

  namespace details
  {
    template <typename Cmp, typename Rank, typename Key>
    inline bool
    order_ref(const Cmp& cmp, const Rank& rank,
              const Key& a, const Key& b)
    {
      for (dimension_type d = 0; d < rank(); ++d)
        {
          if (cmp(d, a, b)) return true;
          if (cmp(d, b, a)) return false;
        }
      return (&a < &b);
    }

    //! Specialization for iterators pointing to node using the relaxed
    //! invariant.
    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter,
                      relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      // Walk the tree in both directions: one step left, one step right, etc..
      dimension_type set_dim = 0; // number of values correctly positionned
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left != 0
                  && (l_dim > set_dim
                      || !cmp(l_dim, const_key(l_node), const_key(iter.node))))
                {
                  l_node = l_node->left;
                  l_dim = incr_dim(rank, l_dim);
                  while (l_node->right != 0
                         && (l_dim > set_dim
                             || best == 0 || !cmp(l_dim, const_key(best),
                                                  const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr_dim(rank, l_dim); }
                  if (order_ref(cmp, rank, const_key(iter.node),
                                const_key(l_node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(l_node),
                                       const_key(best))))
                    { best = l_node; best_dim = l_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_dim = decr_dim(rank, l_dim);
                      p = l_node->parent;
                    }
                  l_node = p;
                  l_dim = decr_dim(rank, l_dim);
                  if (!header(l_node))
                    {
                      if (order_ref(cmp, rank, const_key(iter.node),
                                    const_key(l_node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(l_node),
                                           const_key(best))))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          if (!right_ended)
            {
              if (r_node->right != 0
                  && (r_dim > set_dim
                      || best == 0
                      || !cmp(r_dim, const_key(best), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr_dim(rank, r_dim);
                  while (r_node->left != 0
                         && (r_dim > set_dim
                             || !cmp(r_dim, const_key(r_node),
                                     const_key(iter.node))))
                    { r_node = r_node->left; r_dim = incr_dim(rank, r_dim); }
                  if (order_ref(cmp, rank, const_key(iter.node),
                                const_key(r_node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(r_node),
                                       const_key(best))))
                    { best = r_node; best_dim = r_dim; }
                  continue;
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_dim = decr_dim(rank, r_dim);
                      p = r_node->parent;
                    }
                  r_node = p;
                  r_dim = decr_dim(rank, r_dim);
                  if (!header(r_node))
                    {
                      if (order_ref(cmp, rank, const_key(iter.node),
                                    const_key(r_node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(r_node),
                                           const_key(best))))
                        { best = r_node; best_dim = r_dim; }
                      continue;
                    }
                  else right_ended = true;
                }
            }
          if (left_ended) // continue to jump over this!
            {
              // stepping is over in both directions...
              if (++set_dim == rank()) break;
              right_ended = false;
              left_ended = false;
              l_node = r_node = iter.node;
              l_dim = r_dim = iter.node_dim;
            }
        } while(true);
      if (best != 0)
        { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = r_node; iter.node_dim = r_dim; }
      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointing to node using the strict
    //! invariant.
    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter,
                      strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      // Walk the tree in both directions: one step left, one step right, etc..
      dimension_type set_dim = 0; // number of values correctly positionned
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left != 0
                  && (l_dim > set_dim
                      // strict invarient optimization
                      || cmp(l_dim, const_key(iter.node), const_key(l_node))))
                {
                  l_node = l_node->left;
                  l_dim = incr_dim(rank, l_dim);
                  while (l_node->right != 0
                         && (l_dim > set_dim
                             || best == 0
                             || !cmp(l_dim, const_key(best),
                                     const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr_dim(rank, l_dim); }
                  if (order_ref(cmp, rank, const_key(iter.node),
                                const_key(l_node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(l_node),
                                       const_key(best))))
                    { best = l_node; best_dim = l_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_dim = decr_dim(rank, l_dim);
                      p = l_node->parent;
                    }
                  l_node = p;
                  l_dim = decr_dim(rank, l_dim);
                  if (!header(l_node))
                    {
                      if (order_ref(cmp, rank, const_key(iter.node),
                                    const_key(l_node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(l_node),
                                            const_key(best))))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          if(!right_ended)
            {
              if (r_node->right != 0
                  && (r_dim > set_dim
                      || best == 0
                      || !cmp(r_dim, const_key(best), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr_dim(rank, r_dim);
                  while (r_node->left
                         && (r_dim > set_dim
                             // strict invarient optimization
                             || cmp(r_dim, const_key(iter.node),
                                    const_key(r_node))))
                    { r_node = r_node->left; r_dim = incr_dim(rank, r_dim); }
                  if (order_ref(cmp, rank, const_key(iter.node),
                                const_key(r_node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(r_node),
                                       const_key(best))))
                    { best = r_node; best_dim = r_dim; }
                  continue;
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_dim = decr_dim(rank, r_dim);
                      p = r_node->parent;
                    }
                  r_node = p;
                  r_dim = decr_dim(rank, r_dim);
                  if (!header(r_node))
                    {
                      if (order_ref(cmp, rank, const_key(iter.node),
                                    const_key(r_node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(r_node),
                                           const_key(best))))
                        { best = r_node; best_dim = r_dim; }
                      continue;
                    }
                  else right_ended = true;
                }
            }
          if (left_ended) // continue to jump over this!
            {
              // stepping is over in both directions...
              if (++set_dim == rank()) break;
              right_ended = false;
              left_ended = false;
              l_node = r_node = iter.node;
              l_dim = r_dim = iter.node_dim;
            }
        } while(true);
      if (best != 0)
        { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = r_node; iter.node_dim = r_dim; }
      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    increment_ordered(ordered_iterator<Container>& iter)
    {
      return increment_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // The next largest key on the ordered dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Container>
    inline ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter,
                      relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_ordered(iter);
        }
      // Walk the tree in both directions: one step left, one step right, etc..
      dimension_type set_dim = 0; // number of values correctly positionned
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left != 0
                  && (l_dim > set_dim
                      || best == 0
                      || !cmp(l_dim, const_key(l_node), const_key(best))))
                {
                  l_node = l_node->left;
                  l_dim = incr_dim(rank, l_dim);
                  while (l_node->right
                         && (l_dim > set_dim
                             || !cmp(l_dim, const_key(iter.node),
                                     const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr_dim(rank, l_dim); }
                  if (order_ref(cmp, rank, const_key(l_node),
                                const_key(iter.node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(best),
                                       const_key(l_node))))
                    { best = l_node; best_dim = l_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_dim = decr_dim(rank, l_dim);
                      p = l_node->parent;
                    }
                  l_node = p;
                  l_dim = decr_dim(rank, l_dim);
                  if (!header(l_node))
                    {
                      if (order_ref(cmp, rank, const_key(l_node),
                                    const_key(iter.node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(best),
                                           const_key(l_node))))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          if (!right_ended)
            {
              if (r_node->right != 0
                  && (r_dim > set_dim
                      || !cmp(r_dim, const_key(iter.node), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr_dim(rank, r_dim);
                  while (r_node->left
                         && (r_dim > set_dim
                             || best == 0
                             || !cmp(r_dim, const_key(r_node), const_key(best))))
                    { r_node = r_node->left; r_dim = incr_dim(rank, r_dim); }
                  if (order_ref(cmp, rank, const_key(r_node),
                                const_key(iter.node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(best),
                                       const_key(r_node))))
                    { best = r_node; best_dim = r_dim; }
                  continue;
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_dim = decr_dim(rank, r_dim);
                      p = r_node->parent;
                    }
                  r_node = p;
                  r_dim = decr_dim(rank, r_dim);
                  if (!header(r_node))
                    {
                      if (order_ref(cmp, rank, const_key(r_node),
                                    const_key(iter.node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(best),
                                           const_key(r_node))))
                        { best = r_node; best_dim = r_dim; }
                      continue;
                    }
                  else right_ended = true;
                }
            }
          if (left_ended) // continue to jump over this!
            {
              // stepping is over in both directions...
              if (++set_dim == rank()) break;
              right_ended = false;
              left_ended = false;
              l_node = r_node = iter.node;
              l_dim = r_dim = iter.node_dim;
            }
        } while(true);
      if (best != 0)
        { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = r_node; iter.node_dim = r_dim; }
      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // The next largest key on the ordered dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Container>
    inline ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter,
                      strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_ordered(iter);
        }
      // Walk the tree in both directions: one step left, one step right, etc..
      dimension_type set_dim = 0; // number of values correctly positionned
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr l_node, r_node; l_node = r_node = iter.node;
      dimension_type l_dim, r_dim; l_dim = r_dim = iter.node_dim;
      bool left_ended = false, right_ended = false;
      do
        {
          if (!left_ended)
            {
              if (l_node->left != 0
                  && (l_dim > set_dim
                      || best == 0
                      // optimization for strict invarient
                      || cmp(l_dim, const_key(best), const_key(l_node))))
                {
                  l_node = l_node->left;
                  l_dim = incr_dim(rank, l_dim);
                  while (l_node->right
                         && (l_dim > set_dim
                             || !cmp(l_dim, const_key(iter.node),
                                     const_key(l_node))))
                    { l_node = l_node->right; l_dim = incr_dim(rank, l_dim); }
                  if (order_ref(cmp, rank, const_key(l_node),
                                const_key(iter.node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(best),
                                       const_key(l_node))))
                    { best = l_node; best_dim = l_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_dim = decr_dim(rank, l_dim);
                      p = l_node->parent;
                    }
                  l_node = p;
                  l_dim = decr_dim(rank, l_dim);
                  if (!header(l_node))
                    {
                      if (order_ref(cmp, rank, const_key(l_node),
                                    const_key(iter.node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(best),
                                           const_key(l_node))))
                        { best = l_node; best_dim = l_dim; }
                    }
                  else left_ended = true;
                }
            }
          if (!right_ended)
            {
              if (r_node->right
                  && (r_dim > set_dim
                      || !cmp(r_dim, const_key(iter.node), const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_dim = incr_dim(rank, r_dim);
                  while (r_node->left
                         && (r_dim > set_dim
                             || best == 0
                             // optimization for strict invarient
                             || cmp(r_dim, const_key(best), const_key(r_node))))
                    { r_node = r_node->left; r_dim = incr_dim(rank, r_dim); }
                  if (order_ref(cmp, rank, const_key(r_node),
                                const_key(iter.node))
                      && (best == 0
                          || order_ref(cmp, rank, const_key(best),
                                       const_key(r_node))))
                    { best = r_node; best_dim = r_dim; }
                  continue;
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_dim = decr_dim(rank, r_dim);
                      p = r_node->parent;
                    }
                  r_node = p;
                  r_dim = decr_dim(rank, r_dim);
                  if (!header(r_node))
                    {
                      if (order_ref(cmp, rank, const_key(r_node),
                                    const_key(iter.node))
                          && (best == 0
                              || order_ref(cmp, rank, const_key(best),
                                           const_key(r_node))))
                        { best = r_node; best_dim = r_dim; }
                      continue;
                    }
                  else right_ended = true;
                }
            }
          if (left_ended) // continue to jump over this!
            {
              // stepping is over in both directions...
              if (++set_dim == rank()) break;
              right_ended = false;
              left_ended = false;
              l_node = r_node = iter.node;
              l_dim = r_dim = iter.node_dim;
            }
        } while(true);
      if (best != 0)
        { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = r_node; iter.node_dim = r_dim; }
      SPATIAL_ASSERT_CHECK(r_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    decrement_ordered(ordered_iterator<Container>& iter)
    {
      return decrement_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template <typename Container>
    inline ordered_iterator<Container>&
    minimum_ordered(ordered_iterator<Container>& iter)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      while (iter.node->left != 0)
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          node_ptr node = iter.node;
          dimension_type node_dim = iter.node_dim;
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0)
                    { node = node->left; node_dim = incr_dim(rank, node_dim); }
                  if (order_ref(cmp, rank,
                                const_key(node), const_key(best)))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node_ptr p = node->parent;
                  while (p != end && p->right == node)
                    {
                      node = p;
                      node_dim = decr_dim(rank, node_dim);
                      p = node->parent;
                    }
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (node != end
                      && order_ref(cmp, rank,
                                   const_key(node), const_key(best)))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(!header(best));
      return iter;
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter,
                    relaxed_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          node_ptr node = iter.node;
          dimension_type node_dim = iter.node_dim;
          do
            {
              if (node->left != 0
                  && (node_dim > set_dim
                      || !cmp(node_dim, const_key(node), const_key(best))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->right != 0)
                    { node = node->right; node_dim = incr_dim(rank, node_dim); }
                  if (order_ref(cmp, rank,
                                const_key(best), const_key(node)))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node_ptr p = node->parent;
                  while (p != end && p->left == node)
                    {
                      node = p;
                      node_dim = decr_dim(rank, node_dim);
                      p = node->parent;
                    }
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (node != end
                      && order_ref(cmp, rank, const_key(best),
                                   const_key(node)))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter,
                    strict_invariant_tag)
    {
      typedef typename ordered_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      dimension_type set_dim = 0;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          node_ptr node = iter.node;
          dimension_type node_dim = iter.node_dim;
          do
            {
              // optimization for strict invariant
              if (node->left != 0 && node_dim > set_dim)
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->right != 0)
                    { node = node->right; node_dim = incr_dim(rank, node_dim); }
                  if (order_ref(cmp, rank,
                                const_key(best), const_key(node)))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node_ptr p = node->parent;
                  while (p != end && p->left == node)
                    {
                      node = p;
                      node_dim = decr_dim(rank, node_dim);
                      p = node->parent;
                    }
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (node != end
                      && order_ref(cmp, rank,
                                   const_key(best), const_key(node)))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    maximum_ordered(ordered_iterator<Container>& iter)
    {
      return maximum_ordered
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }
  } // namespace details
} // namespace spatial

#endif // SPATIAL_ORDERED_HPP
