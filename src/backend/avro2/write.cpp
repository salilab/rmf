
#include "DataFile.hh"
#include "RMF/compiler_macros.h"
#include "Specific.hh"
#include "types_encode_decode.h" // IWYU pragma: keep
#include "write.h"

namespace RMF {
namespace avro2 {
struct FileDataChanges;
struct Frame;
}  // namespace avro2
}  // namespace RMF

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
void write(internal_avro::DataFileWriterBase *writer, const Frame &fr) {
  writer->syncIfNeeded();
  internal_avro::encode(writer->encoder(), fr);
  writer->incr();
}
void write(internal_avro::DataFileWriterBase *writer,
           const FileDataChanges &fr) {
  writer->syncIfNeeded();
  internal_avro::encode(writer->encoder(), fr);
  writer->incr();
}
}
}
RMF_DISABLE_WARNINGS
