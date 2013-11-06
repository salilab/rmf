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
#include "KeyFilter.h"
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

  template <class Traits, class SDC>
  ID<Traits> get_key_const(Category cat, std::string name, Traits,
                           SDC *sd) const {
    BOOST_FOREACH(ID<Traits> k, sd->get_keys(cat, Traits())) {
      if (sd->get_name(k) == name) return k;
    }
    return ID<Traits>();
  }

  template <unsigned int D, class Filter>
  void filter_vector(Filter &filter, Category cat) const {
    std::ostringstream oss;
    oss << "_vector" << D;
    StringsKey key = get_key_const(cat, oss.str(), StringsTraits(), sd_.get());
    if (key == StringsKey()) return;
    BOOST_FOREACH(std::string key_name, sd_->get_static_value(NodeID(0), key)) {
      for (unsigned int i = 0; i < 3; ++i) {
        std::ostringstream ossk;
        ossk << "_" << key_name << " " << i;
        filter.add_float_key(cat, key_name);
      }
    }
  }

  /*template <class SDA, class SDB, unsigned int D>
  void load_vector_static(SDA *sda, Category category_a, SDB *sbd,
                          Category category_b) {

                          }*/
  template <class H>
  void load_frame_category(Category category, internal::SharedData *shared_data,
                           H) {
    Category file_cat = sd_->get_category(shared_data->get_name(category));
    KeyFilter<SD> filter(sd_.get());
    if (shared_data->get_name(category) == "sequence") {
      filter.add_index_key(file_cat, "chain id");
    }
    filter_vector<3>(filter, file_cat);
    filter_vector<4>(filter, file_cat);
    RMF::internal::clone_values_category(&filter, file_cat, shared_data,
                                         category, H());
    if (shared_data->get_name(category) == "sequence") {
      IndexKey cidk =
          get_key_const(file_cat, "chain id", IndexTraits(), sd_.get());
      if (cidk != IndexKey()) {
        StringKey cidsk =
            shared_data->get_key(category, "chain id", StringTraits());
        BOOST_FOREACH(NodeID ni, get_nodes(shared_data)) {
          int ci = H::get(sd_.get(), ni, cidk);
          if (!IndexTraits::get_is_null_value(ci)) {
            H::set(shared_data, ni, cidsk, std::string(1, ci = 'A'));
          }
        }
      }
    }
    // load_vector_loaded<3>(sd_.get(), file_cat, shared_data, category);
    // load_vector_loaded<4>(sd_.get(), file_cat, shared_data, category);
  }
  template <class H>
  void save_frame_category(Category category,
                           const internal::SharedData *shared_data, H) {
    Category file_cat = sd_->get_category(shared_data->get_name(category));
    RMF::internal::clone_values_category(shared_data, category, sd_.get(),
                                         file_cat, H());
    // save_vector_loaded<3>(shared_data, category, sd_.get(), file_cat);
    // save_vector_loaded<4>(shared_data, category, sd_.get(), file_cat);
  }

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

  virtual Strings get_categories() RMF_OVERRIDE {
    Strings ret;
    BOOST_FOREACH(Category c, sd_->get_categories()) {
      ret.push_back(sd_->get_name(c));
    }
    return ret;
  }

  virtual unsigned int get_number_of_frames() RMF_OVERRIDE {
    return sd_->get_number_of_frames();
  }

  virtual void load_loaded_frame_category(
      Category category, internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Load loaded frame data for "
                               << shared_data->get_name(category)
                               << " for frame "
                               << shared_data->get_loaded_frame());
    load_frame_category(category, shared_data, internal::LoadedValues());
  }

  virtual void save_loaded_frame_category(
      Category category, const internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Save loaded frame data for "
                               << shared_data->get_name(category)
                               << " for frame "
                               << shared_data->get_loaded_frame());
    RMF_INTERNAL_CHECK(
        shared_data->get_loaded_frame() == sd_->get_loaded_frame(),
        "Loaded frames don't match");
    save_frame_category(category, shared_data, internal::LoadedValues());
  }

  virtual void load_static_frame_category(
      Category category, internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Load static frame data for "
                               << shared_data->get_name(category));
    load_frame_category(category, shared_data, internal::StaticValues());
  }

  virtual void save_static_frame_category(
      Category category, const internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Save static frame data for "
                               << shared_data->get_name(category));
    save_frame_category(category, shared_data, internal::StaticValues());
  }

  virtual void load_file(internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Loading file");
    RMF::internal::clone_file(sd_.get(), shared_data);
    shared_data->set_file_type(sd_->get_file_type());
    shared_data->set_number_of_frames(sd_->get_number_of_frames());
  }

  virtual void save_file(const internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Saving file");
    RMF::internal::clone_file(shared_data, sd_.get());
    flush();
  }

  virtual void save_loaded_frame(const internal::SharedData *shared_data)
      RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Saving frame " << shared_data->get_loaded_frame());
    FrameID cur = shared_data->get_loaded_frame();
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
    RMF_INFO(get_logger(), "Loading frame " << shared_data->get_loaded_frame());
    FrameID cur = shared_data->get_loaded_frame();
    sd_->set_loaded_frame(cur);
    shared_data->set_loaded_frame_name(sd_->get_loaded_frame_name());
    shared_data->set_loaded_frame_type(sd_->get_loaded_frame_type());
    shared_data->set_loaded_frame_parents(FrameIDs());
    shared_data->set_loaded_frame_children(FrameIDs());
  }

  virtual void load_hierarchy(internal::SharedData *shared_data) RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Loading hierarchy");
    RMF::internal::clone_hierarchy(sd_.get(), shared_data);
  }

  virtual void save_hierarchy(const internal::SharedData *shared_data)
      RMF_OVERRIDE {
    RMF_INFO(get_logger(), "Saving hierarchy");
    RMF::internal::clone_hierarchy(shared_data, sd_.get());
  }

  virtual void flush() RMF_OVERRIDE { sd_->flush(); }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_BACKWARDS_IO_H */
