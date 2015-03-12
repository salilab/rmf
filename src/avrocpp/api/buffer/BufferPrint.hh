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

#ifndef avro_BufferPrint_hh__
#define avro_BufferPrint_hh__

#include <ctype.h>
#include <iostream>
#include <iomanip>
#include "BufferReader.hh"

/**
 * \file BufferPrint.hh
 *
 * \brief Convenience functions for printing buffer contents
 **/

namespace internal_avro {

namespace detail {

/**
 * \fn hexPrint
 *
 * Prints a buffer to a stream in the canonical hex+ASCII format,
 * the same used by the program 'hexdump -C'
 *
 **/

inline void hexPrint(std::ostream& os, BufferReader& reader) {
  std::ios_base::fmtflags savedFlags = os.flags();

  char sixteenBytes[16];
  int offset = 0;

  os << std::setfill('0');
  os << std::hex;

  while (reader.bytesRemaining()) {

    os << std::setw(8) << offset << "  ";

    size_t inBuffer = reader.read(sixteenBytes, sizeof(sixteenBytes));
    offset += inBuffer;

    // traverse 8 bytes or inBuffer, whatever is less
    size_t cnt = std::min(inBuffer, static_cast<size_t>(8));

    size_t i = 0;
    for (; i < cnt; ++i) {
      os << std::setw(2);
      os << (static_cast<int>(sixteenBytes[i]) & 0xff) << ' ';
    }
    for (; i < 8; ++i) {
      os << "   ";
    }
    os << ' ';

    // traverse 16 bytes or inBuffer, whatever is less
    cnt = std::min(inBuffer, static_cast<size_t>(16));

    for (; i < cnt; ++i) {
      os << std::setw(2);
      os << (static_cast<int>(sixteenBytes[i]) & 0xff) << ' ';
    }
    for (; i < 16; ++i) {
      os << "   ";
    }
    os << " |";
    for (i = 0; i < inBuffer; ++i) {
      os.put(isprint(sixteenBytes[i] & 0xff) ? sixteenBytes[i] : '.');
    }
    os << "|\n";
  }

  // restore flags
  os.flags(savedFlags);
}

}  // namespace detail

}  // namespace

inline std::ostream& operator<<(std::ostream& os,
                                const internal_avro::OutputBuffer& buffer) {
  internal_avro::BufferReader reader(buffer);
  internal_avro::detail::hexPrint(os, reader);
  return os;
}

inline std::ostream& operator<<(std::ostream& os,
                                const internal_avro::InputBuffer& buffer) {
  internal_avro::BufferReader reader(buffer);
  internal_avro::detail::hexPrint(os, reader);
  return os;
}

#endif
