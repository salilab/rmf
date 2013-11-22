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
  explicit BufferConstHandle(std::string r)
      : data_(boost::make_shared<std::vector<char> >(r.begin(), r.end())) {}
  explicit BufferConstHandle(const std::vector<char> &r)
  : data_(boost::make_shared<std::vector<char> >(r.begin(), r.end())) {
  }
  explicit BufferConstHandle(const std::vector<uint8_t> &r)
      : data_(boost::make_shared<std::vector<char> >(r.begin(), r.end())) {}
  const std::vector<char> &get_buffer() const { return *data_; }
  //! get the buffer encoded in a string
  std::string get_string() const {
    return std::string(data_->begin(), data_->end());
  }
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
