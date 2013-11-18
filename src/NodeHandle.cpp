/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/NodeHandle.h>
#include <boost/tuple/tuple.hpp>
#include <RMF/FileHandle.h>
#include <RMF/decorators.h>

RMF_ENABLE_WARNINGS

RMF_VECTOR_DEF(NodeHandle);

namespace RMF {

NodeHandle::NodeHandle(NodeID node,
                       boost::shared_ptr<internal::SharedData> shared)
    : NodeConstHandle(node, shared) {}

NodeHandle NodeHandle::add_child(std::string name, NodeType t) const {
  try {
    return NodeHandle(shared_->add_child(node_, name, t), shared_);
  }
  RMF_NODE_CATCH();
}

void NodeHandle::add_child(NodeConstHandle nh) const {
  try {
    shared_->add_child(node_, nh.get_id());
  }
  RMF_NODE_CATCH();
}

FileHandle NodeHandle::get_file() const { return FileHandle(shared_); }

NodeHandles NodeHandle::get_children() const {
  try {
    NodeIDs children = shared_->get_children(node_);
    NodeHandles ret(children.size());
    for (unsigned int i = 0; i < ret.size(); ++i) {
      ret[i] = NodeHandle(children[i], shared_);
    }
    return ret;
  }
  RMF_NODE_CATCH();
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
