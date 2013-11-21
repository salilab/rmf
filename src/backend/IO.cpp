#include "backend/IO.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include "avro2/factory.h"
#include "avro/factory.h"
#include "hdf5/factory.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unistd.h>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace backends {
namespace {
boost::unordered_map<std::string, BufferHandle> test_buffers;
struct GetFactories : public std::vector<boost::shared_ptr<IOFactory> > {
  GetFactories() {
    std::vector<boost::shared_ptr<IOFactory> > favro2 = avro2::get_factories();
    insert(end(), favro2.begin(), favro2.end());
    std::vector<boost::shared_ptr<IOFactory> > fhdf5 =
        hdf5_backend::get_factories();
    insert(end(), fhdf5.begin(), fhdf5.end());
    std::vector<boost::shared_ptr<IOFactory> > favro =
        avro_backend::get_factories();
    insert(end(), favro.begin(), favro.end());
  }
} factories;
}

boost::shared_ptr<IO> create_file(const std::string &name) {
  if (boost::filesystem::exists(name)) {
    unlink(name.c_str());
  }
  if (boost::algorithm::ends_with(name, "_rmf_test_buffer")) {
    test_buffers[name] = BufferHandle();
    return create_buffer(test_buffers.find(name)->second);
  } else {
    RMF_FOREACH(boost::shared_ptr<IOFactory> f, factories) {
      if (!boost::algorithm::ends_with(name, f->get_file_extension())) continue;
      boost::shared_ptr<IO> cur = f->create_file(name);
      if (cur) return cur;
    }
  }
  return boost::shared_ptr<IO>();
}
boost::shared_ptr<IO> create_buffer(BufferHandle buffer) {
  RMF_FOREACH(boost::shared_ptr<IOFactory> f, factories) {
    boost::shared_ptr<IO> cur = f->create_buffer(buffer);
    if (cur) return cur;
  }
  return boost::shared_ptr<IO>();
}
boost::shared_ptr<IO> read_file(const std::string &name) {
  if (boost::algorithm::ends_with(name, "_rmf_test_buffer")) {
    return read_buffer(test_buffers.find(name)->second);
  } else {
    RMF_FOREACH(boost::shared_ptr<IOFactory> f, factories) {
      if (!boost::algorithm::ends_with(name, f->get_file_extension())) continue;
      boost::shared_ptr<IO> cur = f->read_file(name);
      if (cur) return cur;
    }
  }
  return boost::shared_ptr<IO>();
}
boost::shared_ptr<IO> read_buffer(BufferConstHandle buffer) {
  RMF_FOREACH(boost::shared_ptr<IOFactory> f, factories) {
    boost::shared_ptr<IO> cur = f->read_buffer(buffer);
    if (cur) return cur;
  }
  return boost::shared_ptr<IO>();
}
}
}

RMF_DISABLE_WARNINGS
