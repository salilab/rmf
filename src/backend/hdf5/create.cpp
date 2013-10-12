/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "create.h"
#include "HDF5SharedData.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/unordered_map.hpp>
#include <backend/ImplementSharedData.h>
#include <boost/make_shared.hpp>
#include <RMF/log.h>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace hdf5_backend {

typedef internal::ImplementSharedData<HDF5SharedData> MySharedData;
boost::shared_ptr<internal::SharedData> create_shared_data(std::string path,
                                                           bool create,
                                                           bool read_only) {
  if (!boost::algorithm::ends_with(path, ".rmf")) {
    return boost::shared_ptr<internal::SharedData>();
  }
  RMF_INFO(get_logger(), "Using HDF5 hdf5_backend");
  return boost::make_shared<MySharedData>(path, create, read_only);
}

boost::shared_ptr<internal::SharedData> create_shared_data_buffer(
    std::string& /*buffer*/, bool /*create*/) {
  return boost::shared_ptr<internal::SharedData>();
}

boost::shared_ptr<internal::SharedData> create_shared_data_buffer(
    const std::string& /*buffer*/) {
  return boost::shared_ptr<internal::SharedData>();
}

}  // namespace avro_backend
} /* namespace RMF */

RMF_DISABLE_WARNINGS
