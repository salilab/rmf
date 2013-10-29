/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_BACKWARDS_IO_H
#define RMF_INTERNAL_BACKWARDS_IO_H

#include <RMF/config.h>
#include "IO.h"
#include "shared_data_adaptors.h"
#include <internal/clone_shared_data.h>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace backends {

template <class SD>
struct BackwardsIO : public IO {
  boost::scoped_ptr<SD> sd_;
  std::string name_;

 public:
  BackwardsIO(std::string name, bool create, bool read_only)
      : sd_(new SD(name, create, read_only)), name_(name) {}
  BackwardsIO(const std::vector<char> &buffer)
      : sd_(new SD(buffer)), name_("buffer") {}
  BackwardsIO() : sd_(new SD()), name_("buffer") {}
  virtual ~BackwardsIO() { flush(); }

 protected:
  virtual std::vector<char> get_buffer() RMF_OVERRIDE {
    return sd_->get_buffer();
  }

  virtual void reload() RMF_OVERRIDE { sd_->reload(); }

  virtual void load_loaded_frame_category(
      Category category, internal::SharedData *shared_data) RMF_OVERRIDE {
    Category file_cat = sd_->get_category(shared_data->get_name(category));
    BackwardsAdaptor backwards_shared_data(shared_data);
    RMF::internal::clone_loaded_values_category(
        sd_.get(), file_cat, &backwards_shared_data, category);
  }

  virtual void save_loaded_frame_category(
      Category category, const internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INTERNAL_CHECK(
        shared_data->get_loaded_frame() == sd_->get_loaded_frame(),
        "Loaded frames don't match");
    Category file_cat = sd_->get_category(shared_data->get_name(category));
    RMF::internal::clone_loaded_values_category(shared_data, category,
                                                sd_.get(), file_cat);
  }

  virtual void load_static_frame_category(
      Category category, internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INTERNAL_CHECK(
        shared_data->get_loaded_frame() == sd_->get_loaded_frame(),
        "Loaded frames don't match");
    BackwardsAdaptor backwards_shared_data(shared_data);
    Category file_cat = sd_->get_category(shared_data->get_name(category));
    RMF::internal::clone_static_values_category(
        sd_.get(), file_cat, &backwards_shared_data, category);
  }

  virtual void save_static_frame_category(
      Category category, const internal::SharedData *shared_data) RMF_OVERRIDE {
    Category file_cat = sd_->get_category(shared_data->get_name(category));
    RMF::internal::clone_static_values_category(shared_data, category,
                                                sd_.get(), file_cat);
  }

  virtual void load_file(internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF::internal::clone_file(sd_.get(), shared_data);
    shared_data->set_file_type(sd_->get_file_type());
    shared_data->set_number_of_frames(sd_->get_number_of_frames());
  }

  virtual void save_file(const internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF::internal::clone_file(shared_data, sd_.get());
    flush();
  }

  virtual void save_loaded_frame(const internal::SharedData *shared_data)
      RMF_OVERRIDE {
    FrameID cur = shared_data->get_loaded_frame();
    RMF_TRACE(get_logger(), "Saving frame " << cur);
    // ignore nesting relationships for now
    if (cur.get_index() >= sd_->get_number_of_frames()) {
      RMF_TRACE(get_logger(), "Adding new frame for " << cur);
      FrameID nfid = sd_->add_frame(shared_data->get_loaded_frame_name(),
                                    shared_data->get_loaded_frame_type());
      RMF_UNUSED(nfid);
      RMF_INTERNAL_CHECK(nfid == cur, "Number of frames don't match.");
    }
    sd_->set_loaded_frame(cur);
  }

  virtual void load_loaded_frame(internal::SharedData *shared_data)
      RMF_OVERRIDE {
    FrameID cur = shared_data->get_loaded_frame();
    sd_->set_loaded_frame(cur);
    shared_data->set_loaded_frame_name(sd_->get_loaded_frame_name());
    shared_data->set_loaded_frame_type(sd_->get_loaded_frame_type());
    shared_data->set_loaded_frame_parents(FrameIDs());
    shared_data->set_loaded_frame_children(FrameIDs());
  }

  virtual void load_hierarchy(internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF::internal::clone_hierarchy(sd_.get(), shared_data);
  }

  virtual void save_hierarchy(const internal::SharedData *shared_data)
      RMF_OVERRIDE {
    RMF::internal::clone_hierarchy(shared_data, sd_.get());
  }

  virtual void flush() RMF_OVERRIDE { sd_->flush(); }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_BACKWARDS_IO_H */
