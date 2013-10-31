/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "avro_schemas.h"
#include "FrameJSON.h"
#include <backend/avro/All.json.h>
#include <backend/avro/File.json.h>
#include <backend/avro/Data.json.h>
#include <backend/avro/Nodes.json.h>
#include <backend/avro/Frame.json.h>
#include <backend/avro/AvroCpp/api/Compiler.hh>

RMF_ENABLE_WARNINGS namespace RMF {
  namespace avro_backend {
#define RMF_SCHEMA(name)                                                 \
  ::rmf_avro::ValidSchema get_##name##_schema() {                        \
    return ::rmf_avro::compileJsonSchemaFromString(name##_json.c_str()); \
  }

  RMF_SCHEMA(All);
  RMF_SCHEMA(File);
  RMF_SCHEMA(Nodes);
  RMF_SCHEMA(Data);
  RMF_SCHEMA(Frame);

  }  // namespace avro_backend
}    /* namespace RMF */

RMF_DISABLE_WARNINGS
