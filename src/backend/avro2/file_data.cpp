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
#include "types.h"
#include "types_encode_decode.h"
#include "generated/embed_jsons.h"
#include <backend/AvroCpp/api/DataFile.hh>
#include <backend/AvroCpp/api/Compiler.hh>
#include <backend/AvroCpp/api/Decoder.hh>

RMF_ENABLE_WARNINGS



namespace RMF {
namespace avro2 {
FileData get_file_data(std::string name) {
  typedef rmf_avro::DataFileReader<FileData> Reader;
  Reader reader(name.c_str(),
                rmf_avro::compileJsonSchemaFromString(data_avro2::frame_json));
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
void validate_file_data() {
  rmf_avro::DecoderPtr ve = rmf_avro::validatingDecoder(
      rmf_avro::compileJsonSchemaFromString(data_avro2::frame_json),
      rmf_avro::binaryDecoder());
  FileData fd;
  rmf_avro::decode(*ve, fd);
}
}
}

RMF_DISABLE_WARNINGS
