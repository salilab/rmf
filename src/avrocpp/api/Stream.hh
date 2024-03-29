/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef avro_Stream_hh__
#define avro_Stream_hh__

#include <memory>
#include <string.h>
#include <stdint.h>

#include "boost/utility.hpp"

#include "Config.hh"
#include "Exception.hh"

namespace internal_avro {

/**
 * A no-copy input stream.
 */
class AVRO_DECL InputStream : boost::noncopyable {
 protected:
  /**
   * An empty constuctor.
   */
  InputStream() {}

 public:
  /**
   * Destructor.
   */
  virtual ~InputStream() {}

  /**
   * Returns some of available data.
   *
   * Returns true if some data is available, false if no more data is
   * available or an error has occurred.
   */
  virtual bool next(const uint8_t** data, size_t* len) = 0;

  /**
   * "Returns" back some of the data to the stream. The returned
   * data must be less than what was obtained in the last call to
   * next().
   */
  virtual void backup(size_t len) = 0;

  /**
   * Skips number of bytes specified by len.
   */
  virtual void skip(size_t len) = 0;

  /**
   * Returns the number of bytes read from this stream so far.
   * All the bytes made available through next are considered
   * to be used unless, retutned back using backup.
   */
  virtual size_t byteCount() const = 0;

  /**
   * Returns the number of bytes that can still be read from the
   * stream or -1 if that is not available. All the bytes made available
   * through next are considered to be used unless, retutned back using backup.
   */
  virtual int64_t remainingBytes() const = 0;
};

/**
 * A no-copy output stream.
 */
class AVRO_DECL OutputStream : boost::noncopyable {
 protected:
  /**
   * An empty constuctor.
   */
  OutputStream() {}

 public:
  /**
   * Destructor.
   */
  virtual ~OutputStream() {}

  /**
   * Returns a buffer that can be written into.
   * On successful return, data has the pointer to the buffer
   * and len has the number of bytes available at data.
   */
  virtual bool next(uint8_t** data, size_t* len) = 0;

  /**
   * "Returns" back to the stream some of the buffer obtained
   * from in the last call to next().
   */
  virtual void backup(size_t len) = 0;

  /**
   * Number of bytes written so far into this stream. The whole buffer
   * returned by next() is assumed to be written unless some of
   * it was retutned using backup().
   */
  virtual uint64_t byteCount() const = 0;

  /**
   * Flushes any data remaining in the buffer to the stream's underlying
   * store, if any.
   */
  virtual void flush() = 0;
};

/**
 * Returns a new OutputStream, which grows in memory chunks of specified size.
 */
AVRO_DECL std::shared_ptr<OutputStream> memoryOutputStream(size_t chunkSize =
                                                                 4 * 1024);

/**
 * Returns a new InputStream, with the data from the given byte array.
 * It does not copy the data, the byte array should remain valid
 * until the InputStream is used.
 */
AVRO_DECL std::shared_ptr<InputStream> memoryInputStream(const uint8_t* data,
                                                           size_t len);

/**
 * Returns a new InputStream with the contents written into an
 * outputstream. The output stream must have been returned by
 * an earlier call to memoryOutputStream(). The contents for the new
 * input stream are the snapshot of the outputstream. One can construct
 * any number of memory input stream from a single memory output stream.
 */
AVRO_DECL std::shared_ptr<InputStream> memoryInputStream(
    const OutputStream& source);

/**
 * Returns a new OutputStream whose contents would be stored in a file.
 * Data is written in chunks of given buffer size.
 *
 * If there is a file with the given name, it is truncated and overwritten.
 * If there is no file with the given name, it is created.
 */
AVRO_DECL std::shared_ptr<OutputStream> fileOutputStream(const char* filename,
                                                           size_t bufferSize =
                                                               8 * 1024);

/**
 * Returns a new InputStream whose contents come from the given file.
 * Data is read in chunks of given buffer size.
 */
AVRO_DECL std::shared_ptr<InputStream> fileInputStream(const char* filename,
                                                         size_t bufferSize =
                                                             8 * 1024);

/**
 * Returns a new OutputStream whose contents will be sent to the given
 * std::ostream. The std::ostream object should outlive the returned
 * OutputStream.
 */
AVRO_DECL std::shared_ptr<OutputStream> ostreamOutputStream(
    std::ostream& os, size_t bufferSize = 8 * 1024);

/**
 * Returns a new InputStream whose contents come from the given
 * std::istream. The std::istream object should outlive the returned
 * InputStream.
 */
AVRO_DECL std::shared_ptr<InputStream> istreamInputStream(std::istream& in,
                                                            size_t bufferSize =
                                                                8 * 1024);

/** A convenience class for reading from an InputStream */
struct StreamReader {
  /**
   * The underlying input stream.
   */
  InputStream* in_;

  /**
   * The next location to read from.
   */
  const uint8_t* next_;

  /**
   * One past the last valid location.
   */
  const uint8_t* end_;

  /**
   * Constructs an empty reader.
   */
  StreamReader() : in_(0), next_(0), end_(0) {}

  /**
   * Constructs a reader with the given underlying stream.
   */
  StreamReader(InputStream& in) : in_(0), next_(0), end_(0) { reset(in); }

  /**
   * Replaces the current input stream with the given one after backing up
   * the original one if required.
   */
  void reset(InputStream& is) {
    if (in_ != 0 && end_ != next_) {
      in_->backup(end_ - next_);
    }
    in_ = &is;
    next_ = end_ = 0;
  }

  /**
   * Read just one byte from the underlying stream. If there are no
   * more data, throws an exception.
   */
  uint8_t read() {
    if (next_ == end_) {
      more();
    }
    return *next_++;
  }

  /**
   * Reads the given number of bytes from the underlying stream.
   * If there are not that many bytes, throws an exception.
   */
  void readBytes(uint8_t* b, size_t n) {
    while (n > 0) {
      if (next_ == end_) {
        more();
      }
      size_t q = end_ - next_;
      if (q > n) {
        q = n;
      }
      ::memcpy(b, next_, q);
      next_ += q;
      b += q;
      n -= q;
    }
  }

  /**
   * Skips the given number of bytes. Of there are not so that many
   * bytes, throws an exception.
   */
  void skipBytes(size_t n) {
    if (n > static_cast<size_t>(end_ - next_)) {
      n -= end_ - next_;
      next_ = end_;
      in_->skip(n);
    } else {
      next_ += n;
    }
  }

  /**
   * Get as many byes from the underlying stream as possible in a single
   * chunk.
   * \return true if some data could be obtained. False is no more
   * data is available on the stream.
   */
  bool fill() {
    size_t n = 0;
    while (in_->next(&next_, &n)) {
      if (n != 0) {
        end_ = next_ + n;
        return true;
      }
    }
    return false;
  }

  /**
   * Tries to get more data and if it cannot, throws an exception.
   */
  void more() {
    if (!fill()) {
      throw Exception("EOF reached");
    }
  }

  /**
   * Returns true if and only if the end of stream is not reached.
   */
  bool hasMore() { return (next_ == end_) ? fill() : true; }
};

/**
 * A convinience class to write data into an OutputStream.
 */
struct StreamWriter {
  /**
   * The underlying output stream for this writer.
   */
  OutputStream* out_;

  /**
   * The next location to write to.
   */
  uint8_t* next_;

  /**
   * One past the last location one can write to.
   */
  uint8_t* end_;

  /**
   * Constructs a writer with no underlying stream.
   */
  StreamWriter() : out_(0), next_(0), end_(0) {}

  /**
   * Constructs a new writer with the given underlying stream.
   */
  StreamWriter(OutputStream& out) : out_(0), next_(0), end_(0) { reset(out); }

  /**
   * Replaces the current underlying stream with a new one.
   * If required, it backs up unused bytes in the previous stream.
   */
  void reset(OutputStream& os) {
    if (out_ != 0 && end_ != next_) {
      out_->backup(end_ - next_);
    }
    out_ = &os;
    next_ = end_;
  }

  /**
   * Writes a single byte.
   */
  void write(uint8_t c) {
    if (next_ == end_) {
      more();
    }
    *next_++ = c;
  }

  /**
   * Writes the specified number of bytes starting at \p b.
   */
  void writeBytes(const uint8_t* b, size_t n) {
    while (n > 0) {
      if (next_ == end_) {
        more();
      }
      size_t q = end_ - next_;
      if (q > n) {
        q = n;
      }
      ::memcpy(next_, b, q);
      next_ += q;
      b += q;
      n -= q;
    }
  }

  /**
   * backs up upto the currently written data and flushes the
   * underlying stream.
   */
  void flush() {
    if (next_ != end_) {
      out_->backup(end_ - next_);
      next_ = end_;
    }
    out_->flush();
  }

  /**
   * Gets more space to write to. Throws an exception it cannot.
   */
  void more() {
    size_t n = 0;
    while (out_->next(&next_, &n)) {
      if (n != 0) {
        end_ = next_ + n;
        return;
      }
    }
    throw Exception("EOF reached");
  }
};

/**
 * A convenience function to copy all the contents of an input stream into
 * an output stream.
 */
inline void copy(InputStream& in, OutputStream& out) {
  const uint8_t* p = 0;
  size_t n = 0;
  StreamWriter w(out);
  while (in.next(&p, &n)) {
    w.writeBytes(p, n);
  }
  w.flush();
}

}  // namespace internal_avro
#endif
