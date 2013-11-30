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
#include <boost/array.hpp>
#include <algorithm>

RMF_ENABLE_WARNINGS

namespace RMF {
template <unsigned int>
class Vector;
/** A Vector3 */
typedef Vector<3> Vector3;
/** A Vector3 */
typedef Vector<4> Vector4;
/** Many Vector3s */
typedef std::vector<Vector3> Vector3s;
/** Many Vector4s */
typedef std::vector<Vector4> Vector4s;

/** Represent a point.
 */
template <unsigned int D>
class Vector {
  float data_[D];

 public:
  Vector() {}
  template <class It>
  Vector(It b, It e) {
    std::copy(b, e, data_);
  }
  template <class Range>
  explicit Vector(Range r) {
    RMF_USAGE_CHECK(std::distance(r.begin(), r.end()) == D,
                    "sizes don't match");
    std::copy(r.begin(), r.end(), data_);
  }
  Vector(const std::vector<float>& input) {
    RMF_USAGE_CHECK(input.size() == D, "sizes don't match");
    std::copy(input.begin(), input.end(), data_);
  }
  Vector(float x, float y, float z) {
    RMF_USAGE_CHECK(D == 3, "3 args to non-3D constructor");
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
  }
  Vector(float x, float y, float z, float q) {
    RMF_USAGE_CHECK(D == 4, "4 args to non-4D constructor");
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
    data_[3] = q;
  }
  RMF_SHOWABLE(Vector, std::vector<float>(data_, data_ + 3));
#ifndef SWIG
  float operator[](unsigned int i) const {
    RMF_USAGE_CHECK(i < D, "Out of range");
    return data_[i];
  }
  typedef const float* const_iterator;
  typedef float* iterator;
  const_iterator begin() const { return data_; }
  const_iterator end() const { return data_ + D; }
  iterator begin() { return data_; }
  iterator end() { return data_ + D; }
#endif
#if !defined(RMF_DOXYGEN)
#ifndef SWIG
  typedef float value_type;
  typedef std::random_access_iterator_tag iterator_category;
  typedef std::ptrdiff_t difference_type;
  typedef float* pointer;
  typedef float& reference;
  Vector(unsigned int s) { RMF_USAGE_CHECK(s == D, "Sizes don't match."); }
  void resize(unsigned int s) { RMF_USAGE_CHECK(s == D, "Sizes don't match."); }
  float& operator[](unsigned int i) {
    RMF_USAGE_CHECK(i < D, "Out of range");
    return data_[i];
  }
#endif
  float __getitem__(unsigned int i) const {
    if (i >= D) throw IndexException();
    return data_[i];
  }
  unsigned int __len__() const { return D; }
#endif
  static unsigned int get_dimension() { return D; }
};

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_VECTOR_3D_H */
