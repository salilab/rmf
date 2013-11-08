/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_KEYS_H
#define RMF_INTERNAL_SHARED_DATA_KEYS_H

#include <RMF/config.h>
#include "../types.h"
#include <boost/unordered_map.hpp>


RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

class SharedDataKeysBase {
 protected:
  boost::unordered_map<unsigned int, Category> key_categories_;
  boost::unordered_map<unsigned int, std::string> key_names_;

  SharedDataKeysBase() {}
};

template <class Traits, class P = SharedDataKeysBase>
class SharedDataKeys : public P {
  typedef boost::unordered_map<std::string, ID<Traits> > KeyInfo;
  typedef boost::unordered_map<Category, KeyInfo> CategoryKeys;
  CategoryKeys category_keys_;

 public:
  SharedDataKeys() {}

  Category get_category(ID<Traits> k) const {
    return P::key_categories_.find(k.get_index())->second;
  }

  ID<Traits> get_key(Category cat, std::string name, Traits) {
    typename KeyInfo::iterator it = category_keys_[cat].find(name);
    if (it == category_keys_[cat].end()) {
      int index = P::key_names_.size();
      P::key_names_[index] = name;
      category_keys_[cat][name] = ID<Traits>(index);
      P::key_categories_[index] = cat;
      return ID<Traits>(index);
    } else {
      return ID<Traits>(it->second);
    }
  }

  std::string get_name(ID<Traits> k) const {
    return P::key_names_.find(k.get_index())->second;
  }

  std::vector<ID<Traits> > get_keys(Category cat, Traits) const {
    if (category_keys_.find(cat) == category_keys_.end()) {
      return std::vector<ID<Traits> >();
    }
    std::vector<ID<Traits> > ret;
    ret.reserve(category_keys_.find(cat)->second.size());
    RMF_FOREACH(typename KeyInfo::value_type it,
                  category_keys_.find(cat)->second) {
      ret.push_back(it.second);
    }
    return ret;
  }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_KEYS_H */
