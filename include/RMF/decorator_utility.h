/**
 *  \file RMF/decorator_utility.h
 *  \brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_DECORATOR_UTILITY_H
#define RMF_DECORATOR_UTILITY_H

#include <RMF/config.h>
#include "NodeConstHandle.h"
#include <RMF/decorators.h>
namespace RMF {

/** Return a list of Particle NodeHandles that forms a slice through the
    tree and whose radii are as close as possible to the passed resolution.
 */
RMFEXPORT NodeConstHandles get_particles_by_resolution(NodeConstHandle h,
                                                       double          resolution);

} /* namespace RMF */

#endif /* RMF_DECORATOR_UTILITY_H */
