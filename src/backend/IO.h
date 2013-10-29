/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_IO_H
#define RMF_INTERNAL_IO_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include <RMF/infrastructure_macros.h>
#include <boost/shared_ptr.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace backends {

struct IO {
  virtual void reload() = 0;
  /** \name Frame
      - parents (and possibly children) of current frame
    @{
  */
  virtual void save_loaded_frame(const internal::SharedData *shared_data) = 0;
  virtual void load_loaded_frame(internal::SharedData *shared_data) = 0;
  /** @} */
  /** \name Category
      - all data for all nodes
   @{
  */
  virtual void load_loaded_frame_category(
      Category category, internal::SharedData *shared_data) = 0;
  virtual void save_loaded_frame_category(
      Category category, const internal::SharedData *shared_data) = 0;
  virtual void load_static_frame_category(
      Category category, internal::SharedData *shared_data) = 0;
  virtual void save_static_frame_category(
      Category category, const internal::SharedData *shared_data) = 0;
  /** @} */
  /** \name File
      - description
      - producer
      - format
      - version
   @{ */
  virtual void load_file(internal::SharedData *shared_data) = 0;
  virtual void save_file(const internal::SharedData *shared_data) = 0;
  /** @} */
  /** \name Node Hierarchy
      @{ */
  virtual void load_hierarchy(internal::SharedData *shared_data) = 0;
  virtual void save_hierarchy(const internal::SharedData *shared_data) = 0;
  /** @} */
  virtual void flush() = 0;
  virtual std::vector<char> get_buffer() {
    RMF_THROW(Message("Not supported"), IOException);
  }
  virtual ~IO() {}
};

class IOFactory {
 public:
  virtual std::string get_file_extension() const = 0;
  virtual boost::shared_ptr<IO> read_file(const std::string &) const {
    return boost::shared_ptr<IO>();
  }
  virtual boost::shared_ptr<IO> create_buffer() const {
    return boost::shared_ptr<IO>();
  }
  virtual boost::shared_ptr<IO> write_file(const std::string &) const {
    return boost::shared_ptr<IO>();
  }
  virtual boost::shared_ptr<IO> open_buffer(const std::vector<char> &) const {
    return boost::shared_ptr<IO>();
  }
  virtual boost::shared_ptr<IO> create_file(const std::string &) const {
    return boost::shared_ptr<IO>();
  }
  virtual ~IOFactory() {}
};

RMFEXPORT void add_io_factory(IOFactory *f);
RMFEXPORT boost::shared_ptr<IO> create_file(const std::string &name);
RMFEXPORT boost::shared_ptr<IO> create_buffer();
RMFEXPORT boost::shared_ptr<IO> read_file(const std::string &name);
RMFEXPORT boost::shared_ptr<IO> open_buffer(const std::vector<char> &buffer);
RMFEXPORT boost::shared_ptr<IO> write_file(const std::string &name);

template <class Type>
struct IOFactoryRegistrar {
  IOFactoryRegistrar() { add_io_factory(new Type()); }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_IO_H */
