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
#include "backend/ImplementSharedData.h"
#include <boost/algorithm/string/predicate.hpp>
#include <RMF/log.h>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro_backend {

typedef backend::ImplementSharedData<avro_backend::AvroSharedData<
    avro_backend::SingleAvroFile> > SingleAvroShareData;
typedef backend::ImplementSharedData<avro_backend::AvroSharedData<
    avro_backend::MultipleAvroFileWriter> > AvroWriterShareData;
typedef backend::ImplementSharedData<avro_backend::AvroSharedData<
    avro_backend::MultipleAvroFileReader> > AvroReaderShareData;

boost::shared_ptr<internal::SharedData> create_shared_data(std::string path,
                                                           bool create,
                                                           bool read_only) {
  if (boost::algorithm::ends_with(path, ".rmf2")) {
    if (create) {
      RMF_INFO(get_avro_logger(), "Using RMF2 writer backend");
      return boost::make_shared<AvroWriterShareData>(path, create, read_only);
    } else if (read_only) {
      RMF_INFO(get_avro_logger(), "Using RMF2 reader backend");
      return boost::make_shared<AvroReaderShareData>(path, create, read_only);
    } else {
      RMF_THROW(Message(
                    "rmf2 files can only be created for writing or "
                    "opened read-only"),
                IOException);
    }
  } else if (boost::algorithm::ends_with(path, ".rmft") ||
             boost::algorithm::ends_with(path, ".rmfa")) {
    RMF_INFO(get_avro_logger(), "Using RMFA/T backend");
    return boost::make_shared<SingleAvroShareData>(path, create, read_only);
  } else {
    return boost::shared_ptr<SingleAvroShareData>();
  }
}
boost::shared_ptr<internal::SharedData> create_shared_data_buffer(
    std::string& buffer, bool create) {
  RMF_INFO(get_avro_logger(),
           (create ? "Created rmf in buffer" : "Opened rmf from buffer"));
  return boost::make_shared<SingleAvroShareData>(boost::ref(buffer), create);
}
boost::shared_ptr<internal::SharedData> create_shared_data_buffer(
    const std::string& buffer) {
  RMF_INFO(get_avro_logger(), "Opened rmf from buffer, read-only");
  return boost::make_shared<SingleAvroShareData>(buffer);
}

}  // namespace avro_backend
} /* namespace RMF */

RMF_DISABLE_WARNINGS
