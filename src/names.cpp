/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/names.h>
#include <boost/algorithm/string.hpp>

namespace RMF {
  std::string get_as_node_name(std::string in) {
    boost::erase_all(in, "\"");
    return in;
  }
} /* namespace RMF */
