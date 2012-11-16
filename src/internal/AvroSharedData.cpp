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

    AvroSharedData::AvroSharedData(std::string g, bool create):
      SharedData(g) {
      reload();
    }
    AvroSharedData::~AvroSharedData() {
      flush();
    }
    std::string AvroSharedData::get_name(unsigned int node) const {
      return all_.nodes.data[node].name;
    }
    unsigned int AvroSharedData::get_type(unsigned int node) const {
      std::string string_type= all_.nodes.data[node].type;
      unsigned int ret_type= boost::lexical_cast<NodeType>(string_type);
      return ret_type;
    }
    int AvroSharedData::add_child(int node, std::string name, int t){}
    void AvroSharedData::add_child(int node, int child_node){}
    Ints AvroSharedData::get_children(int node) const{}
    int AvroSharedData::add_category(std::string name){}
    Categories AvroSharedData::get_categories() const{}
    Category AvroSharedData::get_category(std::string name){}
    std::string AvroSharedData::get_category_name(Category kc) const  {
    }

    void AvroSharedData::set_frame_name(std::string str) {
      all_.frames.data[get_current_frame()].name=str;
     }
    std::string AvroSharedData::get_frame_name() const{}
    std::string AvroSharedData::get_description() const {}
    void AvroSharedData::set_description(std::string str) {}


    void AvroSharedData::flush() {
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
      dirty_=false;
    }
    void AvroSharedData::set_current_frame(int frame){
      SharedData::set_current_frame(frame);
    }

  } // namespace internal
} /* namespace RMF */
