/**
 *  \file compatibility/hash.h
 *  \brief Make sure that we avoid errors in specialization of boost hash
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#ifndef RMF_INTERNAL_AVRO_SCHEMAS_H
#define RMF_INTERNAL_AVRO_SCHEMAS_H

#include <RMF/config.h>
#include "backend/avro/AvroCpp/api/ValidSchema.hh"

RMF_ENABLE_WARNINGS namespace RMF {
  namespace avro_backend {
  RMFEXPORT rmf_avro::ValidSchema get_All_schema();
  RMFEXPORT rmf_avro::ValidSchema get_File_schema();
  RMFEXPORT rmf_avro::ValidSchema get_Nodes_schema();
  RMFEXPORT rmf_avro::ValidSchema get_Data_schema();
  RMFEXPORT rmf_avro::ValidSchema get_Frame_schema();
  }
}

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_UTILITY_H */
