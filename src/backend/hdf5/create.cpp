/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "HDF5SharedData.h"
#include "../IO.h"
#include "../BackwardsIO.h"
#include <boost/make_shared.hpp>
#include <RMF/log.h>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace hdf5_backend {
namespace {

typedef backends::BackwardsIO<HDF5SharedData> MIO;

class HDF5Factory : public RMF::backends::IOFactory {
  virtual std::string get_file_extension() const RMF_OVERRIDE { return ".rmf"; }
  virtual boost::shared_ptr<RMF::backends::IO> read_file(
      const std::string& name) const RMF_OVERRIDE {
    return boost::make_shared<MIO>(name, false, true);
  }
  virtual boost::shared_ptr<RMF::backends::IO> write_file(
      const std::string& name) const RMF_OVERRIDE {
    return boost::make_shared<MIO>(name, false, false);
  }
  virtual boost::shared_ptr<RMF::backends::IO> create_file(
      const std::string& name) const RMF_OVERRIDE {
    return boost::make_shared<MIO>(name, true, false);
  }
  virtual ~HDF5Factory() {}
};

backends::IOFactoryRegistrar<HDF5Factory> registrar;
}  // namespace
}  // namespace avro_backend
}  // namespace RMF

RMF_DISABLE_WARNINGS
