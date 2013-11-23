/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "file_data.h"
#include "types.h"
#include "types_encode_decode.h"
#include "generated/embed_jsons.h"
#include <avrocpp/api/DataFile.hh>
#include <avrocpp/api/Compiler.hh>
#include <avrocpp/api/Decoder.hh>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
FileData get_file_data(internal_avro::DataFileReader<FileData> &reader) {
  FileData ret;
  try {
    while (reader.read(ret)) {
      ret.frame_block_offsets[ret.cur_id] = reader.blockOffsetBytes();
    }
  }
  catch (const std::exception &e) {
  }
  return ret;
}
}
}

RMF_DISABLE_WARNINGS
