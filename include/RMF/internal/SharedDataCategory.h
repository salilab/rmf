/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_CATEGORY_H
#define RMF_INTERNAL_SHARED_DATA_CATEGORY_H

#include <RMF/config.h>
#include "../Category.h"
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

class SharedDataCategory {
  typedef boost::unordered_map<std::string, Category> From;
  From from_name_;
  typedef boost::unordered_map<Category, std::string> To;
  To to_name_;

 public:
  SharedDataCategory() {}

  Categories get_categories() const {
    Categories ret;
    BOOST_FOREACH(const From::value_type & it, from_name_) {
      ret.push_back(it.second);
    }
    return ret;
  }

  Category add_category(std::string name) {
    Category ret(from_name_.size());
    from_name_[name] = ret;
    to_name_[ret] = name;
    return ret;
  }

  Category get_category(std::string name) const {
    From::const_iterator it = from_name_.find(name);
    if (it == from_name_.end()) {
      return Category();
    } else {
      return it->second;
    }
  }

  std::string get_name(Category kc) const { return to_name_.find(kc)->second; }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_CATEGORY_H */