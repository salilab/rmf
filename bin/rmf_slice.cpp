/**
 * Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#include <RMF/FileHandle.h>
#include <RMF/utility.h>
#include "common.h"
#include <boost/foreach.hpp>

namespace {
std::string description("Grab frames from an rmf file");
}

int main(int argc, char** argv) {
  try {
    RMF_ADD_INPUT_FILE("rmf");
    RMF_ADD_OUTPUT_FILE("rmf");
    RMF_ADD_FRAMES;
    process_options(argc, argv);

    RMF::FileConstHandle rh = RMF::open_rmf_file_read_only(input);
    RMF::FileHandle orh = RMF::create_rmf_file(output);
    RMF::clone_file_info(rh, orh);
    orh.set_producer("rmf_slice");
    RMF::clone_hierarchy(rh, orh);
    RMF::clone_static_frame(rh, orh);
    std::cout << "Copying frames";
    BOOST_FOREACH(RMF::FrameID f, rh.get_frames()) {
      if (f.get_index() < start_frame) continue;
      if ((f.get_index() - start_frame) % step_frame != 0) continue;
      rh.set_current_frame(f);
      orh.add_frame(rh.get_current_frame_name(), rh.get_current_frame_type());
      RMF::clone_loaded_frame(rh, orh);
      if (orh.get_number_of_frames() % 10 == 0) std::cout << "." << std::flush;
    }
    std::cout << std::endl;
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
