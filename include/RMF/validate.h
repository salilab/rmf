/**
 *  \file RMF/Validator.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_VALIDATOR_H
#define RMF_VALIDATOR_H

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include "FileHandle.h"
#include "RMF/FileConstHandle.h"
#include "RMF/NodeConstHandle.h"
#include "RMF/config.h"

RMF_ENABLE_WARNINGS

namespace RMF {
/** Validate the hierarchy. Throw an exception if it is not valid and log any
 * errors encountered. */
RMFEXPORT void validate(FileConstHandle fh);

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_VALIDATOR_H */
