/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/SharedDataPath.h>
#include <boost/filesystem/path.hpp>
#include <boost/version.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

SharedDataPath::SharedDataPath(std::string path) : path_(path) {}

std::string SharedDataPath::get_file_name() const {
#if BOOST_VERSION >= 104600
  return boost::filesystem::path(path_).filename().string();
#else
  return boost::filesystem::path(path_).filename();
#endif
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS
