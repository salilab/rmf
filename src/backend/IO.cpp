#include "backend/IO.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path_traits.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unistd.h>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace backends {
namespace {
boost::ptr_vector<IOFactory> factories;
}

void add_io_factory(IOFactory *f) { factories.push_back(f); }

boost::shared_ptr<IO> create_file(const std::string &name) {
  if (boost::filesystem::exists(name)) {
    unlink(name.c_str());
  }
  BOOST_FOREACH(const IOFactory & f, factories) {
    if (!boost::algorithm::ends_with(name, f.get_file_extension())) continue;
    boost::shared_ptr<IO> cur = f.create_file(name);
    if (cur) return cur;
  }
  return boost::shared_ptr<IO>();
}
boost::shared_ptr<IO> create_buffer() {
  BOOST_FOREACH(const IOFactory & f, factories) {
    boost::shared_ptr<IO> cur = f.create_buffer();
    if (cur) return cur;
  }
  return boost::shared_ptr<IO>();
}
boost::shared_ptr<IO> read_file(const std::string &name) {
  BOOST_FOREACH(const IOFactory & f, factories) {
    if (!boost::algorithm::ends_with(name, f.get_file_extension())) continue;
    boost::shared_ptr<IO> cur = f.read_file(name);
    if (cur) return cur;
  }
  return boost::shared_ptr<IO>();
}
boost::shared_ptr<IO> open_buffer(const std::vector<char> &buffer) {
  BOOST_FOREACH(const IOFactory & f, factories) {
    boost::shared_ptr<IO> cur = f.open_buffer(buffer);
    if (cur) return cur;
  }
  return boost::shared_ptr<IO>();
}
boost::shared_ptr<IO> write_file(const std::string &name) {
  BOOST_FOREACH(const IOFactory & f, factories) {
    if (!boost::algorithm::ends_with(name, f.get_file_extension())) continue;
    boost::shared_ptr<IO> cur = f.write_file(name);
    if (cur) return cur;
  }
  return boost::shared_ptr<IO>();
}
}
}

RMF_DISABLE_WARNINGS
