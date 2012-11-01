/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/CoordinateTransformer.h>
#include <RMF/decorators.h>

namespace RMF {
  CoordinateTransformer::CoordinateTransformer(CoordinateTransformer base,
                                               ReferenceFrameConst rb):
    transform_(base.transform_,
               internal::Transform(internal::Rotation(rb.get_orientation()),
                                   rb.get_coordinates())){
  }
}
