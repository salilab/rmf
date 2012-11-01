/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/utility.h>
#include <RMF/decorators.h>
#include <stdexcept>

namespace RMF {
  namespace internal {

    NodeHandle add_child_alias( AliasFactory af,
                                NodeHandle parent,
                                NodeConstHandle child) {
      NodeHandle nh=parent.add_child(child.get_name() + " alias",
                                 ALIAS);
      af.get(nh).set_aliased(child);
      return nh;
    }
  } // namespace internal
} /* namespace RMF */
