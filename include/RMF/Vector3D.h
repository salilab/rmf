/**
 *  \file RMF/Vector3D.h
 *  \brief Represent 3D coordinates.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_VECTOR_3D_H
#define RMF_VECTOR_3D_H

#include <RMF/config.h>
#include <boost/array.hpp>

RMF_ENABLE_WARNINGS

RMF_VECTOR_DECL(Vector3D);

RMF_ENABLE_WARNINGS namespace RMF {
  /** Represent a vector in 3d.
   */
  class Vector3D {
    double data_[3];

   public:
    Vector3D() {}
    Vector3D(double x, double y, double z) {
      data_[0] = x;
      data_[1] = y;
      data_[2] = z;
    }
    RMF_SHOWABLE(Vector3D, Floats(data_, data_ + 3));
    double operator[](unsigned int i) const { return data_[i]; }
#if !defined(RMF_DOXYGEN)
    Vector3D(unsigned int) {}
    double& operator[](unsigned int i) { return data_[i]; }
    double __getitem__(unsigned int i) const {
      if (i >= 3) throw UsageException();
      return data_[i];
    }
#endif
  };

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_VECTOR_3D_H */
