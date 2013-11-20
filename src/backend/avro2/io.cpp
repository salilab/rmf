/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/config.h>
#include "write.h"
#include "file_data.h"
#include "frame.h"
#include "io.h"
#include <RMF/internal/shared_data_ranges.h>
#include "internal/shared_data_maps.h"
#include "generated/embed_jsons.h"
#include <backend/AvroCpp/api/Compiler.hh>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/make_shared.hpp>

RMF_ENABLE_WARNINGS
/*
 Todo:
   store keys in Frame frames as needed
assume category/key equality. that will simplify things a lot.
 */
namespace RMF {

namespace avro2 {

namespace {

template <class Traits, class Loader>
void load(internal::SharedData *shared_data, const KeyMaps<Traits> &keys,
          const internal::TypeData<Traits> &data, Loader) {
  typedef std::pair<ID<Traits>, Category> KKP;
  RMF_FOREACH(KKP kp, keys.category) {
    shared_data->ensure_key(kp.second, kp.first,
                            keys.name.find(kp.first)->second, Traits());
  }
  if (!data.empty()) {
    RMF_TRACE(get_logger(), "Found data for " << data.size() << " nodes.");
  }
  Loader::access_data(shared_data, Traits()) = data;
}

template <class Traits, class Loader>
bool save(KeyType key_type, const internal::SharedData *shared_data,
          KeyMaps<Traits> &keys, std::vector<KeyInfo> *keys_changed,
          internal::TypeData<Traits> &data,
          internal::TypeData<Traits> *change_data, Loader) {
  bool ret = false;
  RMF_FOREACH(ID<Traits> k, shared_data->get_keys(Traits())) {
    if (keys.category.find(k) == keys.category.end()) {
      ret = true;
      std::string name = shared_data->get_name(k);
      keys.category[k] = shared_data->get_category(k);
      keys.name[k] = name;
      KeyInfo ki;
      ki.name = name;
      ki.category = shared_data->get_category(k);
      ki.id = k.get_index();
      ki.type = key_type;
      keys_changed->push_back(ki);
    } else {
      RMF_INTERNAL_CHECK(
          keys.name.find(k) != keys.name.end() &&
              keys.name.find(k)->second == shared_data->get_name(k),
          "Names don't match");
    }
  }

  if (change_data) {
    RMF_FOREACH(typename internal::TypeData<Traits>::const_reference kpd,
                Loader::get_data(shared_data, Traits())) {
      if (data.find(kpd.first) == data.end() && !kpd.second.empty()) {
        ret = true;
        change_data->operator[](kpd.first) = kpd.second;
      } else {
        RMF_FOREACH(typename internal::KeyData<Traits>::const_reference npd,
                    kpd.second) {
          if (data[kpd.first].find(npd.first) == data[kpd.first].end()) {
            ret = true;
            change_data->operator[](kpd.first)[npd.first] = npd.second;
          }
        }
      }
    }
  }
  data = Loader::get_data(shared_data, Traits());
  return ret;
}

template <class Loader>
void load_all(const std::vector<std::pair<Category, std::string> > &categories,
              internal::SharedData *shared_data, const KeyData &keys,
              const DataTypes &data, Loader) {
  typedef std::pair<Category, std::string> CP;
  RMF_FOREACH(CP cp, categories) {
    shared_data->ensure_category(cp.first, cp.second);
  }
  load(shared_data, keys.float_keys, data.float_data, Loader());
  load(shared_data, keys.int_keys, data.int_data, Loader());
  load(shared_data, keys.string_keys, data.string_data, Loader());
  load(shared_data, keys.floats_keys, data.floats_data, Loader());
  load(shared_data, keys.ints_keys, data.ints_data, Loader());
  load(shared_data, keys.strings_keys, data.strings_data, Loader());

  load(shared_data, keys.vector3_keys, data.vector3_data, Loader());
  load(shared_data, keys.vector4_keys, data.vector4_data, Loader());
  load(shared_data, keys.vector3s_keys, data.vector3s_data, Loader());
}

template <class Loader>
bool save_all(FileData &file_data, FileDataChanges &file_data_changes,
              const internal::SharedData *shared_data, DataTypes &data,
              DataTypes *data_changes, Loader) {
  bool ret = false;
  Categories categories = shared_data->get_categories();
  for (unsigned int i = file_data.categories.size(); i < categories.size();
       ++i) {
    ret = true;
    file_data.categories.push_back(
        std::make_pair(categories[i], shared_data->get_name(categories[i])));
    file_data_changes.categories.push_back(file_data.categories.back());
  }

  ret = save(FLOAT, shared_data, file_data.keys.float_keys,
             &file_data_changes.keys, data.float_data,
             data_changes ? &data_changes->float_data : NULL, Loader()) ||
        ret;
  ret = save(INT, shared_data, file_data.keys.int_keys, &file_data_changes.keys,
             data.int_data, data_changes ? &data_changes->int_data : NULL,
             Loader()) ||
        ret;
  ret = save(STRING, shared_data, file_data.keys.string_keys,
             &file_data_changes.keys, data.string_data,
             data_changes ? &data_changes->string_data : NULL, Loader()) ||
        ret;
  ret = save(FLOATS, shared_data, file_data.keys.floats_keys,
             &file_data_changes.keys, data.floats_data,
             data_changes ? &data_changes->floats_data : NULL, Loader()) ||
        ret;
  ret = save(INTS, shared_data, file_data.keys.ints_keys,
             &file_data_changes.keys, data.ints_data,
             data_changes ? &data_changes->ints_data : NULL, Loader()) ||
        ret;
  ret = save(STRINGS, shared_data, file_data.keys.strings_keys,
             &file_data_changes.keys, data.strings_data,
             data_changes ? &data_changes->strings_data : NULL, Loader()) ||
        ret;

  ret = save(VECTOR3, shared_data, file_data.keys.vector3_keys,
             &file_data_changes.keys, data.vector3_data,
             data_changes ? &data_changes->vector3_data : NULL, Loader()) ||
        ret;
  ret = save(VECTOR4, shared_data, file_data.keys.vector4_keys,
             &file_data_changes.keys, data.vector4_data,
             data_changes ? &data_changes->vector4_data : NULL, Loader()) ||
        ret;
  ret = save(VECTOR3S, shared_data, file_data.keys.vector3s_keys,
             &file_data_changes.keys, data.vector3s_data,
             data_changes ? &data_changes->vector3s_data : NULL, Loader()) ||
        ret;
  return ret;
}
}

void Avro2IO::commit() {
  if (file_data_dirty_) {
    RMF_INFO(get_logger(), "Writing static data in commit");
    write(writer_.get(), file_data_changes_);
    file_data_dirty_ = false;
    file_data_changes_ = FileDataChanges();
  }
  if (frame_.id != FrameID()) {
    RMF_INFO(get_logger(), "Writing frame " << frame_.id << " in commit");
    write(writer_.get(), frame_);
    frame_.id = FrameID();
  }
}

Avro2IO::Avro2IO(std::string name) : path_(name), file_data_dirty_(false) {
  if (boost::filesystem::exists(name)) {
  } else {
    writer_.reset(new rmf_avro::DataFileWriterBase(
        path_.c_str(),
        rmf_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json),
        16 * 1024));
  }
}
void Avro2IO::save_loaded_frame(const internal::SharedData *shared_data) {
  if (frame_.id != FrameID()) {
    RMF_INFO(get_logger(), "Writing frame " << frame_.id
                                            << " to make space for new one");
    write(writer_.get(), frame_);
  }
  FrameID id = shared_data->get_loaded_frame();
  frame_ = avro2::Frame();
  frame_.id = id;
  frame_.parents = shared_data->get_loaded_frame_parents();
  frame_.type = shared_data->get_loaded_frame_type();
  frame_.name = shared_data->get_loaded_frame_name();
  save_all(file_data_, file_data_changes_, shared_data, frame_.data, NULL,
           internal::LoadedValues());
}
void Avro2IO::load_loaded_frame(internal::SharedData *shared_data) {
  FrameID id = shared_data->get_loaded_frame();
  RMF_INFO(get_logger(), "Loading frame " << id << " from file");
  if (frame_.id != FrameID() && id < frame_.id) {
    // kind of icky hack to handle being in the block
    reader_.reset();
  }
  frame_ = avro2::get_frame(file_data_, path_, id, reader_);
  shared_data->set_loaded_frame_name(frame_.name);
  shared_data->set_loaded_frame_type(FrameType(frame_.type));
  if (file_data_.frame_children.size() > id.get_index()) {
    shared_data->set_loaded_frame_children(
        file_data_.frame_children[id.get_index()]);
  } else {
    shared_data->set_loaded_frame_children(FrameIDs());
  }
  shared_data->set_loaded_frame_parents(frame_.parents);
  load_all(file_data_.categories, shared_data, file_data_.keys, frame_.data,
           internal::LoadedValues());
}

void Avro2IO::load_static_frame(internal::SharedData *shared_data) {
  load_all(file_data_.categories, shared_data, file_data_.keys, file_data_.data,
           internal::StaticValues());
}

void Avro2IO::save_static_frame(const internal::SharedData *shared_data) {
  bool changed =
      save_all(file_data_, file_data_changes_, shared_data, file_data_.data,
               &file_data_changes_.data, internal::StaticValues());
  file_data_dirty_ = changed || file_data_dirty_;
}

void Avro2IO::load_file(internal::SharedData *shared_data) {
  // set producer and description
  // for some weird reason, mac os 10.8 clang needs this two step thing
  FileData fd = get_file_data(path_);
  file_data_ = fd; // get_file_data(path_);
  RMF_INFO(get_logger(), "Found " << get_number_of_frames() << " frames");
  shared_data->set_description(file_data_.description);
  shared_data->set_producer(file_data_.producer);
  shared_data->set_number_of_frames(get_number_of_frames());
  shared_data->set_file_type("rmf3");

  typedef std::pair<Category, std::string> CP;
  RMF_FOREACH(CP cp, file_data_.categories) {
    shared_data->ensure_category(cp.first, cp.second);
  }
}

void Avro2IO::save_file(const internal::SharedData *shared_data) {
  if (shared_data->get_description() != file_data_.description) {
    file_data_changes_.description = shared_data->get_description();
    file_data_.description.set(shared_data->get_description());
    file_data_dirty_ = true;
  }
  if (shared_data->get_producer() != file_data_.producer) {
    file_data_changes_.producer = shared_data->get_producer();
    file_data_.producer.set(shared_data->get_producer());
    file_data_dirty_ = true;
  }
}

void Avro2IO::load_hierarchy(internal::SharedData *shared_data) {
  // can swap later
  shared_data->access_node_hierarchy() = file_data_.nodes;
}

void Avro2IO::save_hierarchy(const internal::SharedData *shared_data) {
  RMF_FOREACH(NodeID n, get_nodes(shared_data)) {
    HierarchyNode cur;
    cur.id = n;
    bool cur_dirty = false;
    if (file_data_.nodes.size() <= n.get_index()) {
      cur_dirty = true;
      file_data_.nodes.resize(n.get_index() + 1);
      file_data_.nodes[n.get_index()].type = shared_data->get_type(n);
      cur.type = shared_data->get_type(n);
    }
    std::string name = shared_data->get_name(n);
    if (file_data_.nodes[n.get_index()].name != name) {
      cur_dirty = true;
      file_data_.nodes[n.get_index()].name = name;
      cur.name = name;
    }
    if (shared_data->get_parents(n).size() !=
        file_data_.nodes[n.get_index()].parents.size()) {
      cur_dirty = true;
      NodeIDs cur_parents = shared_data->get_parents(n);
      NodeIDs new_parents(
          cur_parents.begin() + file_data_.nodes[n.get_index()].parents.size(),
          cur_parents.end());
      file_data_.nodes[n.get_index()].parents = cur_parents;
      cur.parents = new_parents;
    }
    if (cur_dirty) {
      file_data_changes_.nodes.push_back(cur);
      file_data_dirty_ = true;
    }
  }
}

void Avro2IO::flush() { commit(); }
unsigned int Avro2IO::get_number_of_frames() const {
  if (file_data_.max_id == FrameID()) return 0;  
  else return file_data_.max_id.get_index() + 1;
}

Avro2IO::~Avro2IO() {
  if (writer_) {
    commit();
    writer_->flush();
  }
}

std::string Avro2IOFactory::get_file_extension() const { return ".rmf3"; }
boost::shared_ptr<backends::IO> Avro2IOFactory::read_file(
    const std::string &name) const {
  return boost::make_shared<Avro2IO>(name);
}
boost::shared_ptr<backends::IO> Avro2IOFactory::create_buffer(BufferHandle)
    const {
  return boost::shared_ptr<backends::IO>();
}
boost::shared_ptr<backends::IO> Avro2IOFactory::read_buffer(BufferConstHandle)
    const {
  return boost::shared_ptr<backends::IO>();
}
boost::shared_ptr<backends::IO> Avro2IOFactory::create_file(
    const std::string &name) const {
  return boost::make_shared<Avro2IO>(name);
}
Avro2IOFactory::~Avro2IOFactory() {}

backends::IOFactoryRegistrar<Avro2IOFactory> registrar;

}  // namespace
} /* namespace RMF */

RMF_DISABLE_WARNINGS
