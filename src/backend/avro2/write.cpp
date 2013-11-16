
#include "write.h"
#include "types_encode_decode.h"
#include <backend/AvroCpp/api/Encoder.hh>
#include <backend/AvroCpp/api/Compiler.hh>
#include <generated/embed_jsons.h>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
void write(rmf_avro::DataFileWriterBase *writer, const Frame &fr) {
  writer->syncIfNeeded();
  rmf_avro::encode(writer->encoder(), fr);
  writer->incr();
}
void write(rmf_avro::DataFileWriterBase *writer, const FileDataChanges &fr) {
  writer->syncIfNeeded();
  rmf_avro::encode(writer->encoder(), fr);
  writer->incr();
}
}
}
RMF_DISABLE_WARNINGS
