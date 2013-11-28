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
void show_key_info(FileConstHandle rh, Category cat, std::string name,
                   std::ostream& out) {
  RMF_FOREACH(ID<Traits> k, rh.get_keys<Traits>(cat)) {
    int static_count = 0, frame_count = 0;
    RMF_FOREACH(NodeID n, rh.get_node_ids()) {
      NodeConstHandle nh = rh.get_node(n);
      if (!nh.get_frame_value(k).get_is_null()) {
        ++frame_count;
      } else if (!nh.get_static_value(k).get_is_null()) {
        ++static_count;
      }
    }
    out << "  " << rh.get_name(k) << ", " << name << ", " << frame_count << " ("
        << static_count << ")" << std::endl;
  }
}

namespace {
int count(NodeConstHandle nh, std::set<NodeConstHandle>& seen) {
  int ret = 0;
  if (seen.find(nh) == seen.end()) {
    ++ret;
    seen.insert(nh);
  } else {
    return 0;
  }
  NodeConstHandles ch = nh.get_children();
  RMF_FOREACH(NodeConstHandle c, ch) { ret += count(c, seen); }
  return ret;
}
}

#define RMF_SHOW_TYPE_DATA_INFO(Traits, UCName) \
  show_key_info<Traits>(rh, c, Traits::get_name(), out);

void show_info(FileConstHandle rh, std::ostream& out) {
  out << "Nodes: " << rh.get_number_of_nodes() << std::endl;
  out << "Frames: " << rh.get_number_of_frames() << std::endl;
  RMF_FOREACH(Category c, rh.get_categories()) {
    out << rh.get_name(c) << ":" << std::endl;
    RMF_FOREACH_TYPE(RMF_SHOW_TYPE_DATA_INFO);
  }
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
