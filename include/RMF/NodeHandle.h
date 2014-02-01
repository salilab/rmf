/**
 *  \file RMF/NodeHandle.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_NODE_HANDLE_H
#define RMF_NODE_HANDLE_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include "ID.h"
#include "NodeConstHandle.h"
#include "RMF/Nullable.h"
#include "RMF/config.h"
#include "RMF/enums.h"
#include "RMF/exceptions.h"
#include "RMF/infrastructure_macros.h"
#include "RMF/internal/SharedData.h"
#include "internal/SharedData.h"
#include "types.h"

namespace RMF {
class NodeHandle;
}  // namespace RMF

RMF_ENABLE_WARNINGS

#define RMF_HDF5_NODE_KEY_TYPE_METHODS(Traits, UCName)                      \
  /** \brief  set the value of the attribute k for this node on the         \
      current frame.                                                        \
  */                                                                        \
  void set_frame_value(UCName##Key k, Traits::ArgumentType v) const {       \
    RMF_USAGE_CHECK(shared_->get_loaded_frame() != FrameID(),               \
                    "Need to set a current frame before setting values.");  \
    shared_->set_loaded_value(node_, k, v);                                 \
  }                                                                         \
  /** Set the value                                                         \
      - if the attribute has a static value and it is equal the current one \
        do nothing.                                                         \
      - if the attribute doesn't have a static value, set it,               \
      - otherwise set the frame value.                                      \
  */                                                                        \
  void set_value(UCName##Key k, Traits::ArgumentType v) const {             \
    set_value_impl(k, v);                                                   \
  }                                                                         \
  /** \brief  set the value of the attribute k for all frames.              \
   *                                                                        \
  */                                                                        \
  void set_static_value(UCName##Key k, Traits::ArgumentType v) const {      \
    shared_->set_static_value(node_, k, v);                                 \
  }

namespace RMF {

class NodeHandle;

//! Pass a list of them
typedef std::vector<NodeHandle> NodeHandles;

class FileHandle;

//! A handle for a particular node in the hierarchy.
/** Use these handles to access and modify parts of the
    hierarchy.

    Make sure to check out the base class for the const
    methods.
 */
class RMFEXPORT NodeHandle : public NodeConstHandle {
  friend class FileHandle;
  template <class Traits>
  void set_value_impl(ID<Traits> k, typename Traits::ArgumentType v) const {
    Nullable<typename Traits::Type> sv = get_static_value(k);
    if (sv.get_is_null()) {
      set_static_value(k, v);
      return;
    }
    if (Traits::get_are_equal(sv.get(), v)) return;
    set_frame_value(k, v);
  }
#if !defined(SWIG) && !defined(RMF_DOXYGEN)
 public:
  NodeHandle(NodeID node, boost::shared_ptr<internal::SharedData> shared);
#endif

 public:
  NodeHandle() {}
  /** Create a new node as a child of this one.
   */
  NodeHandle add_child(std::string name, NodeType t) const;

  /** Add an existing node as a child.*/
  void add_child(NodeConstHandle nh) const;

  NodeHandles get_children() const;

  /** \name Functions to access attributes

      @{
   */
  RMF_FOREACH_TYPE(RMF_HDF5_NODE_KEY_TYPE_METHODS);
  /** @} */

  FileHandle get_file() const;
};
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_NODE_HANDLE_H */
