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

RMF_ENABLE_WARNINGS

namespace RMF {

class FrameTag {};
class NodeTag {};

/** Each frame or node in the hierarchy (RMF::FrameHandle)
    associated data has an associated identifier that is unique with
    that %RMF file. These are stored using ID classes with appropriate tags.*/
template <class Tag>
class ID {
  int i_;
  int compare(const ID<Tag>& o) const {
    if (i_ < o.i_)
      return -1;
    else if (i_ > o.i_)
      return 1;
    else
      return 0;
  }

 public:
  explicit ID(int i) : i_(i) {}
  ID() : i_(-2) {}
  int get_index() const {
    RMF_USAGE_CHECK(i_ != -2, "get_index called on uninitialized ID");
    return i_;
  }
  RMF_SHOWABLE(ID, i_);
  RMF_COMPARISONS(ID);
  RMF_HASHABLE(ID, return i_);
};

typedef ID<NodeTag> NodeID;
typedef ID<FrameTag> FrameID;

typedef std::vector<NodeID> NodeIDs;
typedef std::vector<FrameID> FrameIDs;

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_ID_H */
