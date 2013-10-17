/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/internal/SharedData.h>
#include <RMF/NodeHandle.h>
#include <boost/unordered_set.hpp>
#include <RMF/HDF5/File.h>
#include <boost/filesystem/path.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/version.hpp>
#include <RMF/log.h>

RMF_ENABLE_WARNINGS namespace RMF {
  namespace internal {

  SharedData::SharedData(std::string path)
      : valid_(11111), path_(path) {
    RMF_INFO(get_logger(), "Opening file " << path_);
  }

  SharedData::~SharedData() {
    RMF_INTERNAL_CHECK(valid_ == 11111, "Already destroyed");
    valid_ = -66666;
    RMF_INFO(get_logger(), "Closing file " << path_);
  }

  void SharedData::audit_key_name(std::string name) const {
    if (name.empty()) {
      RMF_THROW(Message("Empty key name"), UsageException);
    }
    static const char* illegal = "\\:=()[]{}\"'";
    const char* cur = illegal;
    while (*cur != '\0') {
      if (name.find(*cur) != std::string::npos) {
        RMF_THROW(Message(get_error_message("Key names can't contain ", *cur)),
                  UsageException);
      }
      ++cur;
    }
    if (name.find("  ") != std::string::npos) {
      RMF_THROW(Message("Key names can't contain two consecutive spaces"),
                UsageException);
    }
  }

  void SharedData::audit_node_name(std::string name) const {
    if (name.empty()) {
      RMF_THROW(Message("Empty key name"), UsageException);
    }
    static const char* illegal = "\"";
    const char* cur = illegal;
    while (*cur != '\0') {
      if (name.find(*cur) != std::string::npos) {
        RMF_THROW(
            Message(get_error_message("Node names names can't contain \"", *cur,
                                      "\", but \"", name, "\" does.")),
            UsageException);
      }
      ++cur;
    }
  }

  std::string SharedData::get_file_name() const {
#if BOOST_VERSION >= 104600
    return boost::filesystem::path(path_).filename().string();
#else
    return boost::filesystem::path(path_).filename();
#endif
  }


  }  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS
