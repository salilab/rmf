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
#include "../types.h"
#include "../infrastructure_macros.h"
#include <boost/unordered_map.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

template <class Traits>
class SharedDataFrameData {
 public:
  typedef boost::unordered_map<unsigned int, typename Traits::Type> Data;
  typedef boost::unordered_map<NodeID, Data> NodeDatas;
  NodeDatas node_datas_;
  SharedDataFrameData() {}
  void clear() { node_datas_.clear(); }
};

template <class Traits>
class SharedDataDataStorage {
 protected:
  typedef SharedDataFrameData<Traits> Data;
  Data ps_, pl_;
  bool static_dirty_;

 public:
  SharedDataDataStorage() : static_dirty_(false) {}
  void clear_static_values() { ps_.clear(); }
  void clear_loaded_values() { pl_.clear(); }
  bool get_static_is_dirty() const { return static_dirty_; }
  void set_static_is_dirty(bool tf) { static_dirty_ = tf; }
};

template <class Traits, class P = SharedDataDataStorage<Traits> >
class SharedDataData : public P {
  typename Traits::ReturnType get_value(const typename P::Data::NodeDatas& data,
                                        NodeID node, unsigned int k) const {
    typename P::Data::NodeDatas::const_iterator it0 = data.find(node);
    if (it0 == data.end()) return Traits::get_null_value();
    typename P::Data::Data::const_iterator it1 = it0->second.find(k);
    if (it1 == it0->second.end()) return Traits::get_null_value();
    return it1->second;
  }
  void set_value(typename P::Data::NodeDatas& data, NodeID node, unsigned int k,
                 typename Traits::ArgumentType v) {
    data[node][k] = v;
  }
  typename Traits::Type& access_value(typename P::Data::NodeDatas& data,
                                     NodeID node, unsigned int k) {
    return data[node][k];
  }
 public:
  typename Traits::ReturnType get_static_value(NodeID node,
                                               ID<Traits> k) const {
    return get_value(P::ps_.node_datas_, node, k.get_index());
  }
  typename Traits::ReturnType get_loaded_value(NodeID node,
                                               ID<Traits> k) const {
    return get_value(P::pl_.node_datas_, node, k.get_index());
  }
  void set_static_value(NodeID node, ID<Traits> k,
                        typename Traits::ArgumentType v) {
    set_value(P::ps_.node_datas_, node, k.get_index(), v);
    P::static_dirty_ = true;
  }
  void set_loaded_value(NodeID node, ID<Traits> k,
                        typename Traits::ArgumentType v) {
    set_value(P::pl_.node_datas_, node, k.get_index(), v);
  }
  typename Traits::Type& access_static_value(NodeID node, ID<Traits> k) {
    return access_value(P::ps_.node_datas_, node, k.get_index());
  }
  typename Traits::Type& access_loaded_value(NodeID node, ID<Traits> k) {
    return access_value(P::pl_.node_datas_, node, k.get_index());
  }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_DATA_H */
