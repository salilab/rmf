/**
 *  \file RMF/RestoreCurrentFrame.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_RESTORE_CURRENT_FRAME_H
#define RMF_RESTORE_CURRENT_FRAME_H

#include "RMF/config.h"
#include "FileConstHandle.h"

RMF_ENABLE_WARNINGS
namespace RMF {

/** Restore the frame that was current on creation upon destruction.
 */
class RMFEXPORT RestoreCurrentFrame {
  FileConstHandle file_;
  FrameID old_frame_;

 public:
  RestoreCurrentFrame(FileConstHandle file);
  ~RestoreCurrentFrame();
  RMF_SHOWABLE(RestoreCurrentFrame, "To: " << old_frame_);
};

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_RESTORE_CURRENT_FRAME_H */
