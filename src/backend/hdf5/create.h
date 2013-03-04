/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_HDF5_CREATE_H
#define RMF_HDF5_CREATE_H
#include <RMF/config.h>
#include <string>

RMF_COMPILER_ENABLE_WARNINGS

namespace RMF {
  namespace internal {
    class SharedData;
  }
namespace hdf5_backend {
  RMFEXPORT internal::SharedData* create_shared_data(std::string path, bool create,
                                                bool read_only);
  RMFEXPORT internal::SharedData* create_shared_data_buffer(std::string &buffer,
                                                       bool create);
  RMFEXPORT internal::SharedData* create_shared_data_buffer(const std::string &buffer);
}   // namespace avro_backend
} /* namespace RMF */

RMF_COMPILER_DISABLE_WARNINGS

#endif /* RMF_HDF5_CREATE_H */
