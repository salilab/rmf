/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "AvroSharedData.h"
#include "SingleAvroFile.h"
#include "MultipleAvroFileWriter.h"
#include "MultipleAvroFileReader.h"
#include "../IO.h"
#include "../BackwardsIO.h"
#include <boost/make_shared.hpp>
#include <RMF/log.h>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro_backend {
namespace {

typedef backends::BackwardsIO<avro_backend::AvroSharedData<
    avro_backend::SingleAvroFile> > SingleAvroShareData;
typedef backends::BackwardsIO<avro_backend::AvroSharedData<
    avro_backend::MultipleAvroFileWriter> > AvroWriterShareData;
typedef backends::BackwardsIO<avro_backend::AvroSharedData<
    avro_backend::MultipleAvroFileReader> > AvroReaderShareData;

struct SingleTextAvroFactory : public RMF::backends::IOFactory {
  virtual std::string get_file_extension() const RMF_OVERRIDE {
    return ".rmft";
  }
  virtual boost::shared_ptr<RMF::backends::IO> read_file(
      const std::string& name) const RMF_OVERRIDE {
    return boost::make_shared<SingleAvroShareData>(name, false, true);
  }
  virtual boost::shared_ptr<RMF::backends::IO> create_file(
      const std::string& name) const RMF_OVERRIDE {
    return boost::make_shared<SingleAvroShareData>(name, true, false);
  }
  virtual ~SingleTextAvroFactory() {}
};

backends::IOFactoryRegistrar<SingleTextAvroFactory> registrart;

class SingleAvroFactory : public SingleTextAvroFactory {
  virtual std::string get_file_extension() const RMF_OVERRIDE {
    return ".rmfa";
  }
  virtual boost::shared_ptr<RMF::backends::IO> create_buffer(
      BufferHandle buffer) const RMF_OVERRIDE {
    return boost::make_shared<SingleAvroShareData>(buffer);
  }
  virtual boost::shared_ptr<RMF::backends::IO> read_buffer(
      BufferConstHandle buffer) const RMF_OVERRIDE {
    return boost::make_shared<SingleAvroShareData>(buffer);
  }
  virtual ~SingleAvroFactory() {}
};

backends::IOFactoryRegistrar<SingleAvroFactory> registrars;

class MultipleAvroFactory : public RMF::backends::IOFactory {
  virtual std::string get_file_extension() const RMF_OVERRIDE {
    return ".rmf2";
  }
  virtual boost::shared_ptr<RMF::backends::IO> read_file(
      const std::string& name) const RMF_OVERRIDE {
    return boost::make_shared<AvroReaderShareData>(name, false, true);
  }
  virtual boost::shared_ptr<RMF::backends::IO> create_file(
      const std::string& name) const RMF_OVERRIDE {
    return boost::make_shared<AvroWriterShareData>(name, true, false);
  }
  virtual ~MultipleAvroFactory() {}
};

backends::IOFactoryRegistrar<MultipleAvroFactory> registrar2;

}  // namespace
}  // namespace avro_backend
}  // namespace RMF

RMF_DISABLE_WARNINGS
