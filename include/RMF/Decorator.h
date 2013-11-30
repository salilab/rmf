/**
 *  \file RMF/Decorator.h
 *  \brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_DECORATOR_H
#define RMF_DECORATOR_H

#include "RMF/config.h"
#include "infrastructure_macros.h"

RMF_ENABLE_WARNINGS namespace RMF {

  /** Decorators in RMF provide high level routines to manipulate attributes
      of nodes in the hierarchy. They are created by an associated Factory.
   */
  template <class HandleType>
  class Decorator {
    HandleType handle_;

   protected:
    Decorator(HandleType handle) : handle_(handle) {};
    // TypeTraits::Values
    template <class KeyT>
    typename KeyT::Tag::Type get_value(const KeyT& key) const {
      return get_node().get_value(key);
    }
    template <class KeyT>
    typename KeyT::Tag::Type get_static_value(const KeyT& key) const {
      return get_node().get_static_value(key);
    }
    template <class KeyT>
    typename KeyT::Tag::Type get_frame_value(const KeyT& key) const {
      return get_node().get_frame_value(key);
    }
    template <class Key, class Value>
    void set_value(Key& key, const Value& v) {
      get_node().set_value(key, v);
    }
    template <class Key, class Value>
    void set_static_value(Key& key, const Value& v) {
      get_node().set_static_value(key, v);
    }
    template <class Key, class Value>
    void set_frame_value(Key& key, const Value& v) {
      get_node().set_frame_value(key, v);
    }

   public:
    typedef HandleType Node;
    Node get_node() const { return handle_; }
    RMF_SHOWABLE(Decorator, get_node().get_name());
  };

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_DECORATOR_H */
