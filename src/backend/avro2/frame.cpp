/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "avrocpp/api/DataFile.hh"
#include <ostream>

#include "RMF/infrastructure_macros.h"
#include "RMF/log.h"
#include "backend/avro2/types.h"
#include "types_encode_decode.h"  // IWYU pragma: keep
#include "frame.h"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
void load_frame(FrameID id, internal_avro::DataFileReader<Frame>& reader,
                Frame& frame) {
  RMF_TRACE("Looking for frame " << id);

  do {
    frame = Frame();
    reader.read(frame);
    if (frame.id == id) {
      RMF_INFO("Loaded frame " << frame.id << " " << id << std::endl);
      return;
    } else {
      RMF_INFO("Found frame for frame " << frame.id);
    }
  } while (true);
}
}
}

RMF_DISABLE_WARNINGS
