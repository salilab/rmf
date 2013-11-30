#include <assert.h>
#include <avrocpp/api/Compiler.hh>
#include <avrocpp/api/DataFile.hh>
#include <backend/avro2/raw_frame.h>
#include <boost/shared_ptr.hpp>
#include <generated/embed_jsons.h>
#include <iostream>
#include <string>

#include "RMF/ID.h"
#include "RMF/internal/paths.h"
#include "avrocpp/api/Encoder.hh"
#include "avrocpp/api/Specific.hh"
#include "avrocpp/api/Stream.hh"
#include "avrocpp/api/ValidSchema.hh"
#include "backend/avro2/file_data.h"
#include "backend/avro2/frame.h"
#include "backend/avro2/types.h"
#include "backend/avro2/write.h"

namespace {
void write(std::string name) {
  internal_avro::DataFileWriterBase writer(
      name.c_str(),
      internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json),
      16 * 1024);
  RMF::avro2::FileDataChanges file_data;
  RMF::avro2::Frame frame;
  file_data.description = "description0";
  RMF::avro2::write(&writer, file_data);
  frame.id = RMF::FrameID(0);
  RMF::avro2::write(&writer, frame);
  frame.id = RMF::FrameID(1);
  RMF::avro2::write(&writer, frame);
  file_data.description = "description1";
  RMF::avro2::write(&writer, file_data);
  frame.id = RMF::FrameID(2);
  RMF::avro2::write(&writer, frame);
}
void read(std::string name) {
  internal_avro::DataFileReader<RMF::avro2::FileData> reader(
      name.c_str(),
      internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json));
  RMF::avro2::FileData file_data = RMF::avro2::get_file_data(reader);
  assert(file_data.description == "description1");
  assert(file_data.max_id.get_index() == 2);
  internal_avro::DataFileReader<RMF::avro2::Frame> frame_reader(
      name.c_str(),
      internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json));
  for (unsigned int i = 0; i <= file_data.max_id.get_index(); ++i) {
    RMF::avro2::Frame frame = get_frame(RMF::FrameID(i), frame_reader);
  }
}
void read_raw(std::string name) {
  internal_avro::ValidSchema schema =
      internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json);
  internal_avro::DataFileReader<rmf_raw_avro2::Frame> reader(name.c_str(),
                                                             schema);
  rmf_raw_avro2::Frame frame;
  int read = 0;
  while (reader.read(frame)) {
    internal_avro::EncoderPtr encoder = internal_avro::jsonEncoder(schema);
    boost::shared_ptr<internal_avro::OutputStream> os =
        internal_avro::ostreamOutputStream(std::cout);
    encoder->init(*os);
    internal_avro::encode(*encoder, frame);
    encoder->flush();
    os->flush();
    ++read;
  }
  assert(read == 5);
}
}

int main(int, char * []) {
  std::string name = RMF::internal::get_unique_path();
  std::cout << "File is " << name << std::endl;
  write(name);
  read(name);
  read_raw(name);
  return 0;
}
