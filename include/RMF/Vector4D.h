/**
 *  \file RMF/Vector4D.h
 *  \brief Represent 4D coordinates.
 *
 *  Copyright 2007-2014 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_VECTOR_4D_H
#define RMF_VECTOR_4D_H

#include <RMF/config.h>
#include <boost/array.hpp>

RMF_ENABLE_WARNINGS

RMF_VECTOR_DECL(Vector4D);

RMF_ENABLE_WARNINGS namespace RMF {
  /** Represent a vector in 4d.
   */
  class Vector4D {
    double data_[4];

   public:
    Vector4D() {}
    Vector4D(double x, double y, double z, double w) {
      data_[0] = x;
      data_[1] = y;
      data_[2] = z;
      data_[3] = w;
    }
    RMF_SHOWABLE(Vector4D, Floats(data_, data_ + 4));
    double operator[](unsigned int i) const { return data_[i]; }
#if !defined(RMF_DOXYGEN)
    Vector4D(unsigned int) {}
    double& operator[](unsigned int i) { return data_[i]; }
    double __getitem__(unsigned int i) const {
      if (i >= 4) throw UsageException();
      return data_[i];
    }
#endif
  };

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_VECTOR_4D_H */
