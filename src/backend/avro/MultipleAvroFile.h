/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF__INTERNAL_SINGLE_AVRO_FILE_H
#define RMF__INTERNAL_SINGLE_AVRO_FILE_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include <RMF/infrastructure_macros.h>
#include "AvroKeysAndCategories.h"
#include <avro/DataFile.hh>
#include <backend/avro/AllJSON.h>

namespace RMF {
  namespace internal {

    /* Later have laze and non-lazy frame loading so we can skip check on most
       fetches.

       split into two classes, one for creating and one for reading (with a common base)

       push get_all into base classes, then we can specialize it for the reader and
       do everything nicely with changing on set_current_frame

       merge all the static data (file, nodes, frames, base structure) into one record
       share that. Only have the dynamic stuff be different between backends

       can build cache on node keys later.
    */
    class MultipleAvroFile: public AvroKeysAndCategories {
      typedef RMF_internal::File File;
      File file_;
      bool file_dirty_;
      typedef std::vector<RMF_internal::Node> Nodes;
      Nodes nodes_;
      bool nodes_dirty_;
      typedef std::vector<RMF_internal::Frame > Frames;
      Frames frames_;
      bool frames_dirty_;

      typedef RMF_internal::StaticData StaticData;
      StaticData static_data_;
      bool static_data_dirty_;

      struct CategoryData {
        // always has the name of the file
        std::string name;
        // true if the file exists
        bool has_reader;
        avro::DataFileReader<RMF_internal::Data > reader;
        // true if it has been written, false if has_reader is true
        bool has_writer;
        avro::DataFileWriter<RMF_internal::Data > writer;
        // frame is always something valid
        RMF_internal::Data data;
        bool dirty;
        void audit() const;
      };

      vector<CategoryData> categories_;

      RMF_internal::Data null_frame_data_;
      RMF_internal::NodeData null_node_data_;

      void flush_category_data(Category cat);
      void initialize_category_reader(Category cat);
      void initialize_category_data(Category cat);
      void load_category_frame(Category cat,
                               int frame);

      std::string get_category_file_path(Category cat);
       std::string get_file_file_path(Category cat);
       std::string get_nodes_file_path(Category cat);
      std::string get_framces_file_path(Category cat);
      // begin specific data
    protected:
      const RMF_internal::Data &get_frame_data(Category cat,
                                               int frame) const {
        initialize_category_data(cat);
        load_category_frame(cat, farme);
        return categories_[cat.get_id()].data;
      }

      RMF_internal::Data &access_frame_data(Category cat,
                                            int frame) {
        initialize_category_data(cat);
        load_category_frame(cat, farme);
        categories_[cat.get_id()].dirty=true;
        return categories_[cat.get_id()].data;
      }

      const RMF_internal::Node &get_node(unsigned int node) const {
        return nodes_[node];
      }

      const std::vector<RMF_internal::Node> &get_nodes_data() const {
        return nodes_;
      }

      RMF_internal::Node &access_node(unsigned int node) {
        nodes_dirty_=true;
        if (nodes_.size() <= node) {
          nodes_.resize(node+1);
        }
        return nodes_[node];
      }

      const RMF_internal::File &get_file() const {
        return file_;
      }

      RMF_internal::File &access_file() {
        file_dirty_=true;
        return file_;
      }

      const RMF_internal::Frame& get_frame(int i) const {
        return frames_[i+1];
      }

      RMF_internal::Frame& access_frame(int i) {
        frames_dirty_=true;
        if (frames_.size() <= i+1) {
          frames_.resize(i+1);
        }
        return frames_[i];
      }

      void initialize_categories();
      void initialize_node_keys();
    public:

      void flush();
      void reload();

      MultipleAvroFile(std::string path);

      void set_current_frame(int frame);

    };

  } // namespace internal
} /* namespace RMF */


#include "AvroSharedData.impl.h"

#endif /* RMF__INTERNAL_MULTIPLE_AVRO_FILE_H */
