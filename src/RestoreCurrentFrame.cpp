/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/RestoreCurrentFrame.h>

RMF_ENABLE_WARNINGS

RMF_VECTOR_DEF(RestoreCurrentFrame);

namespace RMF {
RestoreCurrentFrame::RestoreCurrentFrame(FileConstHandle file)
    : file_(file), old_frame_(file.get_current_frame()) {}
RestoreCurrentFrame::~RestoreCurrentFrame() {
  if (old_frame_ != FrameID()) {
    file_.set_current_frame(old_frame_);
  }
}
}

RMF_DISABLE_WARNINGS
