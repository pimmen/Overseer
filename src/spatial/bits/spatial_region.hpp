// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_region.hpp
 *  Contains the definition of \region_iterator. These iterators walk through
 *  all items in the container that are contained within an orthogonal region
 *  defined by a predicate.
 */

#ifndef SPATIAL_REGION_HPP
#define SPATIAL_REGION_HPP

#include <utility> // std::pair<> and std::make_pair()
#include "../traits.hpp"
#include "spatial_bidirectional.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"

namespace spatial
{
  /**
   *  A model of \region_predicate that defines an orthogonal region and checks
   *  if a value of type \c Key is contained within the boundaries marked by \c
   *  lower and \c upper.
   *
   *  To be very specific, given a dimension \f$d\f$ we define that \f$x\f$ is
   *  contained in the boundaries \f$(lower, upper)\f$ if:
   *
   *  \f[lower_d <= x_d < upper_d\f]
   *
   *  Simply stated, \ref bounds used in a \region_iterator will match all keys
   *  that are within the region defined by \c lower and \c upper, but not
   *  "touching" the \c upper edge.
   *
   *  \tparam Key The type used during the comparison.
   *  \tparam Compare The comparison functor using to compare 2 objects of type
   *  \c Key along the same dimension.
   *  \concept_region_predicate
   */
  template <typename Key, typename Compare>
  class bounds
    : private Compare // empty member optimization
  {
  public:
    /**
     *  The default constructor leaves everything un-initialized
     */
    bounds()
      : Compare(), _lower(), _upper() { }

    /**
     *  Set the \c lower and \c upper boundary for the orthogonal region
     *  search.
     */
    bounds(const Compare& compare, const Key& lower, const Key& upper)
      : Compare(compare), _lower(lower), _upper(upper)
    { }

    /**
     *  The operator that returns \c true if the \c key is within the
     *  boundaries, \c false if it isn't.
     */
    relative_order
    operator()(dimension_type dim, dimension_type, const Key& key) const
    {
      return (Compare::operator()(dim, key, _lower)
              ? below
              : (Compare::operator()(dim, key, _upper)
                 ? matching
                 : above));
    }

  private:
    /**
     *  The lower bound for the orthogonal region.
     */
    Key _lower;

    /**
     *  The upper bound for the orthogonal region.
     */
    Key _upper;
  };

  /**
   *  A \ref bounds factory that takes in a \c container, 2 arguments
   *  \c lower and \c upper, and returns a fully constructed \ref bounds
   *  object.
   *
   *  This factory also checks that \c lower is always less or equal to \c upper
   *  for every dimension. If it is not, it raises \ref invalid_bounds.
   *
   *  Because of this extra check, it is safer to invoke the factory rather than
   *  the constructor to build this object, especially if you are expecting user
   *  inputs.
   *
   *  \param container A container to iterate.
   *  \param lower A key defining the lower bound of \ref bounds.
   *  \param upper A key defining the upper bound of \ref bounds.
   *  \return a constructed \ref bounds object.
   *  \throws invalid_bounds
   */
  template <typename Tp>
  bounds<typename container_traits<Tp>::key_type,
         typename container_traits<Tp>::key_compare>
  make_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    except::check_bounds(container, lower, upper);
    return bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::key_compare>
      (container.key_comp(), lower, upper);
  }

  /**
   *  This type provides both an iterator and a constant iterator to iterate
   *  through all elements of a tree that match an orthogonal region defined by
   *  a predicate. If no predicate is provided, the orthogonal region search
   *  default to a \ref bounds predicate, which matches all points
   *  contained within an orthogonal region of space formed by 2 points,
   *  inclusive of lower values, but exclusive of upper values.
   *
   *  \tparam Ct The container upon which these iterator relate to.
   *  \tparam Predicate A model of \region_predicate, defaults to \ref
   *  bounds
   *  \see region_query<>::iterator
   *  \see region_query<>::const_iterator
   */
  template <typename Ct, typename Predicate
            = bounds<typename container_traits<Ct>::key_type,
                     typename container_traits<Ct>::key_compare> >
  class region_iterator
    : public details::Bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type>
  {
  private:
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Uninitialized iterator.
    region_iterator() { }

    /**
     *  Build a region iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the
     *  dimension for the node pointed to by \c iter is not known. The
     *  dimension of the node will be recomputed from the given iterator by
     *  iterating through all parents until the header node has been
     *  reached. This iteration is bounded by \Olog in case the tree is
     *  perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param pred A model of the \region_predicate concept.
     *  \param iter An iterator on the type Ct.
     */
    region_iterator(Ct& container, const Predicate& pred,
                    typename container_traits<Ct>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _pred(pred) { }

    /**
     *  Build a region iterator from the node and current dimension of a
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
     *  \param pred A model of the \region_predicate concept.
     *  \param ptr An iterator on the type Ct.
     *  \param dim The node's dimension for the node pointed to by node.
     *  \param container The container being iterated.
     */
    region_iterator
    (Ct& container, const Predicate& pred, dimension_type dim,
     typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _pred(pred) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    region_iterator<Ct, Predicate>& operator++()
    { return increment_region(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    region_iterator<Ct, Predicate> operator++(int)
    {
      region_iterator<Ct, Predicate> x(*this);
      increment_region(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    region_iterator<Ct, Predicate>& operator--()
    { return decrement_region(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    region_iterator<Ct, Predicate> operator--(int)
    {
      region_iterator<Ct, Predicate> x(*this);
      decrement_region(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    Predicate predicate() const { return _pred; }

  private:
    //! The related data for the iterator.
    Predicate _pred;
  };

  /**
   *  This type provides both an iterator and a constant iterator to iterate
   *  through all elements of a tree that match an orthogonal region defined by
   *  a predicate. If no predicate is provided, the orthogonal region search
   *  default to a \ref bounds predicate, which matches all points
   *  contained within an orthogonal region of space formed by 2 points,
   *  inclusive of lower values, but exclusive of upper values.
   *
   *  \tparam Ct The container upon which these iterator relate to.
   *  \tparam Predicate A model of \region_predicate, defaults to \ref
   *  bounds
   *  \see region_query<>::iterator
   *  \see region_query<>::const_iterator
   */
  template <typename Ct, typename Predicate>
  class region_iterator<const Ct, Predicate>
    : public details::Const_bidirectional_iterator
      <typename container_traits<Ct>::mode_type,
       typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! \empty
    region_iterator() { }

    /**
     *  Build a region iterator from a container's iterator type.
     *
     *  \param container The container being iterated.
     *  \param pred A model of the \region_predicate concept.
     *  \param iter An iterator on the type Ct.
     *
     *  This constructor should be used in the general case where the
     *  dimension for the node pointed to by \c iter is not known. The
     *  dimension of the node will be recomputed from the given iterator by
     *  iterating through all parents until the header node has been
     *  reached. This iteration is bounded by \Olog in case the tree is
     *  perfectly balanced.
     */
    region_iterator(const Ct& container, const Predicate& pred,
                    typename container_traits<Ct>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _pred(pred) { }

    /**
     *  Build a region iterator from the node and current dimension of a
     *  container's element.
     *
     *  \param container The container being iterated.
     *  \param pred A model of the \region_predicate concept.
     *  \param dim The dimension associated with \c ptr when checking the
     *  invariant in \c container.
     *  \param ptr A pointer to a node belonging to \c container.
     *
     *  This constructor should be used only when the dimension of the node
     *  pointed to by iter is known. If in doubt, use the other
     *  constructor. This constructor perform slightly faster than the other,
     *  since the dimension does not have to be calculated. Note however that
     *  the calculation of the dimension in the other iterator takes slightly
     *  longer than \Olog in general, and so it is not likely to affect the
     *  performance of your application in any major way.
     */
    region_iterator
    (const Ct& container, const Predicate& pred, dimension_type dim,
     typename container_traits<Ct>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim), _pred(pred) { }

    //! Convertion of an iterator into a const_iterator is permitted.
    region_iterator(const region_iterator<Ct, Predicate>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim), _pred(iter.predicate()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    region_iterator<const Ct, Predicate>& operator++()
    { return increment_region(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    region_iterator<const Ct, Predicate> operator++(int)
    {
      region_iterator<const Ct, Predicate> x(*this);
      increment_region(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    region_iterator<const Ct, Predicate>& operator--()
    { return decrement_region(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    region_iterator<const Ct, Predicate> operator--(int)
    {
      region_iterator<const Ct, Predicate> x(*this);
      decrement_region(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    Predicate predicate() const { return _pred; }

  private:
    //! The related data for the iterator.
    Predicate _pred;
  };

  namespace details
  {
    /**
     *  From \c iter, returns the next matching iterator in the region delimited
     *  by \c Predicate, using in-order transversal.
     *
     *  \param iter A valid region iterator.
     *  \tparam Predicate  The type of predicate for the orthogonal query.
     *  \tparam Ct The type of container to iterate.
     *  \return  An iterator pointing the next matching value.
     */
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    increment_region(region_iterator<Ct, Predicate>& iter);

    /**
     *  From \c iter, returns the previous matching iterator in the region
     *  delimited by \c Predicate, using in-order transversal.
     *
     *  \param iter A valid region iterator or a past-the-end iterator.
     *  \tparam Predicate  The type of predicate for the orthogonal query.
     *  \tparam Ct The type of container to iterate.
     *  \return  An iterator pointing the previous value.
     *
     *  If \c iter is a past-the-end iterator (pointing to a header node), the
     *  function will return the maximum iterator in region.
     */
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    decrement_region(region_iterator<Ct, Predicate>& iter);

    /**
     *  In the children of the node pointed to by \c iter, find the first
     *  matching iterator in the region delimited by \c Predicate, using
     *  in-order transversal.  If no match can be found, returns past-the-end.
     *
     *  \param iter A valid region iterator.
     *  \tparam Predicate  The type of predicate for the orthogonal query.
     *  \tparam Ct The type of container to look up.
     *  \return  An iterator pointing the minimum, or past-the-end.
     */
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    minimum_region(region_iterator<Ct, Predicate>& iter);

    /**
     *  In the children of the node pointed to by \c iter, find the last
     *  matching iterator in the region delimited by \c Predicate, using
     *  in-order transversal. If no match can be found, returns past-the-end.
     *
     *  \param iter A valid region iterator.
     *  \tparam Predicate  The type of predicate for the orthogonal query.
     *  \tparam Ct The type of container to look up.
     *  \return  An iterator pointing the maximum, or past-the-end.
     */
    template <typename Ct, typename Predicate>
    region_iterator<Ct, Predicate>&
    maximum_region(region_iterator<Ct, Predicate>& iter);

  } // namespace details

  template <typename Ct, typename Predicate>
  inline region_iterator<Ct, Predicate>
  region_end(Ct& container, const Predicate& pred)
  {
    return region_iterator<Ct, Predicate>
      (container, pred, container.dimension() - 1,
       container.end().node); // At header, dim = rank - 1
  }

  template <typename Ct, typename Predicate>
  inline region_iterator<const Ct, Predicate>
  region_end(const Ct& container, const Predicate& pred)
  {
    return region_iterator<const Ct, Predicate>
      (container, pred, container.dimension() - 1,
       container.end().node); // At header, dim = rank - 1
  }

  template <typename Ct, typename Predicate>
  inline region_iterator<const Ct, Predicate>
  region_cend(const Ct& container, const Predicate& pred)
  { return region_end(container, pred); }

  template <typename Ct>
  inline region_iterator<Ct>
  region_end(Ct& container,
             const typename container_traits<Ct>::key_type& lower,
             const typename container_traits<Ct>::key_type& upper)
  { return region_end(container, make_bounds(container, lower, upper)); }

  template <typename Ct>
  inline region_iterator<const Ct>
  region_end(const Ct& container,
             const typename container_traits<Ct>::key_type& lower,
             const typename container_traits<Ct>::key_type& upper)
  { return region_end(container, make_bounds(container, lower, upper)); }

  template <typename Ct>
  inline region_iterator<const Ct>
  region_cend(const Ct& container,
              const typename container_traits<Ct>::key_type& lower,
              const typename container_traits<Ct>::key_type& upper)
  { return region_cend(container, make_bounds(container, lower, upper)); }

  template <typename Ct, typename Predicate>
  inline region_iterator<Ct, Predicate>
  region_begin(Ct& container, const Predicate& pred)
  {
    if (container.empty()) return region_end(container, pred);
    region_iterator<Ct, Predicate>
      it(container, pred, 0, container.end().node->parent); // At root, dim = 0
    return details::minimum_region(it);
  }

  template <typename Ct, typename Predicate>
  inline region_iterator<const Ct, Predicate>
  region_begin(const Ct& container, const Predicate& pred)
  {
    if (container.empty()) return region_end(container, pred);
    region_iterator<const Ct, Predicate>
      it(container, pred, 0, container.end().node->parent); // At root, dim = 0
    return details::minimum_region(it);
  }

  template <typename Ct, typename Predicate>
  inline region_iterator<const Ct, Predicate>
  region_cbegin(const Ct& container, const Predicate& pred)
  { return region_begin(container, pred); }

  template <typename Ct>
  inline region_iterator<Ct>
  region_begin(Ct& container,
               const typename container_traits<Ct>::key_type& lower,
               const typename container_traits<Ct>::key_type& upper)
  { return region_begin(container, make_bounds(container, lower, upper)); }

  template <typename Ct>
  inline region_iterator<const Ct>
  region_begin(const Ct& container,
               const typename container_traits<Ct>::key_type& lower,
               const typename container_traits<Ct>::key_type& upper)
  { return region_begin(container, make_bounds(container, lower, upper)); }

  template <typename Ct>
  inline region_iterator<const Ct>
  region_cbegin(const Ct& container,
                const typename container_traits<Ct>::key_type& lower,
                const typename container_traits<Ct>::key_type& upper)
  { return region_cbegin(container, make_bounds(container, lower, upper)); }

  /**
   *  This structure defines a pair of mutable region iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see region_iterator
   */
  template <typename Ct, typename Predicate
            = bounds<typename container_traits<Ct>::key_type,
                     typename container_traits<Ct>::key_compare> >
  struct region_iterator_pair
    : std::pair<region_iterator<Ct, Predicate>,
                region_iterator<Ct, Predicate> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<region_iterator<Ct, Predicate>,
                      region_iterator<Ct, Predicate> > Base;

    //! Empty constructor.
    region_iterator_pair() { }

    //! Regular constructor that builds a region_iterator_pair out of 2
    //! region_iterators.
    region_iterator_pair(const region_iterator<Ct, Predicate>& a,
                         const region_iterator<Ct, Predicate>& b)
          : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant region iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see region_iterator
   */
  template <typename Ct, typename Predicate>
  struct region_iterator_pair<const Ct, Predicate>
    : std::pair<region_iterator<const Ct, Predicate>,
                region_iterator<const Ct, Predicate> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<region_iterator<const Ct, Predicate>,
                      region_iterator<const Ct, Predicate> > Base;

    //! Empty constructor.
    region_iterator_pair() { }

    //! Regular constructor that builds a region_iterator_pair out of 2
    //! region_iterators.
    region_iterator_pair(const region_iterator<const Ct, Predicate>& a,
                         const region_iterator<const Ct, Predicate>& b)
      : Base(a, b) { }

    //! Convert a mutable region iterator pair into a const region iterator
    //! pair.
    region_iterator_pair(const region_iterator_pair<Ct, Predicate>& p)
      : Base(p.first, p.second) { }
  };

  /**
   *  Returns a \region_iterator_pair where \c first points to the first element
   *  matching the predicate \c pred in \c container, and \c second points after
   *  the last element matching \c pred in \c container.
   *
   *  \tparam Ct The type of \c container.
   *  \tparam Predicate The type of \c pred, which must be a model of
   *  \region_predicate.
   *  \param container The container being searched.
   *  \param pred The predicate used for the search.
   */
  ///@{
  template <typename Ct, typename Predicate>
  inline region_iterator_pair<Ct, Predicate>
  region_range(Ct& container, const Predicate& pred)
  {
    return region_iterator_pair<Ct, Predicate>
      (region_begin(container, pred), region_end(container, pred));
  }

  //! This overload works only on constant containers and will return a set of
  //! constant iterators, where the value dereferrenced by the iterator is
  //! constant.
  template <typename Ct, typename Predicate>
  inline region_iterator_pair<const Ct, Predicate>
  region_range(const Ct& container, const Predicate& pred)
  {
    return region_iterator_pair<const Ct, Predicate>
      (region_begin(container, pred), region_end(container, pred));
  }

  //! This overload works only on constant containers and will return a set of
  //! constant iterators, where the value dereferrenced by the iterator is
  //! constant.
  template <typename Ct, typename Predicate>
  inline region_iterator_pair<const Ct, Predicate>
  region_crange(const Ct& container, const Predicate& pred)
  {
    return region_iterator_pair<const Ct, Predicate>
      (region_begin(container, pred), region_end(container, pred));
  }
  ///@}

  template <typename Ct>
  inline region_iterator_pair<Ct>
  region_range(Ct& container,
                 const typename container_traits<Ct>::key_type& lower,
                 const typename container_traits<Ct>::key_type& upper)
  { return region_range(container, make_bounds(container, lower, upper)); }

  template <typename Ct>
  inline region_iterator_pair<const Ct>
  region_range(const Ct& container,
                 const typename container_traits<Ct>::key_type& lower,
                 const typename container_traits<Ct>::key_type& upper)
  { return region_range(container, make_bounds(container, lower, upper)); }

  template <typename Ct>
  inline region_iterator_pair<const Ct>
  region_crange(const Ct& container,
                  const typename container_traits<Ct>::key_type& lower,
                  const typename container_traits<Ct>::key_type& upper)
  { return region_crange(container, make_bounds(container, lower, upper)); }

  namespace details
  {
    /**
     *  Return a boolean indicating whether all of \c key's coordinates are
     *  within range or not.
     *
     *  The key is simply tested across all dimesions over the predicate.
     *  \tparam Rank Either \static_rank or \dynamic_rank.
     *  \tparam Key The key type that is used in the comparison.
     *  \tparam Predicate A type that is a model of \region_predicate.
     *  \param rank The magnitude of the rank.
     *  \param key The key whose coordinates are verified to be within the
     *  range.
     *  \param predicate The \region_predicate object used to represent the
     *  range.
     */
    template <typename Rank, typename Key, typename Predicate>
    inline bool
    match_all(const Rank& rank, const Key& key, const Predicate& predicate)
    {
      for (dimension_type i = 0; i < rank(); ++i)
        { if (predicate(i, rank(), key) != matching) { return false; } }
      return true;
    }

    template <typename Container, typename Predicate>
    inline region_iterator<Container, Predicate>&
    increment_region(region_iterator<Container, Predicate>& iter)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const Predicate pred(iter.predicate());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      do
        {
          if (iter.node->right != 0
              && pred(iter.node_dim, rank(), const_key(iter.node)) != above)
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && pred(iter.node_dim, rank(),
                             const_key(iter.node)) != below)
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename region_iterator<Container, Predicate>::node_ptr p
                = iter.node->parent;
              while (!header(p) && iter.node == p->right)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (!header(iter.node)
             && match_all(rank, const_key(iter.node), pred) == false);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container, typename Predicate>
    inline region_iterator<Container, Predicate>&
    decrement_region(region_iterator<Container, Predicate>& iter)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const Predicate pred(iter.predicate());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_region(iter);
        }
      do
        {
          if (iter.node->left != 0
              && pred(iter.node_dim, rank(), const_key(iter.node)) != below)
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0
                     && pred(iter.node_dim, rank(),
                             const_key(iter.node)) != above)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename region_iterator<Container, Predicate>::node_ptr p
                = iter.node->parent;
              while (!header(p) && iter.node == p->left)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (!header(iter.node)
             && match_all(rank, const_key(iter.node), pred) == false);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container, typename Predicate>
    inline region_iterator<Container, Predicate>&
    minimum_region(region_iterator<Container, Predicate>& iter)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const Predicate pred(iter.predicate());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      typename region_iterator<Container, Predicate>::node_ptr end
        = iter.node->parent;
      // Quick positioning according to in-order transversal.
      while(iter.node->right != 0
            && pred(iter.node_dim, rank(), const_key(iter.node)) == below)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      while(iter.node->left != 0
            && pred(iter.node_dim, rank(), const_key(iter.node)) != below)
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      // Start algorithm.
      do
        {
          if (match_all(rank, const_key(iter.node), pred) == true) { break; }
          if (iter.node->right != 0
              && pred(iter.node_dim, rank(),
                      const_key(iter.node)) != above)
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && pred(iter.node_dim, rank(),
                             const_key(iter.node)) != below)
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename region_iterator<Container, Predicate>::node_ptr p
                = iter.node->parent;
              while (p != end && iter.node == p->right)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container, typename Predicate>
    inline region_iterator<Container, Predicate>&
    maximum_region(region_iterator<Container, Predicate>& iter)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const Predicate pred(iter.predicate());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      typename region_iterator<Container, Predicate>::node_ptr end
        = iter.node->parent;
      // Quick positioning according to in-order transversal.
      while (iter.node->left != 0
             && pred(iter.node_dim, rank(), const_key(iter.node)) == above)
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      while (iter.node->right != 0
             && pred(iter.node_dim, rank(), const_key(iter.node)) != above)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      // Start algorithm.
      do
        {
          if (match_all(rank, const_key(iter.node), pred) == true) { break; }
          if (iter.node->left != 0
              && pred(iter.node_dim, rank(), const_key(iter.node)) != below)
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0
                     && pred(iter.node_dim, rank(),
                             const_key(iter.node)) != above)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename region_iterator<Container, Predicate>::node_ptr p
                = iter.node->parent;
              while (p != end && iter.node == p->left)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_REGION_HPP
