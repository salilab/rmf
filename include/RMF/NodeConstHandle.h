/**
 *  \file RMF/NodeConstHandle.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_NODE_CONST_HANDLE_H
#define RMF_NODE_CONST_HANDLE_H

#include <RMF/config.h>
#include "internal/SharedData.h"
#include "types.h"
#include "ID.h"
#include "enums.h"
#include "constants.h"
#include <boost/shared_ptr.hpp>

RMF_ENABLE_WARNINGS

#define RMF_NODE_CATCH(extra_info)                                             \
  catch (Exception& e) {                                                       \
    RMF_RETHROW(                                                               \
        File(get_file_name()) << Node(get_id())                                \
                              << Frame(get_current_frame_id())                 \
                              << Operation(BOOST_CURRENT_FUNCTION) extra_info, \
        e);                                                                    \
  }

#define RMF_NODE_CATCH_KEY(k, extra_info) \
  RMF_NODE_CATCH(<< Key(get_name(k))      \
                 << Category(get_category_name(k)) extra_info)

#define RMF_HDF5_NODE_CONST_KEY_TYPE_METHODS_DECL(                            \
    lcname, UCName, PassValue, ReturnValue, PassValues, ReturnValues)         \
 public:                                                                      \
  /** \brief get the value of the attribute k from this node                  \
      The node must have the attribute and if it is a per-frame               \
      attribute, and frame is not specified then frame 0 is                   \
      used.                                                                   \
  */                                                                          \
  ReturnValue get_value(UCName##Key k) const {                                \
    return get_value_impl<UCName##Traits, ReturnValue>(k);                    \
  }                                                                           \
  /** Return the attribute value or TypeTraits::get_null_value() if the       \
      node does not have the attribute. In python the method a value equal to \
      eg RMF.NullFloat if the attribute is not there.*/                       \
  ReturnValue get_value_always(UCName##Key k) const {                         \
    return get_value_always_impl<UCName##Traits, ReturnValue>(k);             \
  }                                                                           \
  /** If the default key is passed, false is returned.*/                      \
  bool get_has_value(UCName##Key k) const {                                   \
    return !UCName##Traits::get_is_null_value(get_value_always(k));           \
  }                                                                           \
  /** Return true if the node has data for that key that is specific          \
      to the current frame, as opposed to static data.*/                      \
  bool get_has_frame_value(UCName##Key k) const {                             \
    ReturnValue ret = shared_->get_current_value(node_, k);                   \
    return !UCName##Traits::get_is_null_value(ret);                           \
  }                                                                           \
 protected:                                                                   \
  std::string get_category_name(UCName##Key k) const;                         \
  std::string get_name(UCName##Key k) const;                                  \
 public:

RMF_VECTOR_DECL(NodeConstHandle);

namespace RMF {

class FileConstHandle;

class RootConstHandle;

//! A handle for a particular node in a read-only hierarchy.
/** Use these handles to access parts of the
    hierarchy.

    The get_value_always() methods return the appropriate
    value or TypeTraits::get_null_value() if the node does
    not have that attribute.

    See the NodeHandle for modifying the contents.
 */
class RMFEXPORT NodeConstHandle {
  friend class FileHandle;
  int compare(const NodeConstHandle& o) const {
    if (node_ < o.node_)
      return -1;
    else if (node_ > o.node_)
      return 1;
    else if (shared_.get() < o.shared_.get())
      return -1;
    else if (shared_.get() > o.shared_.get())
      return 1;
    else
      return 0;
  }
  template <class Traits, class ReturnValue, class K>
  ReturnValue get_value_always_impl(K k) const {
    try {
      if (shared_->get_current_frame() == ALL_FRAMES) {
        return shared_->get_static_value(node_, k);
      } else {
        ReturnValue ret = shared_->get_current_value(node_, k);
        if (!Traits::get_is_null_value(ret))
          return ret;
        else
          return shared_->get_static_value(node_, k);
      }
    }
    RMF_NODE_CATCH();
  }
  template <class Traits, class ReturnValue, class K>
  ReturnValue get_value_impl(K k) const {
    try {
      ReturnValue ret = get_value_always_impl<Traits, ReturnValue>(k);
      RMF_USAGE_CHECK(
          !Traits::get_is_null_value(ret),
          internal::get_error_message("Node ", get_name(),
                                      " does not have a value for key ",
                                      shared_->get_name(k)));
      return ret;
    }
    RMF_NODE_CATCH_KEY(k, );
  }

 protected:
  NodeID node_;
  boost::shared_ptr<internal::SharedData> shared_;
  std::string get_file_name() const;
  FrameID get_current_frame_id() const;
#if !defined(SWIG) && !defined(RMF_DOXYGEN)
 public:
  NodeID get_node_id() const { return node_; }
  NodeConstHandle(NodeID node, boost::shared_ptr<internal::SharedData> shared);
#endif

 public:
  RMF_COMPARISONS(NodeConstHandle);
  RMF_HASHABLE(NodeConstHandle, return node_.get_index());
  NodeConstHandle() {}

  //! Return the number of child nodes
  std::string get_name() const { return shared_->get_name(node_); }
  NodeConstHandles get_children() const;

#ifndef SWIG
  /** Each node can be associated at runtime with an
      arbitrary piece of data to aid in maintaining the
      relationship with application data structures. This
      data type must be convertible to a void* pointer.
      If direct casting does not work, you can implement
      a function called \c get_void_pointer() that takes
      the passed type and returns a \c void* pointer in
      the namespace where the type is declared (so it is
      found via Koenig lookup). Support has already been
      added for boost::shared_ptr.

      Either the association must not have been set before
      or overwrite must be true. If overwrite is true,
      the type must be the same as the old type.
   */
  template <class T>
  void set_association(const T& v, bool overwrite = false) {
    shared_->set_association(node_, v, overwrite);
  }
#else
  void set_association(void* v, bool overwrite = false);
#endif
  //! Return the associated pointer for this node
  /** An exception will be thrown if it doesn't have one.*/
  template <class T>
  T get_association() const {
    return boost::any_cast<T>(shared_->get_association(node_));
  }

  bool get_has_association() const {
    return shared_->get_has_association(node_);
  }

  //! get the type of this node
  NodeType get_type() const { return NodeType(shared_->get_type(node_)); }
  //! get a unique id for this node
  NodeID get_id() const { return NodeID(node_); }

  /** \name Functions to access attributes

      Type is one of the \ref rmf_types "standard types".

      @{
   */
  RMF_FOREACH_TYPE(RMF_HDF5_NODE_CONST_KEY_TYPE_METHODS_DECL);
  /** @} */
  RMF_SHOWABLE(NodeConstHandle, get_name() << "(" << get_type() << ", " << node_
                                           << ")");

  FileConstHandle get_file() const;
};

/** Print out the hierarchy as an ascii tree.
 */
RMFEXPORT void show_hierarchy(NodeConstHandle root,
                              std::ostream& out = std::cout);

/** Print out the hierarchy as an ascii tree along with values
    as described by the frame parameters. If end_frame is -1,
    the only one frame is shown.
 */
RMFEXPORT void show_hierarchy_with_values(NodeConstHandle root,
                                          std::ostream& out = std::cout);

/** Print out the hierarchy as an ascii tree marking what decorators
    apply where.
 */
RMFEXPORT void show_hierarchy_with_decorators(NodeConstHandle root,
                                              bool verbose = false,
                                              std::ostream& out = std::cout);

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_NODE_CONST_HANDLE_H */
