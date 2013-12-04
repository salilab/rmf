/**
 *  \file RMF/operations.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_USE_H
#define RMF_INTERNAL_USE_H

// IWYU pragma: private

#include <boost/exception/all.hpp>  // IWYU pragma: export
#include <sstream>
#include <string>

#include "RMF/config.h"

RMF_ENABLE_WARNINGS
namespace RMF {

namespace internal {

template <class T>
void use(T) {}

}  // namespace internal
} /* namespace RMF */
RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_USE_H */
