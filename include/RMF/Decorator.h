/**
 *  \file RMF/Decorator.h
 *  \brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF__DECORATOR_H
#define RMF__DECORATOR_H

#include <RMF/config.h>
namespace RMF {

/** Decorators in RMF provide high level routines to manipulate attributes
    of nodes in the hierarchy. They are created by an associated Factory.
 */
template <class DecoratorType, class HandleType>
class Decorator {
  HandleType handle_;
 protected:
  Decorator(HandleType handle): handle_(handle) {};
  template <class Keys>
  typename Keys::value_type::TypeTraits::Types
  get_values(const Keys &keys,
             const Keys &pf_keys) const {
    if (!keys.empty() && get_node().get_has_value(keys[0])) {
      return get_node().get_values(keys);
    } else {
      return get_node().get_values(pf_keys);
    }
  }
  //TypeTraits::Values
  template <class KeyT>
  typename KeyT::TypeTraits::Type get_value(const KeyT &key,
                                             const KeyT &pf_key) const {
    if (get_node().get_has_value(key)) {
      return get_node().get_value(key);
    } else {
      return get_node().get_value(pf_key);
    }
  }
  //TypeTraits::Values
  template <class KeyT>
  typename KeyT::TypeTraits::Types get_all_values(const KeyT &pf_key) const {
    return get_node().get_all_values(pf_key);
  }
  template <class Keys, class Values>
  void set_values(Keys &keys, Keys &pf_keys, const Values&v) {
    if (get_frame()>=0) {
      get_node().set_values(pf_keys, v);
    } else {
      get_node().set_values(keys, v);
    }
  }
  template <class Key, class Value>
  void set_value(Key &key, Key &pf_key, const Value&v) {
    if (get_frame()>=0) {
      get_node().set_value(pf_key, v);
    } else {
      get_node().set_value(key, v);
    }
  }
 public:
  typedef HandleType Node;
  int get_frame() const {
    return get_node().get_file().get_current_frame();
  }
  Node get_node() const {
    return handle_;
  }
  RMF_SHOWABLE(Decorator,
                   DecoratorType::get_decorator_type_name()
                   << get_node().get_name());
};

} /* namespace RMF */

#endif /* RMF__DECORATOR_H */
