/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <avrocpp/api/DataFile.hh>
#include <boost/cstdint.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <sys/_types/_int32_t.h>
#include <algorithm>
#include <exception>

#include "RMF/ID.h"
#include "RMF/compiler_macros.h"
#include "RMF/infrastructure_macros.h"
#include "file_data.h"
#include "types.h"
#include "types_encode_decode.h" // IWYU pragma: keep

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
FileData get_file_data(internal_avro::DataFileReader<FileData> &reader) {
  FileData ret;
  try {
    while (reader.read(ret)) {
      if (ret.cur_id != FrameID()) {
        ret.frame_block_offsets[ret.cur_id] = reader.blockOffsetBytes();
      }
    }
  }
  catch (const std::exception &e) {
  }
  return ret;
}
}
}

RMF_DISABLE_WARNINGS
