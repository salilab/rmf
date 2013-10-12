/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/utility.h>
#include <RMF/internal/clone_shared_data.h>
#include <RMF/NodeConstHandle.h>
#include <RMF/NodeHandle.h>
#include <RMF/FileConstHandle.h>
#include <RMF/FileHandle.h>
#include <boost/unordered_set.hpp>
#include <RMF/internal/utility.h>
#include <RMF/decorators.h>

RMF_ENABLE_WARNINGS

namespace RMF {

void clone_file_info(FileConstHandle input, FileHandle output) {
  internal::clone_file(input.shared_, output.shared_);
}

void clone_hierarchy(FileConstHandle input, FileHandle output) {
  internal::clone_hierarchy(input.shared_, output.shared_);
}

void clone_loaded_frame(FileConstHandle input, FileHandle output) {
  internal::clone_loaded_data(input.shared_, output.shared_);
}

void clone_static_frame(FileConstHandle input, FileHandle output) {
  internal::clone_static_data(input.shared_, output.shared_);
}

namespace {
bool get_equal_node_structure(NodeConstHandle in, NodeConstHandle out,
                              bool print_diff) {
  bool ret = true;
  if (in.get_type() != out.get_type()) {
    if (print_diff) {
      std::cout << "Node types differ at " << in << " vs " << out << std::endl;
    }
    ret = false;
  }
  if (in.get_name() != out.get_name()) {
    if (print_diff) {
      std::cout << "Node names differ at " << in << " vs " << out << std::endl;
    }
    ret = false;
  }
  NodeConstHandles inch = in.get_children();
  NodeConstHandles outch = out.get_children();
  if (inch.size() != outch.size()) {
    if (print_diff) {
      std::cout << "Node number of children differ at " << in << " vs " << out
                << std::endl;
    }
    ret = false;
  }
  for (unsigned int i = 0; i < std::min(inch.size(), outch.size()); ++i) {
    ret = get_equal_node_structure(inch[i], outch[i], print_diff) && ret;
  }
  return ret;
}
}

bool get_equal_structure(FileConstHandle in, FileConstHandle out,
                         bool print_diff) {
  bool ret = true;
  ret = get_equal_node_structure(in.get_root_node(), out.get_root_node(),
                                 print_diff) &&
        ret;
  return ret;
}


bool get_equal_current_values(FileConstHandle in, FileConstHandle out) {
  return internal::get_equal_current_values(in.shared_, out.shared_);
}

bool get_equal_static_values(FileConstHandle in, FileConstHandle out) {
  return internal::get_equal_static_values(in.shared_, out.shared_);
}

void add_child_alias(AliasFactory af, NodeHandle parent,
                     NodeConstHandle child) {
  internal::add_child_alias(af, parent, child);
}

void test_throw_exception() {
  RMF_THROW(Message("Test exception"), UsageException);
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
