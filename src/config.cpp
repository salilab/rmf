/**
 *  \file RMF/config.cpp
 *  \brief RMF module version information.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/config.h>

namespace RMF {

namespace internal {
   std::string rmf_data_path;
   std::string rmf_example_path;
}
std::string get_module_version() {
    return std::string("SVN");
}

}
