/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_AVRO2_TRAITS_H
#define RMF_AVRO2_TRAITS_H

#include <RMF/compiler_macros.h>
#include "write.h"
#include "file_data.h"
#include "frame.h"
#include "generated/embed_jsons.h"
#include <avrocpp/api/Compiler.hh>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
struct FileWriterTraits {
  boost::shared_ptr<internal_avro::DataFileWriterBase> writer_;
  std::string path_;
  FileWriterTraits(std::string path) : path_(path) {
    writer_.reset(new internal_avro::DataFileWriterBase(
        path_.c_str(),
        internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json),
        16 * 1024));
  }
  template <class T>
  void write(const T &t) {
    avro2::write(writer_.get(), t);
  }
  void flush() { writer_->flush(); }
  Frame get_frame(const FileData &, FrameID, FrameID) { return Frame(); }
  FileData get_file_data() { return FileData(); }
};
struct FileReaderTraits {
  boost::shared_ptr<internal_avro::DataFileReader<Frame> > reader_;
  std::string path_;
  FileReaderTraits(std::string path) : path_(path) {}
  template <class T>
  void write(const T &) {}
  Frame get_frame(const FileData &file_data, FrameID old_frame, FrameID id) {
    if (old_frame == FrameID() || id < old_frame) {
      // kind of icky hack to handle being in the block
      reader_.reset();
    }
    return avro2::get_frame(file_data, path_, id, reader_);
  }
  FileData get_file_data() { return avro2::get_file_data(path_); }
  void flush() {}
};
}
}

RMF_DISABLE_WARNINGS
#endif  // RMF_AVRO2_TRAITS_H
