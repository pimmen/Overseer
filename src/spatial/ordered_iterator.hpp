// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   ordered_iterator.hpp
 *  Provides \ordered_iterator and all the functions around it.
 */

#ifndef SPATIAL_ORDERED_ITERATOR_HPP
#define SPATIAL_ORDERED_ITERATOR_HPP

#include <utility> // provides ::std::pair<>
#include "spatial.hpp"
#include "bits/spatial_ordered.hpp"

namespace spatial
{
  /**
   *  This structure defines a pair of mutable \ordered_iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see ordered_iterator
   */
  template <typename Ct>
  struct ordered_iterator_pair
        : std::pair<ordered_iterator<Ct>, ordered_iterator<Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<ordered_iterator<Ct>, ordered_iterator<Ct> > Base;

    //! \empty
    ordered_iterator_pair() { }

    //! Regular constructor that builds a ordered_iterator_pair out of 2
    //! \ordered_iterator.
    ordered_iterator_pair(const ordered_iterator<Ct>& a,
                          const ordered_iterator<Ct>& b) : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant ordered iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see ordered_iterator
   */
  template <typename Ct>
  struct ordered_iterator_pair<const Ct>
        : std::pair<ordered_iterator<const Ct>, ordered_iterator<const Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<ordered_iterator<const Ct>, ordered_iterator<const Ct> >
          Base;

    //! \empty
    ordered_iterator_pair() { }

    //! Regular constructor that builds a ordered_iterator_pair out of 2
    //! \ordered_iterator.
    ordered_iterator_pair(const ordered_iterator<const Ct>& a,
                          const ordered_iterator<const Ct>& b) : Base(a, b)
    { }

    //! Convert a mutable ordered iterator pair into a const ordered iterator
    //!pair.
    ordered_iterator_pair(const ordered_iterator_pair<Ct>& p)
      : Base(p.first, p.second) { }
  };

  /**
   *  Returns a pair of iterator on the first and the last value in the range
   *  that can be iterated. This function is convenient to use with
   *  \c std::tie, and is equivalent to calling \ref ordered_begin() and \ref
   *  ordered_end() on both iterators.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \fractime
   */
  template <typename Container>
  inline ordered_iterator_pair<Container>
  ordered_range(Container& container)
  {
    return ordered_iterator_pair<Container>(ordered_begin(container),
                                            ordered_end(container));
  }

  ///@{
  /**
   *  Returns a pair of constant iterator on the first and the last value in the
   *  range that can be iterated. This function is convenient to use with
   *  \c std::tie, and is equivalent to calling \ref ordered_begin() and \ref
   *  ordered_end() on both iterators.
   *
   *  \tparam Container The type of container to iterate.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \fractime
   */
  template <typename Container>
  inline ordered_iterator_pair<const Container>
  ordered_range(const Container& container)
  {
    return ordered_iterator_pair<const Container>(ordered_begin(container),
                                                  ordered_end(container));
  }

  template <typename Container>
  inline ordered_iterator_pair<const Container>
  ordered_crange(const Container& container)
  {
    return ordered_iterator_pair<const Container>(ordered_begin(container),
                                                  ordered_end(container));
  }
  ///@}

  namespace details
  {
    /**
     *  Move the iterator given in parameter to the value with the smallest
     *  coordinate greater or equal to \c bound along the ordered dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The lower bound to the iterator position.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  greater or equal to \c bound along \c iter's \c ordered_dim, or to the
     *  parent of the value pointed to.
     *
     *  \fractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound);

    /**
     *  Move the iterator given in parameter to the value with the largest
     *  coordinate strictly lower than \c bound along the ordered dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  ordered_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The upper bound to the iterator position.
     *  \return \c iter moved to the value with the largest coordinate strictly
     *  less than \c bound along \c iter's \c ordered_dim, or to the
     *  parent of the value pointed to.
     *
     *  \fractime
     */
    template <typename Container>
    ordered_iterator<Container>&
    upper_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound);
  } // namespace details

  /**
   *  Finds the value with the smallest coordinate along the ordered dimension
   *  that is greater or equal to \c bound, and return an iterator pointing to
   *  this value.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c ordered_dim.
   *
   *  \fractime
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_lower_bound(Container& container,
                      const typename container_traits<Container>::key_type&
                      bound)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<Container> it
      (container, 0, container.end().node->parent);
    return details::lower_bound_ordered(it, bound);
  }

  ///@{
  /**
   *  Finds the value with the smallest coordinate along the ordered dimension
   *  that is greater or equal to \c bound, and return a constant iterator to
   *  this value.
   *
   *  \tparam Container The type of container to iterate.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c ordered_dim.
   *
   *  \fractime
   */
  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_lower_bound
  (const Container& container,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<const Container> it
      (container, 0, container.end().node->parent);
    return details::lower_bound_ordered(it, bound);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_clower_bound
  (const Container& container,
   const typename container_traits<Container>::key_type& bound)
  { return ordered_lower_bound(container, bound); }
  ///@}

  /**
   *  Finds the value with the largest coordinate along the ordered dimension
   *  that is stricly less than \c bound, and return an iterator pointing to
   *  this value.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c ordered_dim.
   *
   *  \fractime
   *  \see ordered_iterator
   */
  template <typename Container>
  inline ordered_iterator<Container>
  ordered_upper_bound
  (Container& container,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<Container> it // At root (dim = 0)
      (container, 0, container.end().node->parent);
    return details::upper_bound_ordered(it, bound);
  }

  ///@{
  /**
   *  Finds the value with the largest coordinate along the ordered dimension
   *  that is stricly less than \c bound, and return an iterator pointing to
   *  this value.
   *
   *  \tparam Container The type of container to iterate.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c ordered_dim.
   *
   *  \fractime
   *  \see ordered_iterator
   */
  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_upper_bound
  (const Container& container,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return ordered_end(container);
    ordered_iterator<const Container> it  // At root (dim = 0)
      (container, 0, container.end().node->parent);
    return details::upper_bound_ordered(it, bound);
  }

  template <typename Container>
  inline ordered_iterator<const Container>
  ordered_cupper_bound
  (const Container& container,
   const typename container_traits<Container>::key_type& bound)
  { return ordered_upper_bound(container, bound); }
  ///@}

  namespace details
  {
    // Used to compare the bound against the node's key
    template <typename Cmp, typename Key>
    inline bool
    order_less(const Cmp& cmp, dimension_type set_dim,
               const Key& a, const Key& b)
    {
      for (dimension_type d = 0; d <= set_dim; ++d)
        {
          if (cmp(d, a, b)) return true;
          if (cmp(d, b, a)) return false;
        }
      return false;
    }

    // Specialization for iterators pointed to node using the relaxed
    // invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
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
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr node;
      dimension_type node_dim;
      do
        {
          node = iter.node;
          node_dim = iter.node_dim;
          while (node->left != 0
                 && (node_dim > set_dim
                     || !cmp(node_dim, const_key(node), bound)))
            { node = node->left; node_dim = incr_dim(rank, node_dim); }
          if (!order_less(cmp, set_dim, const_key(node), bound))
            { best = node; best_dim = node_dim; }
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim || best == 0
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0
                         && (node_dim > set_dim
                             || !cmp(node_dim, const_key(node), bound)))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (!order_less(cmp, set_dim, const_key(node), bound)
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
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
                      && !order_less(cmp, set_dim, const_key(node), bound)
                      && (best == 0 || order_ref(cmp, rank, const_key(node),
                                                 const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // Specialization for iterators pointed to node using the strict
    // invariant.
    template<typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
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
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr node;
      dimension_type node_dim;
      do
        {
          node = iter.node;
          node_dim = iter.node_dim;
          while (node->left != 0
                 && (node_dim > set_dim
                     // optimization for strict invarient
                     || cmp(node_dim, bound, const_key(node))))
            { node = node->left; node_dim = incr_dim(rank, node_dim); }
          if (!order_less(cmp, set_dim, const_key(node), bound))
            { best = node; best_dim = node_dim; }
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim || best == 0
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0
                         && (node_dim > set_dim
                             // optimization for strict invarient
                             || cmp(node_dim, bound, const_key(node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (!order_less(cmp, set_dim, const_key(node), bound)
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
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
                      && !order_less(cmp, set_dim, const_key(node), bound)
                      && (best == 0 || order_ref(cmp, rank, const_key(node),
                                                 const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    lower_bound_ordered(ordered_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return lower_bound_ordered
        (iter, bound, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline ordered_iterator<Container>&
    upper_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
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
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr node;
      dimension_type node_dim;
      do
        {
          node = iter.node;
          node_dim = iter.node_dim;
          while (node->left != 0
                 && (node_dim > set_dim
                     || !cmp(node_dim, const_key(node), bound)))
            { node = node->left; node_dim = incr_dim(rank, node_dim); }
          if (order_less(cmp, set_dim, bound, const_key(node)))
            { best = node; best_dim = node_dim; }
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim || best == 0
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0
                         && (node_dim > set_dim
                             || !cmp(node_dim, const_key(node), bound)))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (order_less(cmp, set_dim, bound, const_key(node))
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
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
                      && order_less(cmp, set_dim, bound, const_key(node))
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline ordered_iterator<Container>&
    upper_bound_ordered
    (ordered_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
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
      node_ptr best = 0;
      dimension_type best_dim = 0;
      node_ptr node;
      dimension_type node_dim;
      do
        {
          node = iter.node;
          node_dim = iter.node_dim;
          while (node->left != 0
                 && (node_dim > set_dim
                     // Optimization for strict invarient
                     || cmp(node_dim, bound, const_key(node))))
            { node = node->left; node_dim = incr_dim(rank, node_dim); }
          if (order_less(cmp, set_dim, bound, const_key(node)))
            { best = node; best_dim = node_dim; }
          do
            {
              if (node->right != 0
                  && (node_dim > set_dim || best == 0
                      || !cmp(node_dim, const_key(best), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                  while (node->left != 0
                         && (node_dim > set_dim
                             // Optimization for strict invarient
                             || cmp(node_dim, bound, const_key(node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (order_less(cmp, set_dim, bound, const_key(node))
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
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
                      && order_less(cmp, set_dim, bound, const_key(node))
                      && (best == 0
                          || order_ref(cmp, rank,
                                       const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
            } while (node != end);
        } while (++set_dim < rank());
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline ordered_iterator<Container>&
    upper_bound_ordered(ordered_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return upper_bound_ordered
        (iter, bound, typename container_traits<Container>::mode_type
         ::invariant_category());
    }
  } // namespace details
} // namespace spatial

#endif // SPATIAL_ORDERED_ITERATOR_HPP
