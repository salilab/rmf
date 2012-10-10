/**
 *  \file RMF/Factory.h
 *  \brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPLIBRMF_FACTORY_H
#define IMPLIBRMF_FACTORY_H

#include <RMF/config.h>
namespace RMF {

/** Factories in RMF allow one to create Decorator objects as well
    as check whether a given node has the values needed for a particular
    decorator type in a given frame.
 */
template <class DecoratorType>
class Factory {
  /* It is important that this not save the FileHandle so as
     not to keep the file open.*/
 protected:
  Factory(){}
  template <class Key, class Node>
  bool get_has_value(Node n, const Key &key,
                     const Key &pf_key, int frame) const {
    if (frame >=0) {
      return n.get_has_value(pf_key, frame) || n.get_has_value(key);
    } else {
      return n.get_has_value(key);
    }
  }
  template <class Keys, class Node>
  bool get_has_values(Node n, const Keys &keys,
                      const Keys &pf_keys, int frame) const {
    if (frame >= 0) {
      return (!keys.empty() && n.get_has_value(keys[0]))
          || (!pf_keys.empty() && n.get_has_value(pf_keys[0], frame));
    } else {
      return !keys.empty() && n.get_has_value(keys[0]);
    }
  }
  template <class TypeTraits>
  Key<TypeTraits, 1>
  get_key(FileConstHandle fh, Category cat, const char* name,
          bool per_frame) const {
    if (fh.get_has_key<TypeTraits> (cat, name, per_frame)) {
      return fh.get_key<TypeTraits>(cat, name, per_frame);
    } else {
      return Key<TypeTraits, 1>();
    }
  }
  template <class TypeTraits>
  vector<Key<TypeTraits, 1> >
  get_keys(FileConstHandle fh, Category cat, const Strings &names,
           bool per_frame) const {
    if (fh.get_has_key<TypeTraits> (cat, names[0], per_frame)) {
      return fh.get_keys<TypeTraits>(cat, names, per_frame);
    } else {
      return vector<Key<TypeTraits, 1> >();
    }
  }
 public:
  typedef DecoratorType Decorator;
  IMP_RMF_SHOWABLE(Factory,
                   Decorator::get_decorator_type_name()
                   << "Factory");
};

} /* namespace RMF */

#endif /* IMPLIBRMF_FACTORY_H */