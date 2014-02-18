/**
 *  \file RMF/Vector.h
 *  \brief Represent coordinates.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_VECTOR_H
#define RMF_VECTOR_H

#include "RMF/config.h"
#include "infrastructure_macros.h"
#include "exceptions.h"
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/not.hpp>
#include <boost/array.hpp>
#include <algorithm>

RMF_ENABLE_WARNINGS

namespace RMF {

/** Represent a point.
 */
template <unsigned int D>
class Vector
#ifndef SWIG
    : public boost::array<float, D>
#endif
      {
#ifndef SWIG
  typedef boost::array<float, D> P;
  // work around swig
  template <class R, class Enabled = void>
  struct Convert {};

  template <class R>
  struct Convert<R, typename boost::enable_if<boost::is_convertible<
                        R, boost::array<float, D> > >::type> {
    static void convert(const R& r, boost::array<float, D>& d) { d = r; }
  };

  template <class R>
  struct Convert<
      R, typename boost::enable_if<boost::mpl::not_<
             boost::is_convertible<R, boost::array<float, D> > > >::type> {
    static void convert(const R& r, boost::array<float, D>& d) {
      std::copy(boost::begin(r), boost::end(r), d.begin());
    }
  };
#endif
 public:
  Vector() {}
  //#ifndef RMF_SWIG_WRAPPER
#ifndef SWIG
  template <class Range>
  explicit Vector(Range r) {
    Convert<Range>::convert(r, *this);
  }
  Vector(const Vector<D>& o) : P(o) {};
#endif
  Vector(float x, float y, float z) {
    RMF_USAGE_CHECK(D == 3, "3 args to non-3D constructor");
    P::operator[](0) = x;
    P::operator[](1) = y;
    P::operator[](2) = z;
  }
  Vector(float x, float y, float z, float q) {
    RMF_USAGE_CHECK(D == 4, "4 args to non-4D constructor");
    P::operator[](0) = x;
    P::operator[](1) = y;
    P::operator[](2) = z;
    P::operator[](3) = q;
  }
  RMF_SHOWABLE(Vector, std::vector<float>(P::begin(), P::end()));
#if !defined(RMF_DOXYGEN)
  float __getitem__(unsigned int i) const {
    if (i >= D) throw IndexException();
    return P::operator[](i);
  }
  unsigned int __len__() const { return D; }
#endif
  static unsigned int get_dimension() { return D; }
};

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_VECTOR_H */
