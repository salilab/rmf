/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/SetCurrentFrame.h>

RMF_ENABLE_WARNINGS

RMF_VECTOR_DEF(SetCurrentFrame);

namespace RMF {
SetCurrentFrame::SetCurrentFrame(FileConstHandle file, FrameID current_frame)
    : file_(file), old_frame_(file.get_current_frame()) {
  if (current_frame != ALL_FRAMES) {
    file.set_current_frame(current_frame);
  }
}
SetCurrentFrame::~SetCurrentFrame() {
  if (old_frame_ != ALL_FRAMES && old_frame_ != FrameID()) {
    file_.set_current_frame(old_frame_);
  }
}
}

RMF_DISABLE_WARNINGS
