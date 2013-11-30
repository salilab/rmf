/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <avrocpp/api/DataFile.hh>
#include <ostream>

#include "RMF/infrastructure_macros.h"
#include "RMF/log.h"
#include "backend/avro2/types.h"
#include "types_encode_decode.h" // IWYU pragma: keep
#include "frame.h"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
Frame get_frame(FrameID id, internal_avro::DataFileReader<Frame>& reader) {
  RMF_TRACE("Looking for frame " << id);

  do {
    Frame ret;
    reader.read(ret);
    if (ret.id == id) {
      RMF_INFO("Loaded frame " << ret.id << " " << id
                                             << std::endl);
      return ret;
    } else {
      RMF_INFO("Found frame for frame " << ret.id);
    }
  } while (true);
}
}
}

RMF_DISABLE_WARNINGS
