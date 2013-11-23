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
    avro2::write(writer_.get(), t);
  }
  void flush() { writer_->flush(); }
  Frame get_frame(const FileData &, FrameID, FrameID) { return Frame(); }
  FileData get_file_data() { return FileData(); }
  ~FileWriterTraitsBase() { flush(); }
};

template <class Base>
struct ReaderTraits {
  Base base_;
  boost::shared_ptr<internal_avro::DataFileReader<Frame> > reader_;

  template <class T>
  ReaderTraits(T path)
      : base_(path) {}
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
      reader_ = base_.template get_reader<Frame>();
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
        base_.template get_reader<FileData>();
    return avro2::get_file_data(*reader);
  }
  void flush() {}
};


struct FileWriterTraits : public FileWriterTraitsBase {
  FileWriterTraits(std::string path) : FileWriterTraitsBase(path) {
    writer_.reset(new internal_avro::DataFileWriterBase(
        path_.c_str(),
        internal_avro::compileJsonSchemaFromString(RMF::data_avro2::frame_json),
        16 * 1024));
  }
};

struct FileReaderBase {
  std::string path_;

  FileReaderBase(std::string path) : path_(path) {}
  template <class T>
  boost::shared_ptr<internal_avro::DataFileReader<T> >
  get_reader() {
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

struct BufferReaderBase {
  BufferConstHandle buffer_;
  BufferReaderBase(BufferConstHandle buffer) : buffer_(buffer) {}
  template <class T>
  boost::shared_ptr<internal_avro::DataFileReader<T> > get_reader() {
    boost::shared_ptr<internal_avro::InputStream> stream
      = internal_avro::memoryInputStream(buffer_.get_uint8_t().first,
                                         buffer_.get_uint8_t().second);
    return boost::make_shared<internal_avro::DataFileReader<T> >(stream,
                                                                 get_schema());
  }

};

struct GzipFileWriterTraits : public FileWriterTraitsBase {
  GzipFileWriterTraits(std::string path) : FileWriterTraitsBase(path) {
      boost::iostreams::filtering_ostream ostream;
      ostream.push(boost::iostreams::gzip_compressor());
      ostream.push(boost::iostreams::file_sink(
          path_, std::ios_base::out | std::ios_base::binary));
      boost::shared_ptr<internal_avro::OutputStream> stream =
          internal_avro::ostreamOutputStream(ostream);
      writer_ = boost::make_shared<internal_avro::DataFileWriterBase>(
          stream, get_schema(), 16 * 1024);
  }
};

struct GzipFileReaderBase{
  std::string path_;

  GzipFileReaderBase(std::string path) : path_(path) {}
  template <class T>
  boost::shared_ptr<internal_avro::DataFileReader<T> > get_reader() {
    boost::iostreams::filtering_istream istream;
    istream.push(boost::iostreams::gzip_decompressor());
    istream.push(boost::iostreams::file_source(
        path_, std::ios_base::in | std::ios_base::binary));
    boost::shared_ptr<internal_avro::InputStream> stream =
        internal_avro::istreamInputStream(istream);
    return boost::make_shared<internal_avro::DataFileReader<T> >(stream,
                                                                 get_schema());
  }
};
}
}

RMF_DISABLE_WARNINGS
#endif  // RMF_AVRO2_TRAITS_H
