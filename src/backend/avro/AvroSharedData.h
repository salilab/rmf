/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF__INTERNAL_AVRO_SHARED_DATA_H
#define RMF__INTERNAL_AVRO_SHARED_DATA_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include <RMF/infrastructure_macros.h>
#include <RMF/constants.h>
#include <RMF/internal/map.h>
#include <RMF/internal/set.h>
#include <RMF/internal/AllJSON.h>
#include "AvroSharedData.types.h"

namespace RMF {
  namespace internal {

    class EmptyAvroShareData: public SharedData {
    public:
      EmptyAvroShareData(std::string path): SharedData(path) {}
    };

    template <class Base>
    class AvroSharedData: public Base {
      typedef Base P;
      RMF_internal::All all_;
      bool dirty_;
      bool read_only_;

      typedef map<Category, std::string> CategoryNameMap;
      typedef map<std::string, Category> NameCategoryMap;
      CategoryNameMap category_name_map_;
      NameCategoryMap name_category_map_;
      struct KeyData {
        std::string name;
        Category category;
        int type_index;
      };
      typedef map<unsigned int, KeyData> KeyDataMap;
      KeyDataMap key_data_map_;
      typedef map<std::string, unsigned int> NameKeyInnerMap;
      typedef map<Category, NameKeyInnerMap> NameKeyMap;
      NameKeyMap name_key_map_;

      vector<std::string> node_keys_;

      RMF_internal::Data null_frame_data_;

      // begin specific data

      const RMF_internal::Data &get_frame_data(std::string category,
                                 int frame) const {
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

      RMF_internal::NodeData null_node_data_;

      const RMF_internal::NodeData &get_node_frame_data(int node,
                                    std::string category,
                                    int frame) const {
        const RMF_internal::Data &data= get_frame_data(category, frame);
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

      RMF_internal::Data &access_frame_data(std::string category,
                              int frame) {
        dirty_=true;
        if (all_.category[category].size() <= (frame+1)) {
          all_.category[category].resize(frame+2);
          dirty_=true;
        }
        return all_.category[category][frame+1];
      }


      RMF_internal::NodeData &access_node_data(int node,
                                 std::string category,
                                 int frame) {
        dirty_=true;
        RMF_internal::Data &data= access_frame_data(category, frame);
        return data.nodes[get_node_string(node)];
      }

      template <class TypeTraits>
    Key<TypeTraits>
        get_key_helper(Category category,
                       std::string name) {
        typename NameKeyInnerMap::const_iterator it
          = name_key_map_[category].find(name);
        if (it == name_key_map_[category].end()) {
          unsigned int id= key_data_map_.size();
          key_data_map_[id].name=name;
          key_data_map_[id].category=category;
          key_data_map_[id].type_index=TypeTraits::get_index();
          name_key_map_[category][name]=id;
          RMF_INTERNAL_CHECK(get_key_helper<TypeTraits>(category,
                                                        name)
                             == Key<TypeTraits>(id),
                             "Keys don't match");
          return Key<TypeTraits>(id);
        } else {
          int id= it->second;
          RMF_INTERNAL_CHECK(name==it->first,
                             "Odd names");
          return Key<TypeTraits>(id);
        }
      }

      const std::string &get_node_string(int node) const {
        return node_keys_[node];
      }

      template <class TypeTraits>
        const std::string &get_key_string(Key<TypeTraits> k) const {
        RMF_INTERNAL_CHECK(k.get_id() >=0,
                           "Bad key");
        return key_data_map_.find(k.get_id())->second.name;
      }

      void initialize_categories();
      void initialize_node_keys();
    public:
      void set_current_frame(int frame);
      void flush();
      void reload();
      // end specific functions

      RMF_FOREACH_TYPE(RMF_AVRO_SHARED_TYPE);

      AvroSharedData(std::string g, bool create, bool read_only);
      ~AvroSharedData();
      std::string get_name(unsigned int node) const;
      unsigned int get_type(unsigned int node) const;
      int add_child(int node, std::string name, int t);
      void add_child(int node, int child_node);
      Ints get_children(int node) const;
      void save_frames_hint(int) {}
      unsigned int get_number_of_frames() const;
      Categories get_categories() const;
      Category get_category(std::string name);
      std::string get_category_name(Category kc) const;
      std::string get_description() const;
      void set_description(std::string str);
      void set_frame_name(std::string str);
      std::string get_frame_name() const;
    };

  } // namespace internal
} /* namespace RMF */


#include "AvroSharedData.impl.h"

#endif /* RMF__INTERNAL_AVRO_SHARED_DATA_H */
