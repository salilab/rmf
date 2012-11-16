/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/avro_schemas.h>
#include <RMF/internal/paths.h>
#include <RMF/decorators.h>
#include <avro/Compiler.hh>
#include <stdexcept>

namespace RMF {
  namespace internal {


    avro::ValidSchema get_all_schema() {
      std::string path = get_data_path("All.json");
      return avro::compileJsonSchemaFromFile(path.c_str());
    }

  } // namespace internal
} /* namespace RMF */
