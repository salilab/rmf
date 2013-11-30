/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_AVRO2_FILE_DATA_H
#define RMF_AVRO2_FILE_DATA_H

#include "RMF/config.h"
#include "avrocpp/api/DataFile.hh"
#include "types.h"

namespace internal_avro {
template <typename T> class DataFileReader;
}  // namespace internal_avro

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
RMFEXPORT FileData
    get_file_data(internal_avro::DataFileReader<FileData> &reader);
}
}

RMF_DISABLE_WARNINGS
#endif  // RMF_AVRO2_FILE_DATA_H
