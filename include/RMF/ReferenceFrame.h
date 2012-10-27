/**
 *  \file RMF/ReferenceFrame.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF__REFERENCE_FRAME_H
#define RMF__REFERENCE_FRAME_H

#include <RMF/config.h>
#include "internal/Transform.h"
#include "infrastructure_macros.h"

namespace RMF {
  class RigidParticleConst;
  /** Define a reference frame that internal coordinates are represented in.
   */
  class RMFEXPORT ReferenceFrame {
    internal::Transform transform_;
  public:
    ReferenceFrame(){};
    ReferenceFrame(ReferenceFrame base,
                   RigidParticleConst rb);
    RMF_SHOWABLE(ReferenceFrame, transform_);
    /** Return the global coordinates for a given set of internal coordinates.
     */
    Floats get_global_coordinates(const Floats& internal) const {
      return transform_.get_transformed(internal);
    }
  };

  typedef vector<ReferenceFrame> ReferenceFrames;

} /* namespace RMF */

#endif /* RMF__REFERENCE_FRAME_H */
