/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/paths.h>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <stdexcept>

namespace RMF {
  namespace internal {
    std::string get_relative_path(std::string base,
                                  std::string file) {
      // assume it already is
      return file;
    }
    std::string get_absolute_path(std::string base,
                                  std::string file) {
      return boost::filesystem::canonical(boost::filesystem::path(base)
                                          /boost::filesystem::path(file))
        .string();
    }
  } // namespace internal
} /* namespace RMF */
