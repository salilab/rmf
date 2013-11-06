/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/SharedData.h>
#include <RMF/NodeHandle.h>
#include <boost/unordered_set.hpp>
#include <RMF/HDF5/File.h>
#include <boost/filesystem/path.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/version.hpp>
#include <RMF/log.h>
#include <boost/foreach.hpp>
#include "backend/IO.h"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {
SharedData::SharedData(boost::shared_ptr<backends::IO> io, std::string name,
                       bool write, bool created)
    : SharedDataPath(name),
      number_of_frames_(0),
      write_(write),
      io_(io) {
  if (!created) {
    reload();
  }
}

Category SharedData::get_category(std::string name) {
  Category ret = SharedDataCategory::get_category(name);
  if (!SharedDataCategory::get_is_loaded(ret)) {
    SharedDataCategory::set_is_loaded(ret, true);
    io_->load_static_frame_category(ret, this);
    if (get_loaded_frame() != FrameID() &&
        get_loaded_frame().get_index() < io_->get_number_of_frames())
      io_->load_loaded_frame_category(ret, this);
  }
  return ret;
}

Categories SharedData::get_categories() {
  Categories ret = SharedDataCategory::get_categories();
  BOOST_FOREACH(Category c, ret) {
    if (!SharedDataCategory::get_is_loaded(c)) {
      SharedDataCategory::set_is_loaded(c, true);
      io_->load_static_frame_category(c, this);
      if (get_loaded_frame() != FrameID() &&
          get_loaded_frame().get_index() < io_->get_number_of_frames())
        io_->load_loaded_frame_category(c, this);
    }
  }
  return ret;
}

void SharedData::set_loaded_frame(FrameID frame) {
  RMF_USAGE_CHECK(frame != ALL_FRAMES, "Trying to set loaded to all frames");
  if (frame == get_loaded_frame()) return;
  RMF_INFO(get_logger(), "Setting loaded frame to " << frame);
  if (get_loaded_frame() != FrameID() && write_) {
    flush();
    io_->save_loaded_frame(this);
    BOOST_FOREACH(Category cat, SharedDataCategory::get_loaded_categories()) {
      RMF_TRACE(get_logger(), "Saving frame category " << get_loaded_frame()
                                                       << ": "
                                                       << get_name(cat));
      io_->save_loaded_frame_category(cat, this);
    }
  }
  SharedDataLoadedFrame::set_loaded_frame(frame);

  clear_loaded_values();
  if (frame != FrameID()) {
    io_->load_loaded_frame(this);
    BOOST_FOREACH(Category cat, SharedDataCategory::get_loaded_categories()) {
      io_->load_loaded_frame_category(cat, this);
    }
  }
}

FrameID SharedData::add_frame(std::string name, FrameType type) {
  FrameID ret(get_number_of_frames());
  ++number_of_frames_;
  FrameID cl = get_loaded_frame();
  RMF_INTERNAL_CHECK(cl != ret, "Huh, frames are the same");
  if (cl != FrameID()) {
    add_child_frame(ret);
    if (write_) {
      flush();
      io_->save_loaded_frame(this);
      BOOST_FOREACH(Category cat, SharedDataCategory::get_loaded_categories()) {
        RMF_TRACE(get_logger(), "Saving frame category " << get_loaded_frame()
                                                         << ": "
                                                         << get_name(cat));
        io_->save_loaded_frame_category(cat, this);
      }
    }
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
  RMF_INFO(get_logger(), "Flushing file " << get_file_path());
  if (SharedDataFile::get_is_dirty()) {
    RMF_INFO(get_logger(), "Flushing file info");
    io_->save_file(this);
    SharedDataFile::set_is_dirty(false);
  }
  if (SharedDataHierarchy::get_is_dirty()) {
    RMF_INFO(get_logger(), "Flushing node hierarchy");
    io_->save_hierarchy(this);
    SharedDataHierarchy::set_is_dirty(false);
  }
  if (get_static_is_dirty()) {
    RMF_INFO(get_logger(), "Saving static frame");
    BOOST_FOREACH(Category cat, SharedDataCategory::get_loaded_categories()) {
      RMF_TRACE(get_logger(), "Saving category " << get_name(cat));
      io_->save_static_frame_category(cat, this);
    }
    set_static_is_dirty(false);
  }
  io_->flush();
}

void SharedData::reload() {
  RMF_INFO(get_logger(), "(Re)loading file " << get_file_path());
  SharedDataHierarchy::clear();
  io_->load_file(this);
  SharedDataFile::set_is_dirty(false);
  io_->load_hierarchy(this);
  SharedDataHierarchy::set_is_dirty(false);
  BOOST_FOREACH(std::string cat_name, io_->get_categories()) {
    SharedDataCategory::get_category(cat_name);
  }

  clear_static_values();
  BOOST_FOREACH(Category cat, SharedDataCategory::get_loaded_categories()) {
    io_->load_static_frame_category(cat, this);
  }
  set_static_is_dirty(false);

  clear_loaded_values();
  if (get_loaded_frame() != FrameID() &&
      get_loaded_frame().get_index() < io_->get_number_of_frames()) {
    io_->load_loaded_frame(this);
    BOOST_FOREACH(Category cat, SharedDataCategory::get_loaded_categories()) {
      io_->load_loaded_frame_category(cat, this);
    }
  }
}

bool SharedData::get_static_is_dirty() const {
  return SharedDataData<IntTraits>::get_static_is_dirty() ||
         SharedDataData<StringTraits>::get_static_is_dirty() ||
         SharedDataData<FloatTraits>::get_static_is_dirty() ||
         SharedDataData<IndexTraits>::get_static_is_dirty() ||
         SharedDataData<NodeIDTraits>::get_static_is_dirty() ||
         SharedDataData<IntsTraits>::get_static_is_dirty() ||
         SharedDataData<StringsTraits>::get_static_is_dirty() ||
         SharedDataData<FloatsTraits>::get_static_is_dirty() ||
         SharedDataData<IndexesTraits>::get_static_is_dirty() ||
         SharedDataData<NodeIDsTraits>::get_static_is_dirty();
}

void SharedData::set_static_is_dirty(bool tf) {
  SharedDataData<IntTraits>::set_static_is_dirty(tf);
  SharedDataData<StringTraits>::set_static_is_dirty(tf);
  SharedDataData<FloatTraits>::set_static_is_dirty(tf);
  SharedDataData<IndexTraits>::set_static_is_dirty(tf);
  SharedDataData<NodeIDTraits>::set_static_is_dirty(tf);
  SharedDataData<IntsTraits>::set_static_is_dirty(tf);
  SharedDataData<StringsTraits>::set_static_is_dirty(tf);
  SharedDataData<FloatsTraits>::set_static_is_dirty(tf);
  SharedDataData<IndexesTraits>::set_static_is_dirty(tf);
  SharedDataData<NodeIDsTraits>::set_static_is_dirty(tf);
}

void SharedData::clear_loaded_values() {
  SharedDataData<IntTraits>::clear_loaded_values();
  SharedDataData<StringTraits>::clear_loaded_values();
  SharedDataData<FloatTraits>::clear_loaded_values();
  SharedDataData<IndexTraits>::clear_loaded_values();
  SharedDataData<NodeIDTraits>::clear_loaded_values();
  SharedDataData<IntsTraits>::clear_loaded_values();
  SharedDataData<StringsTraits>::clear_loaded_values();
  SharedDataData<FloatsTraits>::clear_loaded_values();
  SharedDataData<IndexesTraits>::clear_loaded_values();
  SharedDataData<NodeIDsTraits>::clear_loaded_values();
}

void SharedData::clear_static_values() {
  SharedDataData<IntTraits>::clear_static_values();
  SharedDataData<StringTraits>::clear_static_values();
  SharedDataData<FloatTraits>::clear_static_values();
  SharedDataData<IndexTraits>::clear_static_values();
  SharedDataData<NodeIDTraits>::clear_static_values();
  SharedDataData<IntsTraits>::clear_static_values();
  SharedDataData<StringsTraits>::clear_static_values();
  SharedDataData<FloatsTraits>::clear_static_values();
  SharedDataData<IndexesTraits>::clear_static_values();
  SharedDataData<NodeIDsTraits>::clear_static_values();
}

std::vector<char> SharedData::get_buffer() {
  flush();
  return io_->get_buffer();
}

SharedData::~SharedData() {
  RMF_INFO(get_logger(), "Closing file " << get_file_path());
  set_loaded_frame(FrameID());
  flush();
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS
