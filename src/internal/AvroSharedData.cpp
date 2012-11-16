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
#include <algorithm>

namespace RMF {
  namespace internal {

    AvroSharedData::AvroSharedData(std::string g, bool create) {}
    AvroSharedData::~AvroSharedData() {}
    void AvroSharedDataflush() const{}
    int AvroSharedData::add_child(int node, std::string name, int t){}
    void AvroSharedData::add_child(int node, int child_node){}
    Ints AvroSharedData::get_children(int node) const{}
    int AvroSharedData::add_category(std::string name){}
    Categories AvroSharedData::get_categories() const{}
    Category AvroSharedData::get_category(std::string name){}
    std::string AvroSharedData::get_category_name(Category kc) const  {
      return category_data_map_.find(kc)->second.name;
    }

    void AvroSharedData::set_frame_name(std::string str) {
     }
    std::string AvroSharedData::get_frame_name() const{}

    void AvroSharedData::reload(){}
    void AvroSharedData::set_current_frame(int frame){}

  } // namespace internal
} /* namespace RMF */
