/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_AVRO2_TRAITS_H
#define RMF_AVRO2_TRAITS_H

#include "RMF/compiler_macros.h"
#include "RMF/log.h"
#include "data_file.h"
#include "generated/embed_jsons.h"
#include "avrocpp/api/Compiler.hh"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {

namespace {
internal_avro::ValidSchema get_schema() {
  static internal_avro::ValidSchema s =
      internal_avro::compileJsonSchemaFromString(data_avro2::frame_json);
  return s;
}
}

struct FileWriterTraitsBase {
  boost::shared_ptr<internal_avro::DataFileWriterBase> writer_;
  std::string path_;
  FileWriterTraitsBase(std::string path) : path_(path) {}
  template <class T>
  void write(const T &t) {
    RMF_INFO("Writing to file");
    avro2::write(writer_.get(), t);
  }
  void flush() {
    if (writer_) {
      RMF_INFO("Flushing file");
      writer_->flush();
    }
  }
  void load_frame(const FileData &, FrameID, FrameID, Frame &frame) {
    frame = Frame();
  }
  void load_file_data(FileData &fd) { fd = FileData(); }
  ~FileWriterTraitsBase() { flush(); }
};

template <class Base>
struct ReaderTraits {
  Base base_file_data_, base_frame_;
  boost::shared_ptr<internal_avro::DataFileReader<Frame> > reader_;

  template <class T>
  ReaderTraits(T path)
      : base_file_data_(path), base_frame_(path) {}
  template <class T>
  void write(const T &) {}
  void load_frame(const FileData &file_data, FrameID old_frame, FrameID id,
                  Frame &frame) {
    RMF_INFO("Loading frame " << id);
    if (old_frame == FrameID() || id < old_frame) reader_.reset();

    RMF_INTERNAL_CHECK(file_data.frame_block_offsets.find(id) !=
                           file_data.frame_block_offsets.end(),
                       "No such frame found");

    int64_t offset = file_data.frame_block_offsets.find(id)->second;
    if (!reader_ || reader_->blockOffsetBytes() > offset) reader_.reset();

    if (!reader_) {
      RMF_INFO("Creating new reader");
      reader_ = base_frame_.template get_reader<Frame>();
    }
    if (reader_->blockOffsetBytes() != offset) {
      RMF_INFO("Seeking to " << offset << " from "
                             << reader_->blockOffsetBytes());
      try {
        reader_->seekBlockBytes(offset);
      }
      catch (std::exception) {
        RMF_INFO("Seeking not supported");
      }
    }
    avro2::load_frame(id, *reader_, frame);
  }
  void load_file_data(FileData &fd) {
    RMF_INFO("Loading file data");
    boost::shared_ptr<internal_avro::DataFileReader<FileData> > reader =
        base_file_data_.template get_reader<FileData>();
    avro2::load_file_data(*reader, fd);
  }
  void flush() {}
};

template <bool GZIP>
struct FileWriterTraits : public FileWriterTraitsBase {
  FileWriterTraits(std::string path) : FileWriterTraitsBase(path) {
    writer_.reset(new internal_avro::DataFileWriterBase(
        path_.c_str(),
        internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json),
        16 * 1024, GZIP ? internal_avro::GZIP : internal_avro::NONE));
  }
};

struct FileReaderBase {
  std::string path_;

  FileReaderBase(std::string path) : path_(path) {}
  template <class T>
  boost::shared_ptr<internal_avro::DataFileReader<T> > get_reader() {
    return boost::make_shared<internal_avro::DataFileReader<T> >(path_.c_str(),
                                                                 get_schema());
  }
};

struct BufferWriterTraits {
  boost::shared_ptr<internal_avro::DataFileWriterBase> writer_;
  BufferHandle buffer_;
  boost::shared_ptr<internal_avro::OutputStream> stream_;
  BufferWriterTraits(BufferHandle buffer) : buffer_(buffer) {
    stream_ = internal_avro::memoryOutputStream();
    writer_.reset(new internal_avro::DataFileWriterBase(
        stream_,
        internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json),
        16 * 1024, internal_avro::GZIP));
  }
  template <class T>
  void write(const T &t) {
    RMF_INFO("Writing to buffer");
    avro2::write(writer_.get(), t);
  }
  void flush() {
    RMF_INFO("Flushing to buffer");
    // avoid rewriting later
    writer_->flush();
    buffer_.access_buffer().clear();
    boost::shared_ptr<internal_avro::InputStream> input_stream =
        internal_avro::memoryInputStream(*stream_);
    const uint8_t *data;
    size_t len;
    while (input_stream->next(&data, &len)) {
      buffer_.access_buffer().insert(buffer_.access_buffer().end(), data,
                                     data + len);
    }
  }
  void load_frame(const FileData &, FrameID, FrameID, Frame &fr) {
    fr = Frame();
  }
  void load_file_data(FileData &fd) { fd = FileData(); }
  ~BufferWriterTraits() { flush(); }
};

struct BufferReaderBase {
  BufferConstHandle buffer_;
  BufferReaderBase(BufferConstHandle buffer) : buffer_(buffer) {
    // to validate
    get_reader<Frame>();
  }
  template <class T>
  boost::shared_ptr<internal_avro::DataFileReader<T> > get_reader() {
    boost::shared_ptr<internal_avro::InputStream> stream =
        internal_avro::memoryInputStream(buffer_.get_uint8_t().first,
                                         buffer_.get_uint8_t().second);
    return boost::make_shared<internal_avro::DataFileReader<T> >(stream,
                                                                 get_schema());
  }
};
}
}

RMF_DISABLE_WARNINGS
#endif  // RMF_AVRO2_TRAITS_H
