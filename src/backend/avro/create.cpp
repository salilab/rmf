/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "create.h"
#include "AvroSharedData.h"
#include "SingleAvroFile.h"
#include "MultipleAvroFileWriter.h"
#include "MultipleAvroFileReader.h"
#include <boost/algorithm/string/predicate.hpp>


namespace RMF {
namespace avro_backend {

  typedef avro_backend::AvroSharedData<avro_backend::SingleAvroFile> SingleAvroShareData;
  typedef avro_backend::AvroSharedData<avro_backend::MultipleAvroFileWriter> AvroWriterShareData;
  typedef avro_backend::AvroSharedData<avro_backend::MultipleAvroFileReader> AvroReaderShareData;


  internal::SharedData* create_shared_data(std::string path, bool create,
                                      bool read_only) {
    if (boost::algorithm::ends_with(path, ".rmf2")) {
      if (create) {
        return new AvroWriterShareData(path, create, read_only);
      } else if (read_only) {
        return new AvroReaderShareData(path, create, read_only);
      } else {
        RMF_THROW(Message("rmf2 files can only be created for writing or opened read-only"),
                  IOException);
      }
    } else if (boost::algorithm::ends_with(path, ".rmfa")) {
      return new SingleAvroShareData(path, create, read_only);
    } else {
      return NULL;
    }

  }
  internal::SharedData* create_shared_data_buffer(std::string &buffer,
                                             bool create) {
    return new SingleAvroShareData(buffer, create);
  }
  internal::SharedData* create_shared_data_buffer(const std::string &buffer) {
    return new SingleAvroShareData(buffer);
  }

}   // namespace avro_backend
} /* namespace RMF */
