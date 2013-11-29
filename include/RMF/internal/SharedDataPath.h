/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_PATH_H
#define RMF_INTERNAL_SHARED_DATA_PATH_H

#include <RMF/config.h>
#include "RMF/types.h"
#include "RMF/names.h"
#include "RMF/enums.h"
#include "RMF/ID.h"
#include "RMF/infrastructure_macros.h"
#include "SharedDataUserData.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/cstdint.hpp>
#include <boost/any.hpp>
#include <algorithm>

#include <boost/shared_ptr.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace internal {

class SharedDataPath {
  std::string path_;

 public:
  SharedDataPath(std::string path);
  std::string get_file_path() const { return path_; }
  std::string get_file_name() const;
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_PATH_H */
