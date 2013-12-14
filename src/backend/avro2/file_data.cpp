/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "avrocpp/api/DataFile.hh"
#include <algorithm>
#include <exception>

#include "RMF/ID.h"
#include "RMF/infrastructure_macros.h"
#include "file_data.h"
#include "types.h"
#include "types_encode_decode.h"  // IWYU pragma: keep

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
void load_file_data(internal_avro::DataFileReader<FileData> &reader,
                    FileData &fd) {
  // clear it
  fd = FileData();
  try {
    while (reader.read(fd)) {
      if (fd.cur_id != FrameID()) {
        fd.frame_block_offsets[fd.cur_id] = reader.blockOffsetBytes();
      }
    }
  }
  catch (const std::exception &e) {
  }
}
}
}

RMF_DISABLE_WARNINGS
