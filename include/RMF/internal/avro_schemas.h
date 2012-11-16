/**
 *  \file compatibility/hash.h
 *  \brief Make sure that we avoid errors in specialization of boost hash
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef RMF_INTERNAL_AVRO_SCHEMAS_H
#define RMF_INTERNAL_AVRO_SCHEMAS_H

#include <RMF/config.h>
#include <avro/ValidSchema.hh>

namespace RMF {
  namespace internal {
    RMFEXPORT avro::ValidSchema get_all_schema();
  }
}


#endif  /* RMF__INTERNAL_UTILITY_H */
