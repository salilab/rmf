/**
 *  \file RMF/FrameConstHandle.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_FRAME_CONST_HANDLE_H
#define RMF_FRAME_CONST_HANDLE_H

#include <RMF/config.h>
#include "internal/SharedData.h"
#include "types.h"
#include "ID.h"
#include "enums.h"
#include "constants.h"
#include <boost/shared_ptr.hpp>

RMF_ENABLE_WARNINGS

#define RMF_FRAME_CONST_KEY_TYPE_METHODS(                                 \
    lcname, UCName, PassValue, ReturnValue, PassValues, ReturnValues)          \
  /** \brief get the value of the attribute k from this frame                  \
      The frame must have the attribute and if it is a per-frame               \
      attribute, and frame is not specified then frame 0 is                    \
      used.                                                                    \
   */                                                                       \
  ReturnValue get_value(UCName##Key k) const {                                 \
    ReturnValue ret = get_value_always(k);                                                \
    RMF_USAGE_CHECK(!UCName##Traits::get_is_null_value(ret),                                                      \
        internal::get_error_message("Frame ",                                  \
                                    get_name(),                                \
                                    " does not have a value for key ",         \
                                    shared_->get_name(k)));                    \
    return ret;\
  }                                                                            \
  /** Return the attribute value or TypeTraits::get_null_value() if the        \
      frame does not have the attribute. In python the method a value equal to \
      eg RMF.NullFloat if the attribute is not there.*/                        \
  ReturnValue get_value_always(UCName##Key k) const {                          \
    RMF_USAGE_CHECK(get_shared_data()->get_current_frame() == get_frame_id() \
                     || get_frame_id() == ALL_FRAMES,\
                    "Attributes can only be fetched on the current frame "\
                    "or the static one.");\
    if (k == UCName##Key())                                                    \
      return UCName##Traits::get_null_value();                                 \
    if (get_frame_id() == ALL_FRAMES) return shared_->get_static_frame_value(k);\
    else {\
      ReturnValue ret = shared_->get_current_frame_value(k);\
      if (!UCName##Traits::get_is_null_value(ret)) return ret;\
      else return shared_->get_static_frame_value(k);\
    }\
  }                                                                            \
  /** If the default key is passed, false is returned.*/                       \
  bool get_has_value(UCName##Key k) const {                                    \
    return !UCName##Traits::get_is_null_value(get_value_always(k));            \
  }
RMF_VECTOR_DECL(FrameConstHandle);

namespace RMF {

class FileConstHandle;

class RootConstHandle;

//! A handle for a particular frame in a read-only hierarchy.
/** Use these handles to access parts of the
    frame hierarchy.

    The get_value_always() methods return the appropriate
    value or TypeTraits::get_null_value() if the frame does
    not have that attribute.

    See the FrameHandle for modifying the contents.
 */
class RMFEXPORT FrameConstHandle {
  FrameID frame_;
  friend class FileHandle;
  boost::shared_ptr<internal::SharedData> shared_;
  int compare(const FrameConstHandle& o) const {
    if (frame_ < o.frame_)
      return -1;
    else if (frame_ > o.frame_)
      return 1;
    else if (shared_.get() < o.shared_.get())
      return -1;
    else if (shared_.get() > o.shared_.get())
      return 1;
    else
      return 0;
  }
#if !defined(SWIG) && !defined(RMF_DOXYGEN)
 protected:
  boost::shared_ptr<internal::SharedData>
    get_shared_data() const { return shared_; }

 public:
  FrameID get_frame_id() const { return frame_; }
  FrameConstHandle(FrameID frame, boost::shared_ptr<internal::SharedData> shared);
#endif

 public:
  RMF_COMPARISONS(FrameConstHandle);
  RMF_HASHABLE(FrameConstHandle, return frame_.get_index());
  FrameConstHandle() : frame_() {}

  //! Return the name of the frame
  std::string get_name() const { return shared_->get_name(frame_); }
  FrameConstHandles get_children() const;

  //! get the type of this frame
  FrameType get_type() const { return shared_->get_type(frame_); }
  //! get a unique id for this frame
  FrameID get_id() const { return FrameID(frame_); }

  //! Set this to be the current frame in the file
  void set_as_current_frame();

  /** \name Functions to access attributes

      Type is one of the \ref rmf_types "standard types".

      @{
   */
  RMF_FOREACH_TYPE(RMF_FRAME_CONST_KEY_TYPE_METHODS);
  /** @} */
  RMF_SHOWABLE(FrameConstHandle,
               get_name() << "(" << get_type() << ", " << frame_ << ")");

  FileConstHandle get_file() const;
};

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_FRAME_CONST_HANDLE_H */
