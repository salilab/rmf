/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/FileConstHandle.h>
#include <RMF/internal/SharedData.h>
#include <RMF/internal/shared_data_factories.h>
#include <RMF/Validator.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <sstream>

RMF_ENABLE_WARNINGS RMF_VECTOR_DEF(FileConstHandle);

namespace RMF {

FileConstHandle::FileConstHandle(boost::shared_ptr<internal::SharedData> shared)
    : shared_(shared) {}

NodeConstHandle FileConstHandle::get_node(NodeID id) const {
  return NodeConstHandle(id, shared_);
}

std::string FileConstHandle::get_description() const {
  try {
    return shared_->get_description();
  }
  RMF_FILE_CATCH();
}

std::string FileConstHandle::get_producer() const {
  try {
    return shared_->get_producer();
  }
  RMF_FILE_CATCH();
}

Floats get_values(const NodeConstHandles& nodes, FloatKey k,
                  Float missing_value) {
  Floats ret(nodes.size(), missing_value);
  for (unsigned int i = 0; i < nodes.size(); ++i) {
    if (!nodes[i].get_value(k).get_is_null()) {
      ret[i] = nodes[i].get_value(k);
    }
  }
  return ret;
}

FileConstHandle open_rmf_file_read_only(std::string path) {
  return FileConstHandle(internal::read_file(path));
}

FileConstHandle open_rmf_buffer_read_only(BufferConstHandle buffer) {
  return FileConstHandle(internal::read_buffer(buffer));
}

void FileConstHandle::validate(std::ostream& out = std::cerr) {
  try {
    Creators cs = get_validators();
    boost::ptr_vector<Validator> validators;
    for (unsigned int i = 0; i < cs.size(); ++i) {
      validators.push_back(cs[i]->create(*this));
    }
    for (int frame = -1; frame < static_cast<int>(get_number_of_frames());
         ++frame) {
      set_current_frame(FrameID(frame));
      for (unsigned int i = 0; i < cs.size(); ++i) {
        validators[i].write_errors(out);
      }
    }
  }
  RMF_FILE_CATCH();
}

std::string FileConstHandle::validate() {
  std::ostringstream oss;
  validate(oss);
  return oss.str();
}

void FileConstHandle::reload() {
  try {
    shared_->reload();
  }
  RMF_FILE_CATCH();
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
