/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/shared_data_factories.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "backend/IO.h"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

boost::shared_ptr<SharedData> create_file(const std::string& name) {
  boost::shared_ptr<backends::IO> io = backends::create_file(name);
  if (!io) {
    RMF_THROW(Message("Can't create file") << File(name), IOException);
  }
  return boost::make_shared<SharedData>(io, name, true, true);
}
boost::shared_ptr<SharedData> create_buffer() {
  boost::shared_ptr<backends::IO> io = backends::create_buffer();
  if (!io) {
    RMF_THROW(Message("Can't create buffer"), IOException);
  }
  return boost::make_shared<SharedData>(io, "buffer", true, true);
}
boost::shared_ptr<SharedData> read_file(const std::string& name) {
  boost::shared_ptr<backends::IO> io = backends::read_file(name);
  if (!io) {
    RMF_THROW(Message("Can't read file") << File(name), IOException);
  }
  boost::shared_ptr<SharedData> ret =
      boost::make_shared<SharedData>(io, name, false, false);
  return ret;
}
boost::shared_ptr<SharedData> open_buffer(const std::vector<char>& buffer) {
  boost::shared_ptr<backends::IO> io = backends::open_buffer(buffer);
  if (!io) {
    RMF_THROW(Message("Can't create buffer"), IOException);
  }
  boost::shared_ptr<SharedData> ret =
      boost::make_shared<SharedData>(io, "buffer", false, false);
  return ret;
}
boost::shared_ptr<SharedData> write_file(const std::string& name) {
  boost::shared_ptr<backends::IO> io = backends::write_file(name);
  if (!io) {
    RMF_THROW(Message("Can't write file") << File(name), IOException);
  }
  boost::shared_ptr<SharedData> ret =
      boost::make_shared<SharedData>(io, name, true, false);
  return ret;
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS
