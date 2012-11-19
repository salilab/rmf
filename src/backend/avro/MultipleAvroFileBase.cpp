/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include "avro_schemas.h"
#include "MultipleAvroFileBase.h"
#include <RMF/internal/paths.h>
#include <RMF/decorators.h>
#include <avro/Compiler.hh>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <stdexcept>

namespace RMF {
  namespace internal {

    std::string MultipleAvroFileBase::get_category_dynamic_file_path(Category cat) const {
      boost::filesystem::path base=get_file_path();
      boost::filesystem::path full= base/(get_category_name(cat)+".frames");
      return full.string();
    }
    std::string MultipleAvroFileBase::get_category_static_file_path(Category cat) const {
      boost::filesystem::path base=get_file_path();
      boost::filesystem::path full= base/(get_category_name(cat)+".static");
      return full.string();
    }
    std::string MultipleAvroFileBase::get_file_file_path() const {
      boost::filesystem::path base=get_file_path();
      boost::filesystem::path full= base/"file";
      return full.string();
    }
    std::string MultipleAvroFileBase::get_nodes_file_path() const {
      boost::filesystem::path base=get_file_path();
      boost::filesystem::path full= base/"nodes";
      return full.string();
    }
    std::string MultipleAvroFileBase::get_static_file_path() const {
      boost::filesystem::path base=get_file_path();
      boost::filesystem::path full= base/"static_data";
      return full.string();
    }
    std::string MultipleAvroFileBase::get_frames_file_path() const {
      boost::filesystem::path base=get_file_path();
      boost::filesystem::path full= base/"frames";
      return full.string();
    }

    MultipleAvroFileBase::MultipleAvroFileBase(std::string path):
      AvroKeysAndCategories(path){
      null_static_data_.frame=ALL_FRAMES;
      null_data_.frame=0;
    }

  } // namespace internal
} /* namespace RMF */
