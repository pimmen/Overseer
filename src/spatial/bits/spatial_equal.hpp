// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_equal.hpp
 *  Contains the definition of the equal iterators. These iterators
 *  walk through all items in the container that are equal to a key given in
 *  parameter of the iterator.
 */

#ifndef SPATIAL_EQUAL_HPP
#define SPATIAL_EQUAL_HPP

#include "spatial_bidirectional.hpp"
#include "../traits.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"
#include "spatial_compress.hpp"
#include "spatial_preorder.hpp"

namespace spatial
{
  namespace details
  {
    template <typename Container>
    struct Equal : private Container::key_compare // empty member optimization
    {
      Equal() { }

      Equal(const typename Container::key_compare& cmp,
            const typename Container::key_type& value_)
        : Container::key_compare(cmp), value(value_) { }

      typename Container::key_compare key_comp() const
      { return *static_cast<const typename Container::key_compare*>(this); }

      typename Container::key_type value;
    };

    template <typename Container>
    inline bool
    right_traversal(typename Container::mode_type::const_node_ptr node,
                    dimension_type dim,
                    const Equal<Container>& equal)
    {
      return !equal.key_comp()(dim, equal.value, const_key(node));
    }

    /**
     *  Return a boolean indicating whether all of \c x's coordinates are
     *  equal to \c y's coordinates.
     *
     *  The key at \c node y are tested across all dimensions using the
     *  comparator \c equal provided by a container.
     *  \tparam Rank Either \static_rank or \dynamic_rank.
     *  \tparam Key A key type defined in the container as the \c Compare.
     *  \tparam Compare A \trivial_compare type defined in the same
     *  container as \c Key.
     *  \param rank The magnitude of the rank.
     *  \param node A pointer to the node being inspected.
     *  \param equal A functor with all parameters for the query
     */
    template <typename Container>
    inline bool
    stop_traversal(typename Container::mode_type::const_node_ptr node,
                    typename Container::rank_type rank,
                    const Equal<Container>& equal)
    {
      dimension_type i = 0;
      for (; i < rank()
             && !equal.key_comp()(i, const_key(node), equal.value)
             && !equal.key_comp()(i, equal.value, const_key(node));
           ++i) { }
      return (i == rank());
    }

    template <typename Container>
    inline bool
    left_traversal(typename Container::mode_type::const_node_ptr node,
                   dimension_type dim,
                   const Equal<Container>& equal,
                   relaxed_invariant_tag)
    {
      return !equal.key_comp()(dim, const_key(node), equal.value);
    }

    template <typename Container>
    inline bool
    left_traversal(typename Container::mode_type::const_node_ptr node,
                   dimension_type dim,
                   const Equal<Container>& equal,
                   strict_invariant_tag)
    {
      return equal.key_comp()(dim, equal.value, const_key(node));
    }

    template <typename Container>
    inline bool
    left_traversal(typename Container::mode_type::const_node_ptr node,
                   dimension_type dim,
                   const Equal<Container>& equal)
    {
      return left_traversal
        (node, dim, equal,
         typename Container::mode_type::invariant_category());
    }

  } // namespace details

  /**
   *  This type provides an iterator to iterate through all elements of a
   *  container that match a given key, passed as a parameter to the
   *  constructor. The given key is called the model.
   *
   *  \tparam Container The container upon which these iterator relate to.
   *  \headerfile equal_iterator.hpp
   */
  template <typename Container>
  class equal_iterator
    : public details::Bidirectional_iterator
      <typename container_traits<Container>::mode_type,
       typename container_traits<Container>::rank_type>
  {
  private:
    //! The preorder iterator without its criterion
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Container>::mode_type,
     typename container_traits<Container>::rank_type> Base;

  public:
    using Base::node;
    using Base::node_dim;
    using Base::rank;

    //! The type used to store the model key to be looked up in the container.
    typedef typename container_traits<Container>::key_type key_type;

    //! The comparison functor used to compare keys.
    typedef typename container_traits<Container>::key_compare key_compare;

    //! \empty
    equal_iterator() { }

    /**
     *  Build an equal iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the dimension
     *  for the node pointed to by \c iter is not known. The dimension of the
     *  node will be recomputed from the given iterator by iterating through all
     *  parents until the header node has been reached. This iteration is
     *  bounded by \Olog when the container is perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param value_ The key to look for.
     *  \param iter An iterator on the type Ct.
     */
    equal_iterator(Container& container, const key_type& value_,
                   typename container_traits<Container>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _query(container.key_comp(), value_) { }

    /**
     *  Build an equal iterator from the node and current dimension of a
     *  container's element.
     *
     *  This constructor should be used only when the dimension of the node
     *  pointed to by iter is known. If in doubt, use the other
     *  constructor. This constructor perform slightly faster than the other,
     *  since the dimension does not have to be calculated. Note however that
     *  the calculation of the dimension in the other iterator takes slightly
     *  longer than \Olog in general, and so it is not likely to affect the
     *  performance of your application in any major way.
     *
     *  \param container The container being iterated.
     *  \param value_ The key to look for.
     *  \param ptr An iterator on the type Ct.
     *  \param dim The node's dimension for the node pointed to by node.
     *  \param container The container being iterated.
     */
    equal_iterator
    (Container& container, const key_type& value_, dimension_type dim,
     typename container_traits<Container>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _query(container.key_comp(), value_)
    { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<Container>& operator++()
    {
      details::assign(node, node_dim,
                      preorder_increment(node, node_dim, rank(), _query));
      return *this;
    }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    equal_iterator<Container> operator++(int)
    {
      equal_iterator<Container> x(*this);
      details::assign(node, node_dim,
                      preorder_increment(node, node_dim, rank(), _query));
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<Container>& operator--()
    {
      details::assign(node, node_dim,
                      preorder_decrement(node, node_dim, rank(), _query));
      return *this;
    }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    equal_iterator<Container> operator--(int)
    {
      equal_iterator<Container> x(*this);
      details::assign(node, node_dim,
                      preorder_decrement(node, node_dim, rank(), _query));
      return x;
    }

    //! Return the value of key used to find equal keys in the container.
    key_type value() const { return _query.value; }

    //! Return the functor used to compare keys in this iterator.
    key_compare key_comp() const { return _query.key_comp(); }

  private:
    //! The model key used to find equal keys in the container.
    details::Equal<Container> _query;
  };

  /**
   *  This type provides an iterator to iterate through all elements of a
   *  container that match a given key, passed as a parameter to the
   *  constructor. The given key is called the model.
   *
   *  The values returned by this iterator will not be mutable.
   *
   *  \tparam Ct The container upon which these iterator relate to.
   */
  template <typename Container>
  class equal_iterator<const Container>
    : public details::Const_bidirectional_iterator
      <typename container_traits<Container>::mode_type,
       typename container_traits<Container>::rank_type>
  {
  private:
    //! The preorder iterator without its criterion.
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Container>::mode_type,
     typename container_traits<Container>::rank_type> Base;

  public:
    using Base::node;
    using Base::node_dim;
    using Base::rank;

    //! The type used to store the model key to be looked up in the container.
    typedef typename container_traits<Container>::key_type key_type;

    //! The comparison functor used to compare keys.
    typedef typename container_traits<Container>::key_compare key_compare;

    //! \empty
    equal_iterator() { }

    /**
     *  Build an equal iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the dimension
     *  for the node pointed to by \c iter is not known. The dimension of the
     *  node will be recomputed from the given iterator by iterating through all
     *  parents until the header node has been reached. This iteration is
     *  bounded by \Olog when the container is perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param value_ The key to look for.
     *  \param iter An iterator from the container.
     */
    equal_iterator(const Container& container, const key_type& value_,
                   typename container_traits<Container>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _query(container.key_comp(), value_) { }

    /**
     *  Build an equal iterator from the node and current dimension of a
     *  container's element.
     *
     *  This constructor should be used only when the dimension of the node
     *  pointed to by iter is known. If in doubt, use the other
     *  constructor. This constructor perform slightly faster since the
     *  dimension does not have to be calculated.
     *
     *  \param container The container being iterated.
     *  \param value_ The key to look for.
     *  \param dim The dimension associated with \c ptr when checking the
     *  invariant in \c container.
     *  \param ptr A pointer to a node belonging to \c container.
     */
    equal_iterator
    (const Container& container, const key_type& value_, dimension_type dim,
     typename container_traits<Container>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim), _query(container.key_comp(), value_)
    { }

    //! Convertion of an iterator into a const_iterator is permitted.
    equal_iterator(const equal_iterator<Container>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim),
        _query(iter.key_comp(), iter.value()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<const Container>& operator++()
    {
      details::assign(node, node_dim,
                      preorder_increment(node, node_dim, rank(), _query));
      return *this;
    }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    equal_iterator<const Container> operator++(int)
    {
      equal_iterator<const Container> x(*this);
      details::assign(node, node_dim,
                      preorder_increment(node, node_dim, rank(), _query));
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<const Container>& operator--()
    {
      details::assign(node, node_dim,
                      preorder_decrement(node, node_dim, rank(), _query));
      return *this;
    }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    equal_iterator<const Container> operator--(int)
    {
      equal_iterator<const Container> x(*this);
      details::assign(node, node_dim,
                      preorder_decrement(node, node_dim, rank(), _query));
      return x;
    }

    //! Returns the value used to find equivalent keys in the container.
    key_type value() const { return _query.value; }

    //! Returns the functor used to compare keys in this iterator.
    key_compare key_comp() const { return _query.key_comp(); }

  private:
    //! The model key used to find equal keys in the container.
    details::Equal<Container> _query;
  };

  template <typename Container>
  inline equal_iterator<Container>
  equal_end(Container& container,
            const typename equal_iterator<Container>::key_type& value)
  {
    return equal_iterator<Container>
      (container, value, container.dimension() - 1,
       container.end().node); // At header, dim = rank - 1
  }

  template <typename Container>
  inline equal_iterator<const Container>
  equal_cend(const Container& container,
             const typename equal_iterator<Container>::key_type& value)
  { return equal_end(container, value); }

  /**
   *  Find the first element in \c container that compares equally to \c value,
   *  using \c container's \c key_compare comparator.
   *
   *  \tparam Container The container type being iterated.
   *  \param container The container being iterated.
   *  \param value A value to find matches among other keys stored in the
   *  container.
   */
  ///@{
  template <typename Container>
  inline equal_iterator<Container>
  equal_begin(Container& container,
              const typename equal_iterator<Container>::key_type& value)
  {
    if (container.empty()) return equal_end(container, value);
    typename equal_iterator<Container>::node_ptr node
      = container.end().node->parent;
    dimension_type dim;
    details::assign(node, dim,
                    first_equal(node, 0, container.rank(),
                                container.key_comp(), value));
    return equal_iterator<Container>(container, value, dim, node);
  }

  template <typename Container>
  inline equal_iterator<const Container>
  equal_cbegin(const Container& container,
               const typename equal_iterator<Container>::key_type& value)
  { return equal_begin(container, value); }
  ///@}

  namespace details
  {
    template <typename NodePtr, typename Rank, typename KeyCompare, typename Key>
    inline std::pair<NodePtr, dimension_type>
    first_equal(NodePtr node, dimension_type dim, Rank rank,
                KeyCompare key_comp, const Key& key)
    {
      // Write in pre-order fashion
      NodePtr end = node->parent;
      dimension_type end_dim = decr_dim(rank, dim);
      for (;;)
        {
          // Test coordinates of node's key, retain results for dim
          bool walk_left = !key_comp(dim, const_key(node), key);
          bool walk_right = !key_comp(dim, key, const_key(node));
          if (walk_left && walk_right)
            {
              dimension_type test = 0;
              for (; test < dim && !(key_comp(test, key, const_key(node))
                                     || key_comp(test, const_key(node), key)); ++test);
              if (test == dim)
                {
                  test = dim + 1;
                  for (; test < rank() && !(key_comp(test, key, const_key(node))
                                            || key_comp(test, const_key(node), key)); ++test);
                  if (test == rank())
                    { return std::make_pair(node, dim); }
                }
            }
          // Walk the tree to find an equal target
          if (walk_left && node->left != 0)
            {
              if (walk_right && node->right != 0)
                {
                  // Go recursively in this case only, left first
                  NodePtr other;
                  dimension_type other_dim;
                  assign(other, other_dim,
                         first_equal(node->left, incr_dim(rank, dim),
                                     rank, key_comp, key));
                  if (other != node)
                    { return std::make_pair(other, other_dim); }
                  else
                    { node = node->right; dim = incr_dim(rank, dim); }
                }
              else
                { node = node->left; dim = incr_dim(rank, dim); }
            }
          else if (walk_right && node->right != 0)
            { node = node->right; dim = incr_dim(rank, dim); }
          else { return std::make_pair(end, end_dim); }
        }
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_EQUAL_HPP
