/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/FileHandle.h>
#include <RMF/internal/SharedData.h>

namespace RMF {

FileHandle::FileHandle(internal::SharedData *shared): FileConstHandle(shared) {}

// \exception RMF::IOException if couldn't create file,
//                             or if unsupported file format
FileHandle::FileHandle(std::string name, bool create):
    FileConstHandle(internal::create_shared_data(name, create))  {
}

NodeHandle FileHandle::get_node_from_id(NodeID id) const {
  return NodeHandle(id.get_index(), get_shared_data());
}


void FileHandle::set_description(std::string descr) {
  try {
    get_shared_data()->set_description(descr);
  } RMF_FILE_CATCH();
}

void FileHandle::set_producer(std::string descr) {
  try {
    get_shared_data()->set_producer(descr);
 } RMF_FILE_CATCH();
}

FileHandle open_rmf_file(std::string path) {
  return FileHandle(path, false);
}

  FileHandle create_rmf_file(std::string path) {
    return FileHandle(path, true);
  }

 FileHandle create_rmf_buffer(std::string &buffer) {
   return FileHandle(internal::create_shared_data_in_buffer(buffer, true));
 }


} /* namespace RMF */
