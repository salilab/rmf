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
    RMF_FOREACH(ID<Traits> k, sd->get_keys(cat, Traits())) {
      if (sd->get_name(k) == name) return k;
    }
    return ID<Traits>();
  }

  template <unsigned int D>
  Strings get_vector_names(Category cat) const {
    std::ostringstream oss;
    oss << "_vector" << D;
    StringsKey key = get_key_const(cat, oss.str(), StringsTraits(), sd_.get());
    if (key == StringsKey()) return Strings();
    return sd_->get_static_value(NodeID(0), key);
  }

  template <unsigned int D>
  boost::array<std::string, D> get_subkey_names(std::string key_name) const {
    boost::array<std::string, D> ret;
    for (unsigned int i = 0; i < D; ++i) {
      std::ostringstream ossk;
      ossk << "_" << key_name << " " << i;
      ret[i] = ossk.str();
    }
    return ret;
  }

  template <unsigned int D, class Filter>
  void filter_vector(Filter &filter, Category cat) const {
    RMF_FOREACH(std::string key_name, get_vector_names<D>(cat)) {
      RMF_FOREACH(std::string subkey_name, get_subkey_names<D>(key_name)) {
        filter.add_float_key(cat, subkey_name);
      }
    }
  }

  template <unsigned int D, class SDA, class SDB, class H>
  void load_vector(SDA *sda, Category category_a, SDB *sdb, Category category_b,
                   H) {
    typedef ID<VectorTraits<D> > Key;
    typedef std::pair<Key, int> Data;
    boost::unordered_map<FloatKey, Data > map;
    RMF_FOREACH(std::string key_name, get_vector_names<D>(category_a)) {
      boost::array<std::string, D> subkey_names = get_subkey_names<D>(key_name);
      for (unsigned int i = 0; i < D; ++i) {
        FloatKey cur_key =
            sda->get_key(category_a, subkey_names[i], FloatTraits());
        map[cur_key].first =
            sdb->get_key(category_b, key_name, VectorTraits<D>());
        map[cur_key].second = i;
      }
    }
    if (map.empty()) return;
    RMF_FOREACH(NodeID n, internal::get_nodes(sda)) {
      typedef std::pair<FloatKey, Data> KP;
      RMF_FOREACH(KP kp, map) {
        double v = H::get(sda, n, kp.first);
        if (!FloatTraits::get_is_null_value(v)) {
          Vector<D> old = H::get(sdb, n, kp.second.first);
          old[kp.second.second] = v;
          H::set(sdb, n, kp.second.first, old);
        }
      }
    }
  }
  template <unsigned int D, class SDA, class SDB, class H>
  void save_vector(SDA *sda, Category category_a, SDB *sdb, Category category_b,
                   H) {
    typedef ID<VectorTraits<D> > VectorKey;
    std::vector<VectorKey> keys =
      sda->get_keys(category_a, VectorTraits<D>());
    typedef boost::array<ID<FloatTraits>, D > Data;
    boost::unordered_map<VectorKey, Data > map;
    Strings key_names;
    RMF_FOREACH(VectorKey k, keys) {
      std::string name = sda->get_name(k);
      key_names.push_back(name);
      boost::array<std::string, D> subkey_names = get_subkey_names<D>(name);
      for (unsigned int i = 0; i < D; ++i) {
        map[k][i] = sdb->get_key(category_b, subkey_names[i], FloatTraits());
      }
    }
    if (key_names.empty()) return;
    {
      std::ostringstream oss;
      oss << "_vector" << D;
      StringsKey k = sdb->get_key(category_b, oss.str(), StringsTraits());
      sdb->set_static_value(NodeID(0), k, key_names);
    }

    RMF_FOREACH(NodeID n, internal::get_nodes(sda)) {
      typedef std::pair<VectorKey, Data> KP;
      RMF_FOREACH(KP kp, map) {
        Vector<D> v = H::get(sda, n, kp.first);
        if (!VectorTraits<D>::get_is_null_value(v)) {
          for (unsigned int i = 0; i< D; ++i) {
            H::set(sdb, n, kp.second[i], v[i]);
          }
        }
      }
    }
  }

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
    internal::clone_values_type<IntTraits, IntTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<backward_types::IndexTraits, IntTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<backward_types::NodeIDTraits, IntTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<FloatTraits, FloatTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<StringTraits, StringTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<IntsTraits, IntsTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<backward_types::IndexesTraits, IntsTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<backward_types::NodeIDsTraits, IntsTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<FloatsTraits, FloatsTraits>(
        &filter, file_cat, shared_data, category, H());
    internal::clone_values_type<StringsTraits, StringsTraits>(
        &filter, file_cat, shared_data, category, H());

    if (shared_data->get_name(category) == "sequence") {
      backward_types::IndexKey cidk = get_key_const(
          file_cat, "chain id", backward_types::IndexTraits(), sd_.get());
      if (cidk != backward_types::IndexKey()) {
        StringKey cidsk =
            shared_data->get_key(category, "chain id", StringTraits());
        RMF_FOREACH(NodeID ni, internal::get_nodes(shared_data)) {
          int ci = H::get(sd_.get(), ni, cidk);
          if (!backward_types::IndexTraits::get_is_null_value(ci)) {
            H::set(shared_data, ni, cidsk, std::string(1, ci = 'A'));
          }
        }
      }
    }
    load_vector<3>(sd_.get(), file_cat, shared_data, category, H());
    load_vector<4>(sd_.get(), file_cat, shared_data, category, H());
  }
  template <class H>
  void save_frame_category(Category category,
                           const internal::SharedData *shared_data, H) {
    Category file_cat = sd_->get_category(shared_data->get_name(category));
    internal::clone_values_type<IntTraits, IntTraits>(
        shared_data, category, sd_.get(), file_cat, H());
    internal::clone_values_type<FloatTraits, FloatTraits>(
        shared_data, category, sd_.get(), file_cat, H());
    internal::clone_values_type<StringTraits, StringTraits>(
        shared_data, category, sd_.get(), file_cat, H());
    internal::clone_values_type<IntsTraits, IntsTraits>(
        shared_data, category, sd_.get(), file_cat, H());
    internal::clone_values_type<FloatsTraits, FloatsTraits>(
        shared_data, category, sd_.get(), file_cat, H());
    internal::clone_values_type<StringsTraits, StringsTraits>(
        shared_data, category, sd_.get(), file_cat, H());

    save_vector<3>(shared_data, category, sd_.get(), file_cat, H());
    save_vector<4>(shared_data, category, sd_.get(), file_cat, H());
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
    RMF_FOREACH(Category c, sd_->get_categories()) {
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
