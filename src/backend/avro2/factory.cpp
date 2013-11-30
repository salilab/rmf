/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "RMF/config.h"
#include "factory.h"
#include "io.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

RMF_ENABLE_WARNINGS
namespace RMF {

namespace avro2 {

template <bool GZIP>
class Avro2IOFileFactory : public backends::IOFactory {
 public:
  virtual std::string get_file_extension() const RMF_OVERRIDE {
    if (GZIP)
      return ".rmfz";
    else
      return ".rmf3";
  }
  virtual boost::shared_ptr<backends::IO> read_file(const std::string &name)
      const RMF_OVERRIDE {
    return boost::make_shared<Avro2IO<ReaderTraits<FileReaderBase> > >(name);
  }
  virtual boost::shared_ptr<backends::IO> create_file(const std::string &name)
      const RMF_OVERRIDE {
    return boost::make_shared<Avro2IO<FileWriterTraits<GZIP> > >(name);
  }
};

class Avro2IOBufferFactory : public backends::IOFactory {
 public:
  virtual std::string get_file_extension() const RMF_OVERRIDE {
    return ".none";
  }
  virtual boost::shared_ptr<backends::IO> read_buffer(BufferConstHandle buffer)
      const RMF_OVERRIDE {
    return boost::make_shared<Avro2IO<ReaderTraits<BufferReaderBase> > >(
        buffer);
  }
  virtual boost::shared_ptr<backends::IO> create_buffer(BufferHandle buffer)
      const RMF_OVERRIDE {
    return boost::make_shared<Avro2IO<BufferWriterTraits> >(buffer);
  }
};

std::vector<boost::shared_ptr<backends::IOFactory> > get_factories() {
  std::vector<boost::shared_ptr<backends::IOFactory> > ret;
  ret.push_back(boost::make_shared<Avro2IOFileFactory<false> >());
  ret.push_back(boost::make_shared<Avro2IOFileFactory<true> >());
  ret.push_back(boost::make_shared<Avro2IOBufferFactory>());
  return ret;
}

}  // namespace
} /* namespace RMF */

RMF_DISABLE_WARNINGS
