/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/AvroSharedData.h>
#include <RMF/NodeHandle.h>
#include <RMF/Validator.h>
#include <RMF/internal/set.h>
#include <RMF/internal/avro_schemas.h>
#include <boost/lexical_cast.hpp>
#include <avro/DataFile.hh>
#include <algorithm>

namespace RMF {
  namespace internal {

    AvroSharedData::AvroSharedData(std::string g, bool create,
                                   bool read_only):
      SharedData(g), read_only_(read_only) {
      if (!create) {
        reload();
      } else {
        initialize_categories();
        initialize_node_keys();
        all_.file.number_of_frames=0;
        // write to disk
        add_child(-1, "root", ROOT);
        flush();
      }
    }
    AvroSharedData::~AvroSharedData() {
      flush();
    }
    std::string AvroSharedData::get_name(unsigned int node) const {
      return all_.nodes[node].name;
    }
    unsigned int AvroSharedData::get_type(unsigned int node) const {
      std::string string_type= all_.nodes[node].type;
      unsigned int ret_type= boost::lexical_cast<NodeType>(string_type);
      return ret_type;
    }
    unsigned int AvroSharedData::get_number_of_frames() const {
      return all_.file.number_of_frames;
    }
    int AvroSharedData::add_child(int node, std::string name, int t){
      int index= all_.nodes.size();
      all_.nodes.push_back(RMF_internal::Node());
      all_.nodes.back().name=name;
      all_.nodes.back().type= boost::lexical_cast<std::string>(NodeType(t));
      all_.nodes.back().index=index;
      if (node >=0) {
        add_child(node, index);
      }
      std::ostringstream oss;
      oss << index;
      node_keys_.push_back(oss.str());
      dirty_=true;
      RMF_INTERNAL_CHECK(get_type(index) ==t,
                         "Types don't match");
      return index;
    }
    void AvroSharedData::add_child(int node, int child_node){
      all_.nodes[node].children.push_back(child_node);
      dirty_=true;
    }
    Ints AvroSharedData::get_children(int node) const{
      return Ints(all_.nodes[node].children.begin(),
                  all_.nodes[node].children.end());
    }
    Categories AvroSharedData::get_categories() const {
      Categories ret;
      for (CategoryNameMap::const_iterator
             it= category_name_map_.begin(); it != category_name_map_.end();
           ++it) {
        ret.push_back(it->first);
      }
      return ret;
    }
    Category AvroSharedData::get_category(std::string name){
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
    std::string AvroSharedData::get_category_name(Category kc) const  {
      return category_name_map_.find(kc)->second;
    }
    void AvroSharedData::set_frame_name(std::string str) {
      // offset for static data
      all_.frames[get_current_frame()+1].name=str;
      dirty_=true;
     }
    std::string AvroSharedData::get_frame_name() const{
      return all_.frames[get_current_frame()+1].name;
    }
    std::string AvroSharedData::get_description() const {
      return all_.file.description;
    }    void AvroSharedData::set_description(std::string str) {
      all_.file.description=str;
      dirty_=true;
    }


    void AvroSharedData::flush() {
      if (read_only_) return;
      avro::DataFileWriter<RMF_internal::All>
        rd(get_file_name().c_str(), get_all_schema());
      rd.write(all_);
      dirty_=false;
    }
    void AvroSharedData::reload() {
      avro::DataFileReader<RMF_internal::All>
        rd(get_file_name().c_str(), get_all_schema());
      bool ok=rd.read(all_);
      if (!ok) {
        throw IOException("Can't read input file on reload");
      }

      initialize_categories();
      initialize_node_keys();
      dirty_=false;
    }
    void AvroSharedData::set_current_frame(int frame){
      SharedData::set_current_frame(frame);
      all_.file.number_of_frames=std::max(all_.file.number_of_frames,
                                          frame+1);
      dirty_=true;
    }




    void AvroSharedData::initialize_categories() {
      for (std::map<std::string, std::vector<RMF_internal::Data > >::const_iterator
             it= all_.category.begin(); it != all_.category.end(); ++it) {
        get_category(it->first);
      }
    }

    void AvroSharedData::initialize_node_keys() {
      node_keys_.resize(all_.nodes.size());
      for (unsigned int i=0; i< node_keys_.size(); ++i) {
        std::ostringstream oss;
        oss << i;
        node_keys_[i]=oss.str();
      }
    }

  } // namespace internal
} /* namespace RMF */
