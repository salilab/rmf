/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_DATA_H
#define RMF_INTERNAL_SHARED_DATA_DATA_H

#include <RMF/config.h>
#include "../Key.h"
#include "../infrastructure_macros.h"
#include <boost/unordered_map.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

template <class Traits>
class SharedDataFrameData {
  typedef boost::unordered_map<Key<Traits>, typename Traits::Type> Data;
  typedef boost::unordered_map<NodeID, Data> NodeDatas;
  NodeDatas node_datas_;

 public:
  SharedDataFrameData() {}
  typename Traits::ReturnType get_value(NodeID node, Key<Traits> k) const {
    typename NodeDatas::const_iterator it0 = node_datas_.find(node);
    if (it0 == node_datas_.end()) return Traits::get_null_value();
    typename Data::const_iterator it1 = it0->second.find(k);
    if (it1 == it0->second.end()) return Traits::get_null_value();
    return it1->second;
  }
  void set_value(NodeID node, Key<Traits> k, typename Traits::ArgumentType v) {
    node_datas_[node][k] = v;
  }
  void clear() { node_datas_.clear(); }
};

template <class Traits>
class SharedDataData {
  typedef SharedDataFrameData<Traits> Data;
  Data ps_, pl_;
  bool static_dirty_;

 public:
  SharedDataData() : static_dirty_(false) {}
  typename Traits::ReturnType get_static_value(NodeID node,
                                               Key<Traits> k) const {
    return ps_.get_value(node, k);
  }
  typename Traits::ReturnType get_loaded_value(NodeID node,
                                               Key<Traits> k) const {
    return pl_.get_value(node, k);
  }
  void set_static_value(NodeID node, Key<Traits> k,
                        typename Traits::ArgumentType v) {
    ps_.set_value(node, k, v);
    static_dirty_ = true;
  }
  void set_loaded_value(NodeID node, Key<Traits> k,
                        typename Traits::ArgumentType v) {
    pl_.set_value(node, k, v);
  }
  void clear_static_values() { ps_.clear(); }
  void clear_loaded_values() { pl_.clear(); }
  bool get_static_is_dirty() const { return static_dirty_; }
  void set_static_is_dirty(bool tf) { static_dirty_ = tf; }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_DATA_H */
