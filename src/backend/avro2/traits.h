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

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <avrocpp/api/Compiler.hh>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {

struct FileWriterTraitsBase {
  boost::shared_ptr<internal_avro::DataFileWriterBase> writer_;
  std::string path_;
  FileWriterTraitsBase(std::string path) : path_(path) {}
  template <class T>
  void write(const T &t) {
    avro2::write(writer_.get(), t);
  }
  void flush() { writer_->flush(); }
  Frame get_frame(const FileData &, FrameID, FrameID) { return Frame(); }
  FileData get_file_data() { return FileData(); }
  ~FileWriterTraitsBase() { flush(); }
};

struct FileWriterTraits : public FileWriterTraitsBase {
  FileWriterTraits(std::string path) : FileWriterTraitsBase(path) {
    writer_.reset(new internal_avro::DataFileWriterBase(
        path_.c_str(),
        internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json),
        16 * 1024));
  }
};

struct FileReaderTraits {
  boost::shared_ptr<internal_avro::DataFileReader<Frame> > reader_;
  std::string path_;

  FileReaderTraits(std::string path) : path_(path) {}
  template <class T>
  void write(const T &) {}
  Frame get_frame(const FileData &file_data, FrameID old_frame, FrameID id) {
    if (old_frame == FrameID() || id < old_frame) reader_.reset();

    RMF_INTERNAL_CHECK(file_data.frame_block_offsets.find(id) !=
                           file_data.frame_block_offsets.end(),
                       "No such frame found");

    int64_t offset = file_data.frame_block_offsets.find(id)->second;
    if (!reader_ || reader_->blockOffsetBytes() > offset) reader_.reset();

    if (!reader_) {
      RMF_INFO(get_logger(), "Creating new reader");
      reader_.reset(new internal_avro::DataFileReader<Frame>(
          path_.c_str(),
          internal_avro::compileJsonSchemaFromString(data_avro2::frame_json)));
    }
    if (reader_->blockOffsetBytes() != offset) {
      RMF_TRACE(get_logger(), "Seeking to " << offset << " from "
                                            << reader_->blockOffsetBytes());
      reader_->seekBlockBytes(offset);
    }
    return avro2::get_frame(id, *reader_);
  }
  FileData get_file_data() {
    internal_avro::DataFileReader<FileData> reader(
        path_.c_str(),
        internal_avro::compileJsonSchemaFromString(data_avro2::frame_json));
    return avro2::get_file_data(reader);
  }
  void flush() {}
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
        16 * 1024));
  }
  template <class T>
  void write(const T &t) {
    avro2::write(writer_.get(), t);
  }
  void flush() {
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
  Frame get_frame(const FileData &, FrameID, FrameID) { return Frame(); }
  FileData get_file_data() { return FileData(); }
  ~BufferWriterTraits() { flush(); }
};

struct BufferReaderTraits {
  boost::shared_ptr<internal_avro::DataFileReader<Frame> > reader_;
  BufferConstHandle buffer_;
  template <class T>
  boost::shared_ptr<internal_avro::DataFileReader<T> >
  get_reader() {
    boost::shared_ptr<internal_avro::InputStream> stream
      = internal_avro::memoryInputStream(buffer_.get_uint8_t().first,
                                         buffer_.get_uint8_t().second);
    return boost::make_shared<internal_avro::DataFileReader<T> >(
        stream, internal_avro::compileJsonSchemaFromString(
                    RMF::data_avro2::frame_json));
  }

  BufferReaderTraits(BufferConstHandle buffer) : buffer_(buffer) {}
  template <class T>
  void write(const T &) {}
  Frame get_frame(const FileData &file_data, FrameID old_frame, FrameID id) {
    if (old_frame == FrameID() || id < old_frame) reader_.reset();

    RMF_INTERNAL_CHECK(file_data.frame_block_offsets.find(id) !=
                           file_data.frame_block_offsets.end(),
                       "No such frame found");

    int64_t offset = file_data.frame_block_offsets.find(id)->second;
    if (!reader_ || reader_->blockOffsetBytes() > offset) reader_.reset();

    if (!reader_) {
      RMF_INFO(get_logger(), "Creating new reader");

      reader_ = get_reader<Frame>();
    }
    if (reader_->blockOffsetBytes() != offset) {
      RMF_TRACE(get_logger(), "Seeking to " << offset << " from "
                                            << reader_->blockOffsetBytes());
      reader_->seekBlockBytes(offset);
    }
    return avro2::get_frame(id, *reader_);
  }
  FileData get_file_data() {
    boost::shared_ptr<internal_avro::DataFileReader<FileData> > reader =
        get_reader<FileData>();
    return avro2::get_file_data(*reader);
  }
  void flush() {}
};

#if 0
struct GzipFileWriterTraits : public FileWriterTraitsBase {
  GzipFileWriterTraits(std::string path) : path_(path) {
      boost::iostreams::filtering_ostream ostream;
      ostream.push(boost::iostreams::gzip_compressor());
      ostream.push(boost::iostreams::file_sink(
          filename, std::ios_base::out | std::ios_base::binary));
      boost::shared_ptr<internal_avro::OutputStream> stream =
          internal_avro::ostreamOutputStream(ostream);
      writer_ = boost::make_shared<internal_avro::DataFileWriterBase>(
          stream, dschema, 16 * 1024);
  }
};

struct GzipFileReaderTraits {
  boost::shared_ptr<internal_avro::DataFileReader<Frame> > reader_;
  std::string path_;

  GzipFileReaderTraits(std::string path) : path_(path) {}
  template <class T>
  void write(const T &) {}
  Frame get_frame(const FileData &file_data, FrameID old_frame, FrameID id) {
    if (old_frame == FrameID() || id < old_frame) reader_.reset();

    RMF_INTERNAL_CHECK(file_data.frame_block_offsets.find(id) !=
                           file_data.frame_block_offsets.end(),
                       "No such frame found");

    int64_t offset = file_data.frame_block_offsets.find(id)->second;
    if (!reader_ || reader_->blockOffsetBytes() > offset) reader_.reset();

    if (!reader_) {
      RMF_INFO(get_logger(), "Creating new reader");
      reader_.reset(new internal_avro::DataFileReader<Frame>(
          path_.c_str(),
          internal_avro::compileJsonSchemaFromString(data_avro2::frame_json)));
    }
    if (reader_->blockOffsetBytes() != offset) {
      RMF_TRACE(get_logger(), "Seeking to " << offset << " from "
                                            << reader_->blockOffsetBytes());
      reader_->seekBlockBytes(offset);
    }
    return avro2::get_frame(id, *reader_);
  }
  FileData get_file_data() {
    internal_avro::DataFileReader<FileData> reader(
        path_.c_str(),
        internal_avro::compileJsonSchemaFromString(data_avro2::frame_json));
    return avro2::get_file_data(reader);
  }
  void flush() {}
};
#endif

}
}

RMF_DISABLE_WARNINGS
#endif  // RMF_AVRO2_TRAITS_H
