/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef SRC_BACKEND_AVRO2_FRAME_H_1753077812__H_
#define SRC_BACKEND_AVRO2_FRAME_H_1753077812__H_

#include "avrocpp/api/DataFile.hh"
#include <boost/shared_ptr.hpp>

#include "RMF/ID.h"
#include "RMF/config.h"
#include "types.h"

namespace internal_avro {
template <typename T>
class DataFileReader;
}  // namespace internal_avro

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
RMFEXPORT void load_frame(FrameID id,
                          internal_avro::DataFileReader<Frame> &reader,
                          Frame &frame);
}
}

RMF_DISABLE_WARNINGS
#endif
