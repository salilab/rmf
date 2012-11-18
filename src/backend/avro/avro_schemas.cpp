/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include "avro_schemas.h"
#include <RMF/internal/paths.h>
#include <RMF/decorators.h>
#include <avro/Compiler.hh>
#include <stdexcept>

namespace RMF {
  namespace internal {
#define RMF_SCHEMA(name)                                        \
    avro::ValidSchema get_##name##_schema() {                   \
    std::string path = get_data_path(#name".json");             \
    return avro::compileJsonSchemaFromFile(path.c_str());       \
    }

    RMF_SCHEMA(All);
    RMF_SCHEMA(File);
    RMF_SCHEMA(Node);
    RMF_SCHEMA(Frame);
    RMF_SCHEMA(Data);


  } // namespace internal
} /* namespace RMF */
