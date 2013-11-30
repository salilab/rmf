/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <boost/functional/hash/hash.hpp>
#include <boost/move/utility.hpp>
#include <algorithm>
#include <exception>
#include <functional>
#include <iostream>

#include "RMF/constants.h"
#include "RMF/exceptions.h"
#include "RMF/internal/SharedData.h"
#include "RMF/internal/large_set_map.h"
#include "RMF/log.h"
#include "RMF/types.h"
#include "backend/IO.h"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

namespace {
RMF_LARGE_UNORDERED_SET<std::string> open_for_writing;
}
SharedData::SharedData(boost::shared_ptr<backends::IO> io, std::string name,
                       bool write, bool created)
    : SharedDataPath(name), number_of_frames_(0), write_(write), io_(io) {
  if (!created) {
    reload();
  }
  RMF_USAGE_CHECK(
      open_for_writing.find(get_file_path()) == open_for_writing.end(),
      "Opening a file that is still being written is asking for trouble.");
  if (write) open_for_writing.insert(get_file_path());
}

void SharedData::set_loaded_frame(FrameID frame) {
  RMF_USAGE_CHECK(!write_, "Can't call set loaded frame when writing.");
  RMF_USAGE_CHECK(frame != ALL_FRAMES, "Trying to set loaded to all frames");
  RMF_USAGE_CHECK(
      frame == FrameID() || frame.get_index() < get_number_of_frames(),
      "Trying to load a frame that isn't there");
  if (frame == get_loaded_frame()) return;
  RMF_INFO("Setting loaded frame to " << frame);
  SharedDataLoadedFrame::set_loaded_frame(frame);

  clear_loaded_values();
  if (frame != FrameID()) {
    io_->load_loaded_frame(this);
  }
}

FrameID SharedData::add_frame(std::string name, FrameType type) {
  RMF_INTERNAL_CHECK(write_, "Can't add frame if not writing");
  FrameID ret(get_number_of_frames());
  ++number_of_frames_;
  FrameID cl = get_loaded_frame();
  RMF_INTERNAL_CHECK(cl != ret, "Huh, frames are the same");
  if (cl != FrameID()) {
    add_child_frame(ret);
    if (SharedDataFile::get_is_dirty()) {
      RMF_INFO("Flushing file info");
      io_->save_file(this);
      SharedDataFile::set_is_dirty(false);
    }
    if (SharedDataHierarchy::get_is_dirty()) {
      RMF_INFO("Flushing node hierarchy");
      io_->save_hierarchy(this);
      SharedDataHierarchy::set_is_dirty(false);
    }
    io_->save_loaded_frame(this);
  }
  SharedDataLoadedFrame::set_loaded_frame(ret);

  set_loaded_frame_name(name);
  set_loaded_frame_type(type);
  if (cl != FrameID()) {
    set_loaded_frame_parents(FrameIDs(1, cl));
  }
  RMF_INTERNAL_CHECK(get_loaded_frame_children().empty(),
                     "There are already children");
  clear_loaded_values();
  return ret;
}

void SharedData::flush() {
  if (!write_) return;
  RMF_INFO("Flushing file " << get_file_path());
  if (SharedDataFile::get_is_dirty()) {
    RMF_INFO("Flushing file info");
    io_->save_file(this);
    SharedDataFile::set_is_dirty(false);
  }
  if (SharedDataHierarchy::get_is_dirty()) {
    RMF_INFO("Flushing node hierarchy");
    io_->save_hierarchy(this);
    SharedDataHierarchy::set_is_dirty(false);
  }
  if (get_static_is_dirty()) {
    RMF_INFO("Saving static frame");
    io_->save_static_frame(this);
    set_static_is_dirty(false);
  }
  io_->flush();
}

void SharedData::reload() {
  RMF_INFO("(Re)loading file " << get_file_path());
  SharedDataHierarchy::clear();
  io_->load_file(this);
  SharedDataFile::set_is_dirty(false);
  io_->load_hierarchy(this);
  SharedDataHierarchy::set_is_dirty(false);

  clear_static_values();
  io_->load_static_frame(this);
  set_static_is_dirty(false);

  clear_loaded_values();
  if (get_loaded_frame() != FrameID() &&
      get_loaded_frame().get_index() < get_number_of_frames()) {
    io_->load_loaded_frame(this);
  }
}

#define RMF_GET_STATIC_DIRTY(Traits, UCName) \
  SharedDataData<Traits>::get_static_is_dirty() ||

bool SharedData::get_static_is_dirty() const {
  return RMF_FOREACH_TYPE(RMF_GET_STATIC_DIRTY) false;
}

#define RMF_SET_STATIC_DIRTY(Traits, UCName) \
  SharedDataData<Traits>::set_static_is_dirty(tf);

void SharedData::set_static_is_dirty(bool tf) {
  RMF_FOREACH_TYPE(RMF_SET_STATIC_DIRTY);
}

#define RMF_CLEAR_LOADED(Traits, UCName) \
  SharedDataData<Traits>::clear_loaded_values();

void SharedData::clear_loaded_values() { RMF_FOREACH_TYPE(RMF_CLEAR_LOADED); }

#define RMF_CLEAR_STATIC(Traits, UCName) \
  SharedDataData<Traits>::clear_static_values();

void SharedData::clear_static_values() { RMF_FOREACH_TYPE(RMF_CLEAR_STATIC); }

SharedData::~SharedData() {
  if (write_) {
    try {
      RMF_INFO("Closing file " << get_file_path());
      flush();
      if (get_loaded_frame() != FrameID()) {
        io_->save_loaded_frame(this);
      }
      io_.reset();
    }
    catch (const std::exception &e) {
      std::cerr << "Exception caught in shared data destructor " << e.what()
                << std::endl;
    }
    open_for_writing.erase(get_file_path());
  }
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS
