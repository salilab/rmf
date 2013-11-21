/**
 * Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#include "backend/avro2/raw_frame.h"
#include "avrocpp/api/DataFile.hh"
#include "avrocpp/api/Compiler.hh"
#include "avrocpp/api/Stream.hh"
#include "avrocpp/api/Encoder.hh"
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
    internal_avro::ValidSchema schema =
        internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json);
    internal_avro::DataFileReader<rmf_raw_avro2::Frame> reader(input.c_str(),
                                                               schema);
    rmf_raw_avro2::Frame frame;
    try {
      while (reader.read(frame)) {
        std::size_t offset = reader.blockOffsetBytes();
        std::cout << "Block offset " << offset << std::endl;
        internal_avro::EncoderPtr encoder = internal_avro::jsonEncoder(schema);
        boost::shared_ptr<internal_avro::OutputStream> os =
            internal_avro::ostreamOutputStream(std::cout);
        encoder->init(*os);
        internal_avro::encode(*encoder, frame);

        // apparently these are necessary
        encoder->flush();
        os->flush();
        std::cout << std::endl;
      }
    }
    catch (std::exception e) {
    }
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
