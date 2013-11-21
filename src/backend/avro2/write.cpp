
#include "write.h"
#include "types_encode_decode.h"
#include <avrocpp/api/Encoder.hh>
#include <avrocpp/api/Compiler.hh>
#include <generated/embed_jsons.h>

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
