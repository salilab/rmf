/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/FrameHandle.h>
#include <boost/tuple/tuple.hpp>
#include <RMF/Category.h>
#include <RMF/FileHandle.h>
#include <RMF/decorators.h>

namespace RMF {

FrameHandle::FrameHandle(int frame, internal::SharedData *shared):
  FrameConstHandle(frame, shared) {
}

FrameHandle FrameHandle::add_child(std::string name, FrameType t) {
  FrameHandle ret(get_shared_data()->add_child_frame(get_frame_id(), name, t),
                  get_shared_data());
  ret.set_as_current_frame();
  RMF_INTERNAL_CHECK(get_shared_data()->get_number_of_frames()
                     == ret.get_id().get_index() + 1,
                     "Wrong number of frames");
  return ret;
}

void FrameHandle::add_child(FrameConstHandle nh) {
  get_shared_data()->add_child_frame(get_frame_id(), nh.get_frame_id());
}


FileHandle FrameHandle::get_file() const {
  return FileHandle(get_shared_data());
}

vector<FrameHandle> FrameHandle::get_children() const {
  Ints children = get_shared_data()->get_children(get_frame_id());
  vector<FrameHandle> ret(children.size());
  for (unsigned int i = 0; i < ret.size(); ++i) {
    ret[i] = FrameHandle(children[i], get_shared_data());
  }
  return ret;
}

} /* namespace RMF */
