/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_FACTORIES_H
#define RMF_INTERNAL_SHARED_DATA_FACTORIES_H

#include <RMF/config.h>
#include "SharedData.h"
#include <vector>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace internal {

RMFEXPORT boost::shared_ptr<SharedData> create_file(const std::string& name);
RMFEXPORT boost::shared_ptr<SharedData> create_buffer();
RMFEXPORT boost::shared_ptr<SharedData> read_file(const std::string& name);
RMFEXPORT boost::shared_ptr<SharedData> open_buffer(
    const std::vector<char>& buffer);
RMFEXPORT boost::shared_ptr<SharedData> write_file(const std::string& name);

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_FACTORIES_H */
