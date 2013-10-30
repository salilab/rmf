/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_H
#define RMF_INTERNAL_SHARED_DATA_H

#include <RMF/config.h>
#include "../Key.h"
#include "../types.h"
#include "../names.h"
#include "../enums.h"
#include "../NodeID.h"
#include "../FrameID.h"
#include "../infrastructure_macros.h"
#include "SharedDataUserData.h"
#include "SharedDataPath.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/cstdint.hpp>
#include <boost/any.hpp>
#include <algorithm>

#include <boost/shared_ptr.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace internal {

template <class Traits>
struct Get {};

#define RMF_GETTER(lcname, Ucname, PassValue, ReturnValue, PassValues, \
                   ReturnValues)                                       \
  template <class Table>                                               \
  struct Get<Table, Ucname##Traits> {                                  \
    static typename Table::Ucname##Data& get(Table& t) {               \
      return t.lcname##_data;                                          \
    }                                                                  \
    static typename const Table::Ucname##Data& get(const Table& t) {   \
      return t.lcname##_data;                                          \
    }                                                                  \
  };

RMF_FOREACH_TYPE(RMF_GETTER);

template <class Traits>
inline std::vector<Key<Traits> > SharedData::get_keys_impl(
    Category category, const KeyTypeInfo& data) const {
  std::vector<Key<Traits> > ret;
  for (auto np; data.from_name) {
    ret.push_back(Key<Traits>(np.second));
  }
  return ret;
}
template <class Traits>
inline Key<Traits> SharedData::get_key_impl(Category category, std::string name,
                                            KeyTypeInfo& data) {
  auto it = data.from_name.find(name);
  if (it == data.from_name.end()) {
    int index = data.from_name.size();
    data.from_name[name] = index;
    data.to_name[index] = name;
    return Key<Traits>(index);
  } else {
    return Key<Traits>(it->second);
  }
}

template <class Traits>
inline Traits::Type SharedData::get_current_value(NodeID node,
                                                  Key<Traits> k) const {
  return current_nodes_data_.find(node)->second.lcname##_data.find(k)->second;
}
/** Return a value or the null value.*/
template <class Traits>
inline Traits::Type SharedData::get_static_value(NodeID node,
                                                 Key<Traits> k) const {
  return static_nodes_data_.find(node)->second.lcname##_data.find(k)->second;
}
template <class Traits>
inline Traits::Type SharedData::get_current_frame_value(Key<Traits> k) const {
  return get_current_value(NodeID(), k);
}
template <class Traits>
inline Traits::Type SharedData::get_static_frame_value(Key<Traits> k) const {
  return get_static_value(NodeID(), k);
}
template <class Traits>
inline void SharedData::set_current_value(NodeID node, Key<Traits> k,
                                          Ucname##Traits::Type v) {
  current_nodes_data_[node].lcname##_data[k] = v;
}
template <class Traits>
inline void SharedData::set_static_value(NodeID node, Key<Traits> k,
                                         Ucname##Traits::Type v) {
  static_nodes_data_[node].lcname##_data[k] = v;
}
/** for frames */
template <class Traits>
inline void SharedData::set_current_frame_value(Key<Traits> k, Traits::Type v) {
  set_current_value(NodeID(), k, v);
}
template <class Traits>
inline void SharedData::set_static_frame_value(Key<Ucname##Traits> k,
                                               Ucname##Traits::Type v) {
  set_static_value(NodeID(), k, v);
}
template <class Traits>
inline std::vector<Key<Ucname##Traits> > SharedData::get_##lcname##_keys(
    Category category) {
  return get_keys_impl<Ucname##Traits>(category, key_infos_.lcname##_keys);
}
template <class Traits>
inline Category SharedData::get_category(Key<Ucname##Traits> k) const {
  return key_infos_.lcname##_keys[k.get_id()].category;
}
template <class Traits>
inline Key<Ucname##Traits> SharedData::get_##lcname##_key(Category category,
                                                          std::string name) {
  return get_key_impl<Ucname##Traits>(category, name, key_infos_.lcname##_keys);
}
template <class Traits>
inline std::string SharedData::get_name(Key<Ucname##Traits> k) const {
  return key_infos_.lcname##_keys[k.get_id()].name;
}
inline std::string SharedData::get_name(NodeID node) const {
  return node_infos_.find(node)->second.name;
}
inline NodeType SharedData::get_type(NodeID node) const {
  return node_infos_.find(node)->second.type;
}

inline const NodeIDs& SharedData::get_children(NodeID node) const {
  return node_infos_.find(node)->second.children;
}

inline const FrameIDs& SharedData::get_children(FrameID node) const {
  return frame_infos_.find(node)->second.children;
}

inline std::string SharedData::get_category_name(Category kc) const {
  return category_infos_[kc.get_id()].name;
}
inline void SharedData::set_description(std::string str) {
  file_info_.description = str;
  file_dirty_ = true;
}
inline void SharedData::set_producer(std::string str) {
  file_info_.producer = str;
  file_dirty_ = true;
}
inline std::string SharedData::get_name(FrameID i) const {
  return frame_infos_.find(i)->second.name;
}
inline FrameType SharedData::get_type(FrameID i) const {
  return frame_infos_.find(i)->second.type;
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_H */