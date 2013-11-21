/**
 *  \file RMF/BufferConstHandle.h
 *  \brief Manage a memory buffer for reading/writing RMF files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_BUFFER_CONST_HANDLE_H
#define RMF_BUFFER_CONST_HANDLE_H

#include <RMF/config.h>
#include "infrastructure_macros.h"
#include "exceptions.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <limits>

RMF_ENABLE_WARNINGS

namespace RMF {
/** Manage a shared buffer for storing a RMF.*/
class BufferConstHandle {
 protected:
  boost::shared_ptr<std::vector<char> > data_;
  int compare(BufferConstHandle o) const {
    if (&*data_ < &*o.data_)
      return -1;
    else if (&*data_ > &*o.data_)
      return 1;
    else
      return 0;
  }

 public:
  template <class R>
  explicit BufferConstHandle(const R &r)
      : data_(boost::make_shared<std::vector<char> >()) {
    data_->insert(data_->end(), r.begin(), r.end());
  }
  BufferConstHandle(const BufferConstHandle &o) : data_(o.data_) {}
  const std::vector<char> &get_buffer() const { return *data_; }
  RMF_COMPARISONS(BufferConstHandle);
  RMF_HASHABLE(BufferConstHandle, return reinterpret_cast<size_t>(&*data_););
  RMF_SHOWABLE(BufferConstHandle, "buffer");
#if !defined(IMP_DOXYGEN) && !defined(SWIG)
  boost::shared_ptr<std::vector<char> > get() const { return data_; }
#endif
};

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_BUFFER_CONST_HANDLE_H */
