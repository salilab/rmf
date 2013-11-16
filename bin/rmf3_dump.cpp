/**
 * Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#include "backend/avro2/raw_frame.h"
#include "backend/AvroCpp/api/DataFile.hh"
#include "backend/AvroCpp/api/Compiler.hh"
#include "backend/AvroCpp/api/Stream.hh"
#include "backend/AvroCpp/api/Encoder.hh"
#include "generated/embed_jsons.h"
#include <RMF/info.h>
#include "common.h"

namespace {
std::string description("Dump frames from an rmf3 file.");
}

int main(int argc, char** argv) {
  try {
    RMF_ADD_INPUT_FILE("rmf");
    process_options(argc, argv);
    rmf_avro::ValidSchema schema =
        rmf_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json);
    rmf_avro::DataFileReader<rmf_raw_avro2::Frame> reader(input.c_str(),
                                                          schema);
    rmf_raw_avro2::Frame frame;
    while (reader.read(frame)) {
      rmf_avro::EncoderPtr encoder = rmf_avro::jsonEncoder(schema);
      std::auto_ptr<rmf_avro::OutputStream> os =
          rmf_avro::ostreamOutputStream(std::cout);
      encoder->init(*os);
      rmf_avro::encode(*encoder, frame);

      // apparently these are necessary
      encoder->flush();
      os->flush();
    }
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
