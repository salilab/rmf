/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_AVRO_KEYS_AND_CATEGORIES_H
#define RMF_INTERNAL_AVRO_KEYS_AND_CATEGORIES_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include <RMF/infrastructure_macros.h>
#include <RMF/constants.h>
#include <boost/unordered_map.hpp>
#include "AvroSharedData.types.h"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro_backend {

class AvroKeysAndCategories : public internal::SharedData {
  typedef boost::unordered_map<Category, std::string> CategoryNameMap;
  typedef boost::unordered_map<std::string, Category> NameCategoryMap;
  CategoryNameMap category_name_map_;
  NameCategoryMap name_category_map_;
  struct KeyData {
    std::string name;
    Category category;
  };
  typedef boost::unordered_map<unsigned int, KeyData> KeyDataMap;
  KeyDataMap key_data_map_;
  typedef boost::unordered_map<std::string, unsigned int> NameKeyInnerMap;
  typedef boost::unordered_map<Category, NameKeyInnerMap> NameKeyMap;
  NameKeyMap name_key_map_;

  std::vector<std::string> node_keys_;
  std::string frame_key_;
  std::string get_key_name(unsigned int id) const {
    return key_data_map_.find(id)->second.name;
  }
  Category get_category_impl(unsigned int id) const {
    return key_data_map_.find(id)->second.category;
  }

 public:
  template <class Traits>
  std::string get_name(Key<Traits> k) const {
    return get_key_name(k.get_id());
  }
  template <class Traits>
  Category get_category(Key<Traits> k) const {
    return get_category_impl(k.get_id());
  }
  template <class TypeTraits>
  Key<TypeTraits> get_key(Category category, std::string name) {
    typename NameKeyInnerMap::const_iterator it =
        name_key_map_[category].find(name);
    if (it == name_key_map_[category].end()) {
      unsigned int id = key_data_map_.size();
      key_data_map_[id].name = name;
      key_data_map_[id].category = category;
      name_key_map_[category][name] = id;
      RMF_INTERNAL_CHECK(
          get_key<TypeTraits>(category, name) == Key<TypeTraits>(id),
          "Keys don't match");
      return Key<TypeTraits>(id);
    } else {
      int id = it->second;
      RMF_INTERNAL_CHECK(name == it->first, "Odd names");
      return Key<TypeTraits>(id);
    }
  }

  const std::string& get_node_string(NodeID node) const {
    if (node == NodeID()) return frame_key_;
    return node_keys_[node.get_index()];
  }

  template <class TypeTraits>
  const std::string& get_key_string(Key<TypeTraits> k) const {
    RMF_INTERNAL_CHECK(k.get_id() >= 0, "Bad key");
    return key_data_map_.find(k.get_id())->second.name;
  }

  void clear_node_keys() { node_keys_.clear(); }

  void add_node_key() {
    std::ostringstream oss;
    oss << node_keys_.size();
    node_keys_.push_back(oss.str());
  }

  RMF_BACKEND_VIRTUAL std::string get_name(Category kc) const
      RMF_BACKEND_OVERRIDE {
    return category_name_map_.find(kc)->second;
  }

  RMF_BACKEND_VIRTUAL Categories get_categories() const RMF_BACKEND_OVERRIDE {
    Categories ret;
    for (CategoryNameMap::const_iterator it = category_name_map_.begin();
         it != category_name_map_.end(); ++it) {
      ret.push_back(it->first);
    }
    return ret;
  }
  RMF_BACKEND_VIRTUAL Category
  get_category(std::string name) RMF_BACKEND_OVERRIDE {
    NameCategoryMap::iterator it = name_category_map_.find(name);
    if (it == name_category_map_.end()) {
      unsigned int id = category_name_map_.size();
      Category ret(id);
      name_category_map_[name] = ret;
      category_name_map_[ret] = name;
      return ret;
    } else {
      return it->second;
    }
  }

  AvroKeysAndCategories(std::string path) : SharedData(path) {}
};

}  // namespace avro_backend
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_AVRO_KEYS_AND_CATEGORIES_H */
