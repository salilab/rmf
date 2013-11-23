/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/FileHandle.h>
#include <RMF/internal/SharedData.h>
#include <RMF/internal/shared_data_factories.h>

RMF_ENABLE_WARNINGS

RMF_VECTOR_DEF(FileHandle);

namespace RMF {

FileHandle::FileHandle(boost::shared_ptr<internal::SharedData> shared)
    : FileConstHandle(shared) {}

NodeHandle FileHandle::get_node(NodeID id) const {
  return NodeHandle(id, shared_);
}

void FileHandle::flush() const {
  try {
    shared_->flush();
  }
  RMF_FILE_CATCH();
}

void FileHandle::set_description(std::string descr) const {
  try {
    shared_->set_description(descr);
  }
  RMF_FILE_CATCH();
}

void FileHandle::set_producer(std::string descr) const {
  try {
    shared_->set_producer(descr);
  }
  RMF_FILE_CATCH();
}

FrameID FileHandle::add_frame(std::string name, FrameType t) const {
  FrameID ret = shared_->add_frame(name, t);
  return ret;
}

FileHandle create_rmf_file(std::string path) {
  return FileHandle(internal::create_file(path));
}

FileHandle create_rmf_buffer(BufferHandle buffer) {
  return FileHandle(internal::create_buffer(buffer));
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
