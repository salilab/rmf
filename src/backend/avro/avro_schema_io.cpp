/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "avro_schema_io.h"
#include <RMF/decorators.h>
#include <backend/avro/File.json.h>
#include <backend/avro/All.json.h>
#include <backend/avro/Data.json.h>
#include <backend/avro/Nodes.json.h>
#include <backend/avro/Frame.json.h>
#include "avro_schemas.h"
#include <backend/avro/AvroCpp/api/Compiler.hh>
#include <stdexcept>
#include <backend/avro/AvroCpp/api/Encoder.hh>
#include <backend/avro/AvroCpp/api/Stream.hh>
#include <boost/scoped_ptr.hpp>

RMF_ENABLE_WARNINGS namespace RMF {
  namespace avro_backend {

  void show(const RMF_avro_backend::Data& data, std::ostream& out) {
    boost::scoped_ptr< ::rmf_avro::OutputStream> os(
        rmf_avro::ostreamOutputStream(out).release());
    ::rmf_avro::EncoderPtr encoder = rmf_avro::jsonEncoder(get_Data_schema());
    encoder->init(*os);
    ::rmf_avro::codec_traits<RMF_avro_backend::Data>::encode(*encoder, data);
    os->flush();
  }

  }  // namespace avro_backend
}    /* namespace RMF */

RMF_DISABLE_WARNINGS
