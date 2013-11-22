
#ifndef RMF_AVRO2_WRITE_H
#define RMF_AVRO2_WRITE_H

#include "types.h"
#include <avrocpp/api/DataFile.hh>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
RMFEXPORT void write(internal_avro::DataFileWriterBase *writer,
                     const Frame &fr);
RMFEXPORT void write(internal_avro::DataFileWriterBase *writer,
                     const FileDataChanges &fr);
}
}

RMF_DISABLE_WARNINGS
#endif  //  RMF_AVRO2_WRITE_H