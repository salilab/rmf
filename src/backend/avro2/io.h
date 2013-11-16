/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/config.h>
#include "types.h"
#include <RMF/internal/SharedData.h>
#include "backend/IO.h"
#include <RMF/infrastructure_macros.h>
#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {

struct Avro2IO : public backends::IO {
  std::string path_;
  avro2::FileData file_data_;
  bool file_data_dirty_;
  avro2::FileDataChanges file_data_changes_;
  boost::scoped_ptr<rmf_avro::DataFileWriterBase> writer_;
  boost::scoped_ptr<rmf_avro::DataFileReader<Frame> > reader_;
  avro2::Frame frame_;
  void commit();
  unsigned int get_number_of_frames() const;
 public:
  Avro2IO(std::string name);
  virtual void save_loaded_frame(const internal::SharedData *shared_data)
      RMF_OVERRIDE;
  virtual void load_loaded_frame(internal::SharedData *shared_data)
      RMF_OVERRIDE;
  virtual void save_static_frame(const internal::SharedData *shared_data)
      RMF_OVERRIDE;
  virtual void load_static_frame(internal::SharedData *shared_data)
      RMF_OVERRIDE;
  virtual void load_file(internal::SharedData *shared_data) RMF_OVERRIDE;
  virtual void save_file(const internal::SharedData *shared_data) RMF_OVERRIDE;
  virtual void load_hierarchy(internal::SharedData *shared_data) RMF_OVERRIDE;
  virtual void save_hierarchy(const internal::SharedData *shared_data)
      RMF_OVERRIDE;
  virtual void flush() RMF_OVERRIDE;
  virtual std::vector<char> get_buffer();
  virtual ~Avro2IO();
};

class Avro2IOFactory : public backends::IOFactory {
 public:
  virtual std::string get_file_extension() const;
  virtual boost::shared_ptr<backends::IO> read_file(const std::string &name)
      const;
  virtual boost::shared_ptr<backends::IO> create_buffer() const;
  virtual boost::shared_ptr<backends::IO> write_file(const std::string &name)
      const;
  virtual boost::shared_ptr<backends::IO> open_buffer(const std::vector<char> &)
      const;
  virtual boost::shared_ptr<backends::IO> create_file(const std::string &name)
      const;
  virtual ~Avro2IOFactory();
};

}  // namespace avro2
} /* namespace RMF */

RMF_DISABLE_WARNINGS
