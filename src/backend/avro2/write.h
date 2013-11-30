
#ifndef RMF_AVRO2_WRITE_H
#define RMF_AVRO2_WRITE_H

#include <avrocpp/api/DataFile.hh>

#include "RMF/config.h"
#include "types.h"

namespace RMF {
namespace avro2 {
struct FileDataChanges;
struct Frame;
}  // namespace avro2
}  // namespace RMF
namespace internal_avro {
class DataFileWriterBase;
}  // namespace internal_avro

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
