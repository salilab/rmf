/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/compiler_warnings.h>
#include <RMF/names.h>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <fstream>

namespace RMF {
namespace internal {
extern std::string rmf_data_path;
extern std::string rmf_example_path;
}
namespace {
std::string get_concatenated_path(std::string part0,
                                  std::string part1) {
  return part0 + "/" + part1;
}

}

std::string get_as_node_name(std::string in) {
  boost::erase_all(in, "\"");
  return in;
}
} /* namespace RMF */
