#include <RMF/internal/shared_data_factories.h>
#include <backend/hdf5/create.h>
#include <backend/avro/create.h>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace internal {

namespace {
boost::shared_ptr<SharedData> create_shared_data_internal(std::string path,
                                                          bool create,
                                                          bool read_only) {
  try {
    boost::shared_ptr<SharedData> ret;
    if ((ret = hdf5_backend::create_shared_data(path, create, read_only))) {
      return ret;
    } else if ((ret = avro_backend::create_shared_data(path, create,
                                                       read_only))) {
      return ret;
    } else {
      RMF_THROW(Message("Don't know how to open file"), IOException);
    }
  }
  catch (Exception& e) {
    RMF_RETHROW(File(path), e);
  }
}
}

boost::shared_ptr<SharedData> create_file(const std::string& name) {
  return create_shared_data_internal(name, true, false);
}

boost::shared_ptr<SharedData> create_buffer() {  try {
    boost::shared_ptr<SharedData> ret =
        avro_backend::create_shared_data_buffer();
    if (ret) {
      return ret;
    } else {
      RMF_THROW(Message("Don't know how to open file"), IOException);
    }
  }
  catch (Exception& e) {
    RMF_RETHROW(File("buffer"), e);
  }
}

boost::shared_ptr<SharedData> read_file(const std::string& name) {
 return create_shared_data_internal(name, false, true);
}
boost::shared_ptr<SharedData> open_buffer(const std::vector<char>& buffer) {
  try {
    boost::shared_ptr<SharedData> ret =
        avro_backend::open_shared_data_buffer(buffer);
    if (ret) {
      return ret;
    } else {
      RMF_THROW(Message("Don't know how to open file"), IOException);
    }
  }
  catch (Exception& e) {
    RMF_RETHROW(File("buffer"), e);
  }
}
boost::shared_ptr<SharedData> write_file(const std::string& name) {
  return create_shared_data_internal(name, false, false);
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS
