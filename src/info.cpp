/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/info.h>
#include <RMF/NodeConstHandle.h>
#include <RMF/NodeHandle.h>
#include <RMF/FileConstHandle.h>
#include <RMF/FileHandle.h>
#include <boost/unordered_set.hpp>
#include <RMF/internal/utility.h>
#include <RMF/decorators.h>

RMF_ENABLE_WARNINGS

namespace RMF {

template <class Traits>
struct GetCount {
  static int get_count(FileConstHandle rh, Key<Traits> k) {
    return get_count(rh.get_root_node(), k);
  }
  static int get_count(NodeConstHandle rh, Key<Traits> k) {
    int cur = 0;
    if (rh.get_has_value(k)) ++cur;
    NodeConstHandles nhs = rh.get_children();
    for (unsigned int i = 0; i < nhs.size(); ++i) {
      cur += get_count(nhs[i], k);
    }
    return cur;
  }
};

template <class Traits>
void show_key_info(FileConstHandle rh, Category cat, std::string name,
                   std::ostream& out) {
  BOOST_FOREACH(Key<Traits> k, rh.get_keys<Traits>(cat)) {
    out << "    " << rh.get_name(k) ", " << name ", "
        << GetCount<Traits>::get_count(rh, k) << " uses" << std::endl;
  }
}

#define RMF_SHOW_TYPE_DATA_INFO(lcname, UCName, PassValue, ReturnValue, \
                                PassValues, ReturnValues)               \
  show_key_info<UCName##Traits>(rh, c, #lcname, out);

int count(NodeConstHandle nh, std::set<NodeConstHandle>& seen) {
  int ret = 0;
  if (seen.find(nh) == seen.end()) {
    ++ret;
    seen.insert(nh);
  } else {
    return 0;
  }
  NodeConstHandles ch = nh.get_children();
  BOOST_FOREACH(NodeConstHandle c, ch) { ret += count(c, seen); }
  return ret;
}

void show_info(FileConstHandle rh, std::ostream& out) {
  out << "Nodes: " << rh.get_number_of_nodes() << std::endl;
  out << "Frames: " << rh.get_number_of_frames() << std::endl;
  BOOST_FOREACH(Category c, rh.get_categories()) {
    out << rh.get_name(c) << ":" << std::endl;
    RMF_FOREACH_TYPE(RMF_SHOW_TYPE_DATA_INFO);
  }
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
