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
#include <backend/avro/AllJSON.h>

namespace RMF {
  namespace internal {

    class SingleAvroFile: public AvroKeysAndCategories {
      RMF_internal::All all_;
      bool dirty_;

      RMF_internal::Data null_frame_data_;
      RMF_internal::NodeData null_node_data_;

      // begin specific data
    protected:
      const RMF_internal::Data &get_frame_data(Category cat,
                                               int frame) const {
        std::string category= get_category_name(cat);
        std::map<std::string, std::vector<RMF_internal::Data > >::const_iterator
          it= all_.category.find(category);
        if (it==all_.category.end()) {
          return null_frame_data_;
        }
        if (it->second.size() <= (frame+1)) {
          return null_frame_data_;
        } else {
          return it->second[frame+1];
        }
      }

      const RMF_internal::Node &get_node(unsigned int node) const {
        return all_.nodes[node];
      }

      const std::vector<RMF_internal::Node> &get_nodes_data() const {
        return all_.nodes;
      }

      RMF_internal::Node &access_node(unsigned int node) {
        dirty_=true;
        if (all_.nodes.size() <= node) {
          all_.nodes.resize(node+1);
        }
        return all_.nodes[node];
      }

      const RMF_internal::File &get_file() const {
        return all_.file;
      }

      RMF_internal::File &access_file() {
        dirty_=true;
        return all_.file;
      }

      const RMF_internal::NodeData &get_node_frame_data(int node,
                                                        Category cat,
                                                        int frame) const {
        const RMF_internal::Data &data= get_frame_data(cat, frame);
        std::map<std::string, RMF_internal::NodeData>::const_iterator
          nit= data.nodes.find(get_node_string(node));
        if (nit == data.nodes.end()) {
          return null_node_data_;
        } else {
          return nit->second;
        }
      }

      const RMF_internal::Frame& get_frame(int i) const {
        return all_.frames[i+1];
      }

      RMF_internal::Frame& access_frame(int i) {
        dirty_=true;
        if (all_.frames.size() <= i+1) {
          all_.frames.resize(i+1);
        }
        return all_.frames[i];
      }

      RMF_internal::Data &access_frame_data(Category cat,
                                            int frame) {
        std::string category= get_category_name(cat);
        dirty_=true;
        if (all_.category[category].size() <= (frame+1)) {
          all_.category[category].resize(frame+2);
          dirty_=true;
        }
        return all_.category[category][frame+1];
      }


      RMF_internal::NodeData &access_node_data(int node,
                                               Category cat,
                                               int frame) {
        dirty_=true;
        RMF_internal::Data &data= access_frame_data(cat, frame);
        return data.nodes[get_node_string(node)];
      }


      void initialize_categories();
      void initialize_node_keys();
    public:

      void flush();
      void reload();
      void set_current_frame(int frame){
        SharedData::set_current_frame(frame);
        if (all_.file.number_of_frames < frame+1) {
          access_file().number_of_frames=frame+1;
        }
      }

      SingleAvroFile(std::string path): AvroKeysAndCategories(path){}

    };

  } // namespace internal
} /* namespace RMF */


#include "AvroSharedData.impl.h"

#endif /* RMF__INTERNAL_SINGLE_AVRO_FILE_H */
