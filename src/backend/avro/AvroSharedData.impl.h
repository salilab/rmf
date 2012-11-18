/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_AVRO_SHARED_DATA_IMPL_H
#define RMF_INTERNAL_AVRO_SHARED_DATA_IMPL_H

#include "AvroSharedData.h"
#include <RMF/NodeHandle.h>
#include <RMF/Validator.h>
#include <RMF/internal/set.h>
#include <RMF/internal/avro_schemas.h>
#include <boost/lexical_cast.hpp>
#include <avro/DataFile.hh>
#include <algorithm>

namespace RMF {
  namespace internal {

    template <class Base>
    AvroSharedData<Base>::AvroSharedData(std::string g, bool create,
                                   bool read_only):
      Base(g), read_only_(read_only) {
      if (!create) {
        reload();
      } else {
        initialize_categories();
        initialize_node_keys();
        access_file().number_of_frames=0;
        // write to disk
        add_child(-1, "root", ROOT);
        flush();
      }
    }
    template <class Base>
    AvroSharedData<Base>::~AvroSharedData() {
      flush();
    }
    template <class Base>
    std::string AvroSharedData<Base>::get_name(unsigned int node) const {
      return get_node(node).name;
    }
    template <class Base>
    unsigned int AvroSharedData<Base>::get_type(unsigned int node) const {
      std::string string_type= get_node(node).type;
      unsigned int ret_type= boost::lexical_cast<NodeType>(string_type);
      return ret_type;
    }
    template <class Base>
    unsigned int AvroSharedData<Base>::get_number_of_frames() const {
      return get_file().number_of_frames;
    }
    template <class Base>
    int AvroSharedData<Base>::add_child(int node, std::string name, int t){
      int index= get_nodes_data().size();
      access_node(index).name=name;
      access_node(index).type= boost::lexical_cast<std::string>(NodeType(t));
      access_node(index).index=index;
      if (node >=0) {
        add_child(node, index);
      }
      std::ostringstream oss;
      oss << index;
      node_keys_.push_back(oss.str());
      RMF_INTERNAL_CHECK(get_type(index) ==t,
                         "Types don't match");
      return index;
    }
    template <class Base>
    void AvroSharedData<Base>::add_child(int node, int child_node){
      access_node(node).children.push_back(child_node);
    }
    template <class Base>
    Ints AvroSharedData<Base>::get_children(int node) const{
      return Ints(get_node(node).children.begin(),
                  get_node(node).children.end());
    }
    template <class Base>
    Categories AvroSharedData<Base>::get_categories() const {
      Categories ret;
      for (CategoryNameMap::const_iterator
             it= category_name_map_.begin(); it != category_name_map_.end();
           ++it) {
        ret.push_back(it->first);
      }
      return ret;
    }
    template <class Base>
    Category AvroSharedData<Base>::get_category(std::string name){
      NameCategoryMap::iterator it=name_category_map_.find(name);
      if (it== name_category_map_.end()) {
        unsigned int id= category_name_map_.size();
        Category ret(id);
        name_category_map_[name]=ret;
        category_name_map_[ret]=name;
        return ret;
      } else {
        return it->second;
      }
    }
    template <class Base>
    std::string AvroSharedData<Base>::get_category_name(Category kc) const  {
      return category_name_map_.find(kc)->second;
    }
    template <class Base>
    void AvroSharedData<Base>::set_frame_name(std::string str) {
      // offset for static data
      access_frame(P::get_current_frame()).name=str;
     }
    template <class Base>
    std::string AvroSharedData<Base>::get_frame_name() const{
      return get_frame(P::get_current_frame()).name;
    }
    template <class Base>
    std::string AvroSharedData<Base>::get_description() const {
      return get_file().description;
    }
    template <class Base>
    void AvroSharedData<Base>::set_description(std::string str) {
      access_file().description=str;
    }


    template <class Base>
    void AvroSharedData<Base>::flush() {
      if (!dirty_) return;
      RMF_INTERNAL_CHECK(!read_only_, "Trying to write read-only file");
      avro::DataFileWriter<RMF_internal::All>
        rd(P::get_file_path().c_str(), get_all_schema());
      rd.write(all_);
      dirty_=false;
    }
    template <class Base>
    void AvroSharedData<Base>::reload() {
      avro::DataFileReader<RMF_internal::All>
        rd(P::get_file_path().c_str(), get_all_schema());
      bool ok=rd.read(all_);
      if (!ok) {
        throw IOException("Can't read input file on reload");
      }

      initialize_categories();
      initialize_node_keys();
      dirty_=false;
    }
    template <class Base>
    void AvroSharedData<Base>::set_current_frame(int frame){
      SharedData::set_current_frame(frame);
      if (all_.file.number_of_frames < frame+1) {
        access_file().number_of_frames=frame+1;
      }
    }

    template <class Base>
    void AvroSharedData<Base>::initialize_categories() {
      for (std::map<std::string, std::vector<RMF_internal::Data > >::const_iterator
             it= all_.category.begin(); it != all_.category.end(); ++it) {
        get_category(it->first);
      }
    }

    template <class Base>
    void AvroSharedData<Base>::initialize_node_keys() {
      node_keys_.resize(all_.nodes.size());
      for (unsigned int i=0; i< node_keys_.size(); ++i) {
        std::ostringstream oss;
        oss << i;
        node_keys_[i]=oss.str();
      }
    }

  } // namespace internal
} /* namespace RMF */
#endif /* RMF__INTERNAL_AVRO_SHARED_DATA_IMPL_H */
