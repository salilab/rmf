/**
 * Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#include <RMF/FileHandle.h>
#include <sstream>
#include <RMF/info.h>
#include "common.h"

namespace {
std::string description("Print out information about categories and keys.");

unsigned int frame = 0;
}

int main(int argc, char** argv) {
  try {
    options.add_options()("frame,f",
                          boost::program_options::value<unsigned int>(&frame),
                          "Frame to use");
    RMF_ADD_INPUT_FILE("rmf");
    process_options(argc, argv);

    RMF::FileConstHandle rh = RMF::open_rmf_file_read_only(input);
    if (!rh.get_description().empty()) {
      std::cout << "description: " << rh.get_description();
    }
    rh.set_current_frame(RMF::FrameID(frame));
    std::cout << "frames: " << rh.get_number_of_frames() << std::endl;
    RMF::show_info(rh, std::cout);
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
