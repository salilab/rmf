/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "file_data.h"
#include "types_encode_decode.h"
#include "generated/embed_jsons.h"
#include <avrocpp/api/DataFile.hh>
#include <avrocpp/api/Compiler.hh>
#include <boost/scoped_ptr.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
Frame get_frame(FrameID id, internal_avro::DataFileReader<Frame>& reader) {
  RMF_TRACE(get_logger(), "Looking for frame " << id);

  do {
    Frame ret;
    reader.read(ret);
    if (ret.id == id) {
      RMF_INFO(get_logger(), "Loaded frame " << ret.id << " " << id
                                             << std::endl);
      return ret;
    } else {
      RMF_TRACE(get_logger(), "Found frame for frame " << ret.id);
    }
  } while (true);
}
}
}

RMF_DISABLE_WARNINGS
