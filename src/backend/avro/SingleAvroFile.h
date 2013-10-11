/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SINGLE_AVRO_FILE_H
#define RMF_INTERNAL_SINGLE_AVRO_FILE_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include <RMF/infrastructure_macros.h>
#include "AvroKeysAndCategories.h"
#include <backend/avro/AllJSON.h>

RMF_ENABLE_WARNINGS namespace RMF {
  namespace avro_backend {

  class SingleAvroFile : public AvroKeysAndCategories {
    RMF_avro_backend::All all_;
    bool dirty_;
    bool text_;

    std::string* buffer_;
    bool write_to_buffer_;

    RMF_avro_backend::Data null_frame_data_;
    RMF_avro_backend::Data null_static_frame_data_;

    RMF_avro_backend::Node null_frame_real_data_;

    // begin specific data
   protected:
    const RMF_avro_backend::Data& get_frame_data(Category cat,
                                                 FrameID frame) const {
      std::string category = get_category_name(cat);
      std::map<std::string,
               std::vector<RMF_avro_backend::Data> >::const_iterator it =
          all_.category.find(category);
      if (it == all_.category.end()) {
        return frame == ALL_FRAMES ? null_static_frame_data_ : null_frame_data_;
      }
      unsigned int index;
      if (frame == ALL_FRAMES) index = 0;
      else index = frame.get_index() + 1;
      if (it->second.size() <= static_cast<unsigned int>(index)) {
        return frame == ALL_FRAMES ? null_static_frame_data_ : null_frame_data_;
      } else {
        return it->second[index];
      }
    }

    RMF_avro_backend::Data& access_frame_data(Category cat, FrameID frame) {
      std::string category = get_category_name(cat);
      dirty_ = true;
      unsigned int index;
      if (frame == ALL_FRAMES) index = 0;
      else index = frame.get_index() + 1;
      while (all_.category[category].size() <=
             static_cast<unsigned int>(index)) {
        int cur = all_.category[category].size() - 1;
        all_.category[category].push_back(RMF_avro_backend::Data());
        all_.category[category].back().frame = cur;
      }
      return all_.category[category][index];
    }

    const RMF_avro_backend::Node& get_node(NodeID node) const {
      return all_.nodes[node.get_index()];
    }

    const std::vector<RMF_avro_backend::Node>& get_nodes_data() const {
      return all_.nodes;
    }

    RMF_avro_backend::Node& access_node(NodeID node) {
      dirty_ = true;
      if (all_.nodes.size() <= static_cast<unsigned int>(node.get_index())) {
        all_.nodes.resize(node.get_index() + 1);
      }
      return all_.nodes[node.get_index()];
    }

    const RMF_avro_backend::File& get_file() const { return all_.file; }

    RMF_avro_backend::File& access_file() {
      dirty_ = true;
      return all_.file;
    }

    const RMF_avro_backend::Node& get_frame(FrameID i) const {
      if (i.get_index() + 1 >= all_.frames.size()) {
        return null_frame_real_data_;
      }
      return all_.frames[i.get_index() + 1];
    }

    const std::vector<RMF_avro_backend::Node>& get_frames() const {
      return all_.frames;
    }

    RMF_avro_backend::Node& access_frame(FrameID i) {
      dirty_ = true;
      unsigned int index;
      if (i == ALL_FRAMES) {
        index = 0;
      } else {
        index = i.get_index() +1;
      }
      if (all_.frames.size() <= index) {
        RMF_avro_backend::Node def;
        def.type = "frame";
        all_.frames.resize(index + 1, def);
      }
      return all_.frames[index];
    }
    void initialize_frames();
    void initialize_categories();
    void initialize_node_keys();
    static bool get_is_text(std::string name) {
      return name[name.size() - 1] == 't';
    }

   public:

    void set_current_frame(FrameID frame) RMF_OVERRIDE {
      if (frame == ALL_FRAMES) {
        null_frame_data_.frame = -1;
      } else {
        null_frame_data_.frame = frame.get_index();
      }
      AvroKeysAndCategories::set_current_frame(frame);
    }

    void flush() RMF_OVERRIDE;
    void reload() RMF_OVERRIDE;

    SingleAvroFile(std::string path, bool create, bool read_only);
    SingleAvroFile(std::string& buffer, bool create);
    SingleAvroFile(const std::string& buffer);
    ~SingleAvroFile() { flush(); }
    FrameID add_child(FrameID node, std::string name, FrameType t) RMF_OVERRIDE;
    void add_child(FrameID node, FrameID child_node) RMF_OVERRIDE;
    FrameIDs get_children(FrameID node) const RMF_OVERRIDE;
    virtual std::string get_name(FrameID i) const RMF_OVERRIDE;
    FrameType get_type(FrameID i) const RMF_OVERRIDE;
    unsigned int get_number_of_frames() const;
    virtual std::string get_file_type() const RMF_OVERRIDE {
      if (text_) {
        return "Single avro version 1 (text)";
      } else {
        return "Single avro version 1 (binary)";
      }
    }

  };

  }  // namespace avro_backend
}    /* namespace RMF */

RMF_DISABLE_WARNINGS

#include "AvroSharedData.impl.h"

#endif /* RMF_INTERNAL_SINGLE_AVRO_FILE_H */
