/**
 *  \file RMF/Vector.h
 *  \brief Represent coordinates.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_VECTOR_H
#define RMF_VECTOR_H

#include <RMF/config.h>
#include <boost/array.hpp>
#include "types.h"

RMF_ENABLE_WARNINGS

namespace RMF {
template <unsigned int>
class Vector;
typedef Vector<3> Vector3;
typedef Vector<4> Vector4;
typedef std::vector<Vector3> Vector3s;
typedef std::vector<Vector4> Vector4s;

/** Represent a point.
 */
template <unsigned int D>
class Vector {
  double data_[D];

 public:
  Vector() {}
  Vector(const Floats& input) {
    RMF_USAGE_CHECK(input.size() == D, "sizes don't match");
    std::copy(input.begin(), input.end(), data_);
  }
  Vector(double x, double y, double z) {
    RMF_USAGE_CHECK(D == 3, "3 args to non-3D constructor");
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
  }
  Vector(double x, double y, double z, double q) {
    RMF_USAGE_CHECK(D == 4, "4 args to non-4D constructor");
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
    data_[3] = q;
  }
  RMF_SHOWABLE(Vector, Floats(data_, data_ + 3));
#ifndef SWIG
  double operator[](unsigned int i) const {
    RMF_USAGE_CHECK(i < D, "Out of range");
    return data_[i];
  }
  typedef const double* const_iterator;
  typedef double* iterator;
  const_iterator begin() const { return data_; }
  const_iterator end() const { return data_ + D; }
  iterator begin() { return data_; }
  iterator end() { return data_ + D; }
#endif
#if !defined(RMF_DOXYGEN)
#ifndef SWIG
  typedef double value_type;
  typedef std::random_access_iterator_tag iterator_category;
  typedef std::ptrdiff_t difference_type;
  typedef double* pointer;
  typedef double& reference;
  Vector(unsigned int s) { RMF_USAGE_CHECK(s == D, "Sizes don't match."); }
  void resize(unsigned int s) { RMF_USAGE_CHECK(s == D, "Sizes don't match."); }
  double& operator[](unsigned int i) {
    RMF_USAGE_CHECK(i < D, "Out of range");
    return data_[i];
  }
#endif
  double __getitem__(unsigned int i) const {
    if (i >= D) throw UsageException();
    return data_[i];
  }
  unsigned int __len__() const { return D; }
#endif
};

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_VECTOR_3D_H */
