/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_LOADED_FRAME_H
#define RMF_INTERNAL_SHARED_DATA_LOADED_FRAME_H

#include <RMF/config.h>
#include "../Key.h"
#include "../types.h"
#include "../names.h"
#include "../enums.h"
#include "../ID.h"
#include "../constants.h"
#include "../infrastructure_macros.h"

RMF_ENABLE_WARNINGS

namespace RMF {

namespace internal {

class SharedDataLoadedFrame {
  FrameID id_;
  FrameIDs parents_, children_;
  std::string name_;
  FrameType type_;

 protected:
  void set_loaded_frame(FrameID id) {
    RMF_INTERNAL_CHECK(id != FrameID(), "Uninitialized SharedDataLoadedFrame.");
    RMF_INTERNAL_CHECK(id != ALL_FRAMES,
                       "ALL_FRAMES in SharedDataLoadedFrame.");
    id_ = id;
  }

 public:
  SharedDataLoadedFrame() {}
  std::string get_loaded_frame_name() const { return name_; }
  FrameType get_loaded_frame_type() const { return type_; }
  const FrameIDs& get_loaded_frame_parents() const { return parents_; }
  const FrameIDs& get_loaded_frame_children() const { return children_; }
  FrameID get_loaded_frame() const {
    RMF_INTERNAL_CHECK(id_ != ALL_FRAMES,
                       "ALL_FRAMES in SharedDataLoadedFrame.");
    return id_;
  }
  void set_loaded_frame_name(std::string n) { name_ = n; }
  void set_loaded_frame_type(FrameType t) { type_ = t; }
  void set_loaded_frame_parents(const FrameIDs& parents) { parents_ = parents; }
  void set_loaded_frame_children(const FrameIDs& children) {
    children_ = children;
  }
  void add_child_frame(FrameID id) { children_.push_back(id); }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_LOADED_FRAME_H */
