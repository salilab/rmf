/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/enums.h>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <fstream>

RMF_ENABLE_WARNINGS

namespace RMF {

std::string get_frame_type_name(FrameType t) {
  switch (t) {
    case STATIC:
      return "static";
    case FRAME:
      return "frame";
    case MODEL:
      return "model";
    case CENTER:
      return "center";
    case FRAME_ALIAS:
      return "alias";
    default:
      return "unknown";
  }
}

std::ostream& operator<<(std::ostream& out, FrameType t) {
  out << get_frame_type_name(t);
  return out;
}
std::istream& operator>>(std::istream& in, FrameType& t) {
  std::string token;
  in >> token;
  for (FrameType i = STATIC; i <= FRAME_ALIAS; i = FrameType(i + 1)) {
    if (token == get_frame_type_name(i)) {
      t = i;
      return in;
    }
  }
  t = FRAME;
  return in;
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
