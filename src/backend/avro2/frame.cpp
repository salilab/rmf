/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
Frame get_frame(
    const FileData& fd, std::string path, FrameID id,
    boost::scoped_ptr<internal_avro::DataFileReader<Frame> >& reader) {
  RMF_INTERNAL_CHECK(
      fd.frame_block_offsets.find(id) != fd.frame_block_offsets.end(),
      "No such frame found");
  RMF_TRACE(get_logger(), "Looking for frame " << id);
  int64_t offset = fd.frame_block_offsets.find(id)->second;
  if (!reader || reader->blockOffsetBytes() > offset) {
    RMF_TRACE(get_logger(), "Creating new reader");
    reader.reset(new internal_avro::DataFileReader<Frame>(
        path.c_str(),
        internal_avro::compileJsonSchemaFromString(data_avro2::frame_json)));
  }
  RMF_INTERNAL_CHECK(reader->blockOffsetBytes() <= offset,
                     "Too high an offset");
  if (reader->blockOffsetBytes() != offset) {
    RMF_TRACE(get_logger(), "Seeking to " << offset << " from "
                                          << reader->blockOffsetBytes());
    reader->seekBlockBytes(offset);
  }
  do {
    RMF_INTERNAL_CHECK(reader->blockOffsetBytes() == offset,
                       "Not at right offset");
    Frame ret;
    reader->read(ret);
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
