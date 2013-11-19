
#ifndef SRC_BACKEND_AVRO2_FRAME_H_1753077812__H_
#define SRC_BACKEND_AVRO2_FRAME_H_1753077812__H_

#include <RMF/config.h>
#include "types.h"
#include <backend/AvroCpp/api/DataFile.hh>
#include <boost/scoped_ptr.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
// reuse reader later
RMFEXPORT Frame
    get_frame(const FileData &fd, std::string path, FrameID id,
              boost::scoped_ptr<rmf_avro::DataFileReader<Frame> > &reader);
}
}

RMF_DISABLE_WARNINGS
#endif
