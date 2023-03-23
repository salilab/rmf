/**
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

#include "cpx.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

int main() {
  std::shared_ptr<internal_avro::OutputStream> out =
      internal_avro::memoryOutputStream();
  internal_avro::EncoderPtr e = internal_avro::binaryEncoder();
  e->init(*out);
  c::cpx c1;
  c1.re = 1.0;
  c1.im = 2.13;
  internal_avro::encode(*e, c1);

  std::shared_ptr<internal_avro::InputStream> in =
      internal_avro::memoryInputStream(*out);
  internal_avro::DecoderPtr d = internal_avro::binaryDecoder();
  d->init(*in);

  c::cpx c2;
  internal_avro::decode(*d, c2);
  std::cout << '(' << c2.re << ", " << c2.im << ')' << std::endl;
  return 0;
}
