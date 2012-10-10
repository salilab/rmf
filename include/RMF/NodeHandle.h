/**
 *  \file RMF/NodeHandle.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPLIBRMF_NODE_HANDLE_H
#define IMPLIBRMF_NODE_HANDLE_H

#include <RMF/config.h>
#include "HDF5Group.h"
#include "internal/SharedData.h"
#include "types.h"
#include "NodeID.h"
#include "NodeConstHandle.h"



#define IMP_HDF5_NODE_KEY_TYPE_METHODS(lcname, UCName, PassValue, ReturnValue, \
                                       PassValues, ReturnValues)        \
/** \brief  set the value of the attribute k for this node
    If it is a per-frame attribute, frame must be specified.
*/                                                                      \
void set_value(UCName##Key k, PassValue v,                              \
               int frame =ALL_FRAMES) {                                 \
  IMP_RMF_USAGE_CHECK(frame >=0 || !k.get_is_per_frame(),               \
                      "No frame specified for per-frame data.");        \
  get_shared_data()->set_value(get_node_id(),                           \
                               k, v, frame);                            \
}                                                                       \
/** \brief  set the values of the attributes k for this node
    These keys must have been produced by the add_keys method.
    If it is a per-frame attribute, frame must be specified.
*/                                                                      \
void set_values(const UCName##Key##s& k, PassValues v,                  \
                int frame =ALL_FRAMES) {                                \
  IMP_RMF_USAGE_CHECK(frame >=0 || !k[0].get_is_per_frame(),            \
                      "No frame specified for per-frame data.");        \
  get_shared_data()->set_values(get_node_id(),                          \
                                k, v, frame);                           \
}

namespace RMF {

class NodeHandle;
// for children
typedef vector<NodeHandle> NodeHandles;

class FileHandle;

//! A handle for a particular node in the hierarchy.
/** Use these handles to access and modify parts of the
    hierarchy.

    Make sure to check out the base class for the const
    methods.
*/
class RMFEXPORT NodeHandle: public NodeConstHandle {
  friend class FileHandle;
#if !defined(SWIG) && !defined(IMP_DOXYGEN)
 public:
  NodeHandle(int node, internal::SharedData *shared);
#endif

 public:
  NodeHandle(){}
  /** Create a new node as a child of this one.
   */
  NodeHandle add_child(std::string name, NodeType t);

  NodeHandles get_children() const;

  /** \name Functions to access attributes

      Type is one of the \ref rmf_types "standard types".

      @{
  */
  IMP_RMF_FOREACH_TYPE(IMP_HDF5_NODE_KEY_TYPE_METHODS);
  /** @} */

  FileHandle get_file() const;
};

/** See get_children_resolving_aliases(NodeConstHandle).*/
RMFEXPORT NodeHandles get_children_resolving_aliases(NodeHandle nh);

/** Add a child to the node that is an alias to another node.*/
RMFEXPORT NodeHandle add_child_alias(NodeHandle parent,
                                     NodeConstHandle alias);
} /* namespace RMF */

#endif /* IMPLIBRMF_NODE_HANDLE_H */