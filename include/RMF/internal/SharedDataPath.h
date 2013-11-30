/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_PATH_H
#define RMF_INTERNAL_SHARED_DATA_PATH_H

#include <string>

#include "RMF/config.h"
#include "paths.h"

RMF_ENABLE_WARNINGS

namespace RMF {

namespace internal {

class SharedDataPath {
  std::string path_;

 public:
  SharedDataPath(std::string path) : path_(path) {}
  std::string get_file_path() const { return path_; }
  std::string get_file_name() const { return internal::get_file_name(path_); }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_PATH_H */
