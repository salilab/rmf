/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_DATA_H
#define RMF_INTERNAL_SHARED_DATA_DATA_H

#include "RMF/config.h"
#include "RMF/keys.h"
#include "RMF/types.h"
#include "RMF/infrastructure_macros.h"
#include "small_set_map.h"
#include "large_set_map.h"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

template <class Traits>
struct KeyData : public RMF_LARGE_UNORDERED_MAP<NodeID, typename Traits::Type> {
  typedef RMF_LARGE_UNORDERED_MAP<NodeID, typename Traits::Type> P;
  KeyData(const P& d) : P(d) {}
  KeyData() {}
};

template <class Traits>
struct TypeData : RMF_SMALL_UNORDERED_MAP<ID<Traits>, KeyData<Traits> > {
  typedef RMF_SMALL_UNORDERED_MAP<ID<Traits>, KeyData<Traits> > P;
  // Mac OS 10.8 and earlier clang needs this for some reason
  TypeData() {}
  TypeData(const TypeData& o) : P(o) {}
  TypeData<Traits> operator=(const TypeData& o) {
    P::operator=(o);
    return *this;
  }
};

template <class Traits>
class SharedDataData {
  TypeData<Traits> ps_, pl_;
  bool static_dirty_;

  typename Traits::ReturnType get_value(const TypeData<Traits>& data,
                                        NodeID node, ID<Traits> k) const {
    typename TypeData<Traits>::const_iterator it0 = data.find(k);
    if (it0 == data.end()) return Traits::get_null_value();
    typename KeyData<Traits>::const_iterator it1 = it0->second.find(node);
    if (it1 == it0->second.end()) return Traits::get_null_value();
    return it1->second;
  }
  void set_value(TypeData<Traits>& data, NodeID node, ID<Traits> k,
                 typename Traits::ArgumentType v) {
    data[k][node] = v;
  }
  void unset_value(TypeData<Traits>& data, NodeID node, ID<Traits> k) {
    data[k].erase(node);
  }
  typename Traits::Type& access_value(TypeData<Traits>& data, NodeID node,
                                      ID<Traits> k) {
    return data[k][node];
  }

 public:
  SharedDataData() : static_dirty_(false) {}
  void clear_static_values() { ps_.clear(); }
  void clear_loaded_values() { pl_.clear(); }
  bool get_static_is_dirty() const { return static_dirty_; }
  void set_static_is_dirty(bool tf) { static_dirty_ = tf; }
  const TypeData<Traits>& get_loaded_data(Traits) const { return pl_; }
  const TypeData<Traits>& get_static_data(Traits) const { return ps_; }
  TypeData<Traits>& access_loaded_data(Traits) { return pl_; }
  TypeData<Traits>& access_static_data(Traits) { return ps_; }

  typename Traits::ReturnType get_static_value(NodeID node,
                                               ID<Traits> k) const {
    return get_value(ps_, node, k);
  }
  typename Traits::ReturnType get_loaded_value(NodeID node,
                                               ID<Traits> k) const {
    return get_value(pl_, node, k);
  }
  void set_static_value(NodeID node, ID<Traits> k,
                        typename Traits::ArgumentType v) {
    set_value(ps_, node, k, v);
    static_dirty_ = true;
  }
  void set_loaded_value(NodeID node, ID<Traits> k,
                        typename Traits::ArgumentType v) {
    set_value(pl_, node, k, v);
  }
  void unset_static_value(NodeID node, ID<Traits> k) {
    unset_value(ps_, node, k);
    static_dirty_ = true;
  }
  void unset_loaded_value(NodeID node, ID<Traits> k) {
    unset_value(pl_, node, k);
  }
  typename Traits::Type& access_static_value(NodeID node, ID<Traits> k) {
    return access_value(ps_, node, k);
  }
  typename Traits::Type& access_loaded_value(NodeID node, ID<Traits> k) {
    return access_value(pl_, node, k);
  }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_DATA_H */
