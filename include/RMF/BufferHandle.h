/**
 *  \file RMF/BufferHandle.h
 *  \brief Manage a memory buffer for reading/writing RMF files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_BUFFER_HANDLE_H
#define RMF_BUFFER_HANDLE_H

#include <RMF/config.h>
#include "infrastructure_macros.h"
#include "exceptions.h"
#include "BufferConstHandle.h"
#include <limits>

RMF_ENABLE_WARNINGS

namespace RMF {
/** Manage a shared buffer for storing a RMF.*/
class BufferHandle : public BufferConstHandle {
 public:
  //! Empty buffer
  BufferHandle() : BufferConstHandle(std::vector<char>()) {}
#if !defined(IMP_DOXYGEN) && !defined(SWIG)
  std::vector<char> &access_buffer() const { return *data_; }
#endif
};

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_BUFFER_HANDLE_H */
