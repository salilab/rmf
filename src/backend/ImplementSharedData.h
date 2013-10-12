/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_IMPLEMENT_SHARED_DATA_H
#define RMF_IMPLEMENT_SHARED_DATA_H

#include <RMF/config.h>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace backend {

#define RMF_IMPLEMENT_SHARED_TYPE(lcname, Ucname, PassValue, ReturnValue,    \
                                  PassValues, ReturnValues)                  \
  RMF_BACKEND_VIRTUAL Ucname##Traits::Type get_current_value(                \
      NodeID node, Key<Ucname##Traits> k) const RMF_BACKEND_OVERRIDE {       \
    return Base::get_current_value(node, k);                                 \
  }                                                                          \
  RMF_BACKEND_VIRTUAL Ucname##Traits::Type get_static_value(                 \
      NodeID node, Key<Ucname##Traits> k) const RMF_BACKEND_OVERRIDE {       \
    return Base::get_static_value(node, k);                                  \
  }                                                                          \
  RMF_BACKEND_VIRTUAL void set_current_value(                                \
      NodeID node, Key<Ucname##Traits> k,                                    \
      Ucname##Traits::Type v) RMF_BACKEND_OVERRIDE {                         \
    return Base::set_current_value(node, k, v);                              \
  }                                                                          \
  RMF_BACKEND_VIRTUAL void set_static_value(                                 \
      NodeID node, Key<Ucname##Traits> k,                                    \
      Ucname##Traits::Type v) RMF_BACKEND_OVERRIDE {                         \
    return Base::set_static_value(node, k, v);                               \
  }                                                                          \
  RMF_BACKEND_VIRTUAL std::vector<Key<Ucname##Traits> > get_##lcname##_keys( \
      Category category) RMF_BACKEND_OVERRIDE {                              \
    return Base::template get_keys<Ucname##Traits>(category);                \
  }                                                                          \
  RMF_BACKEND_VIRTUAL Key<Ucname##Traits> get_##lcname##_key(                \
      Category category, std::string name) RMF_BACKEND_OVERRIDE {            \
    return Base::template get_key<Ucname##Traits>(category, name);           \
  }                                                                          \
  RMF_BACKEND_VIRTUAL std::string get_name(Key<Ucname##Traits> k) const      \
      RMF_BACKEND_OVERRIDE {                                                 \
    return Base::get_name(k);                                                \
  }                                                                          \
  RMF_BACKEND_VIRTUAL Category get_category(Key<Ucname##Traits> k) const     \
      RMF_BACKEND_OVERRIDE {                                                 \
    return Base::get_category(k);                                            \
  }

template <class Base>
class ImplementSharedData : public Base {
 public:
  ImplementSharedData(std::string path, bool a, bool b) : Base(path, a, b) {}
  ImplementSharedData(std::string &buffer, bool a) : Base(buffer, a) {}
  ImplementSharedData(const std::string &buffer) : Base(buffer) {}
  using Base::get_name;
  using Base::get_category;
  RMF_FOREACH_TYPE(RMF_IMPLEMENT_SHARED_TYPE);
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_IMPLEMENT_SHARED_DATA_H */
