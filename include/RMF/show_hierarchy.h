/**
 *  \file RMF/show_hierarchy.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_NODE_SHOW_HIERARCHY_H
#define RMF_NODE_SHOW_HIERARCHY_H

#include <RMF/config.h>
#include <iostream>

namespace RMF {
  class NodeConstHandle;

/** Print out the hierarchy as an ascii tree.
 */
RMFEXPORT void show_hierarchy(NodeConstHandle root,
                              std::ostream& out = std::cout);

/** Print out the hierarchy as an ascii tree along with values
    as described by the frame parameters. If end_frame is -1,
    the only one frame is shown.
 */
RMFEXPORT void show_hierarchy_with_values(NodeConstHandle root,
                                          std::ostream& out = std::cout);

/** Print out the hierarchy as an ascii tree marking what decorators
    apply where.
 */
RMFEXPORT void show_hierarchy_with_decorators(NodeConstHandle root,
                                              bool verbose = false,
                                              std::ostream& out = std::cout);

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_NODE_SHOW_HIERARCHY_H */
