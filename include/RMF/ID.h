/**
 *  \file RMF/ID.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_ID_H
#define RMF_ID_H

#include <RMF/config.h>
#include "infrastructure_macros.h"
#include "exceptions.h"
#include <limits>

RMF_ENABLE_WARNINGS

namespace RMF {

struct FrameTag {
  static std::string get_tag() { return "f"; }
};
struct NodeTag {
  static std::string get_tag() { return "n"; }
};
struct CategoryTag {
  static std::string get_tag() { return "c"; }
};

/** Each frame or node in the hierarchy (RMF::FrameHandle)
    associated data has an associated identifier that is unique with
    that %RMF file. These are stored using ID classes with appropriate tags.*/
template <class TagT>
class ID {
  int i_;
  int compare(const ID<TagT>& o) const {
    if (i_ < o.i_)
      return -1;
    else if (i_ > o.i_)
      return 1;
    else
      return 0;
  }
  std::string get_string() const {
    if (i_ == -1)
      return Tag::get_tag() + "NULL";
    else if (i_ == std::numeric_limits<int>::min())
      return Tag::get_tag() + "INV";
    else {
      std::ostringstream oss;
      oss << Tag::get_tag() << i_;
      return oss.str();
    }
  }

 public:
  typedef TagT Tag;
#if !defined(RMF_DOXGYGEN) && !defined(SWIG)
  class SpecialTag {};
  ID(int i, SpecialTag) : i_(i) {}
  int get_index_always() const {
    RMF_USAGE_CHECK(i_ != std::numeric_limits<int>::min(),
                    "get_index called on uninitialized ID");
    return i_;
  }
#endif
  explicit ID(unsigned int i) : i_(i) {
    RMF_USAGE_CHECK(static_cast<int>(i_) >= 0,
                    "Bad index passed on initialize");
  }
  ID() : i_(std::numeric_limits<int>::min()) {}
  unsigned int get_index() const {
    RMF_USAGE_CHECK(i_ != std::numeric_limits<int>::min(),
                    "get_index called on uninitialized ID");
    RMF_USAGE_CHECK(i_ >= 0, "get_index called on special ID.");
    return i_;
  }
  RMF_SHOWABLE(ID, get_string());
  RMF_COMPARISONS(ID);
  RMF_HASHABLE(ID, return i_);
#if !defined(RMF_DOXYGEN) && !defined(SWIG)
  ID operator++() {
    ++i_;
    return *this;
  }
#endif
};

typedef ID<NodeTag> NodeID;
typedef ID<FrameTag> FrameID;
typedef ID<CategoryTag> Category;

typedef std::vector<NodeID> NodeIDs;
typedef std::vector<FrameID> FrameIDs;
typedef std::vector<Category> Categories;

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_ID_H */
