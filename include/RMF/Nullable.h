/**
 *  \file RMF/Nullable.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_NULLABLE_H
#define RMF_NULLABLE_H

#include <RMF/config.h>
#include "infrastructure_macros.h"
#include "exceptions.h"
#include <limits>

RMF_ENABLE_WARNINGS

namespace RMF {

/** Store a value as returned by RMF with the additional ability to check if it
  is the null value. These get converted to plain values or `None` in python.
  \note they should never be stored.
*/
template <class Traits>
class Nullable {
  typename Traits::ReturnType v_;
 public:
  Nullable(typename Traits::ReturnType v): v_(v) {}
#ifndef SWIG
  /** \pre !get_is_null() */
  operator const typename Traits::Type& () const {
    return get();
  }
  /** \pre !get_is_null() */
  const typename Traits::Type& get() const {
    RMF_USAGE_CHECK(!get_is_null(), "Can't convert null value.");
    return v_;
  }
#else
  // otherwise SWIG gets confused by it being, eg, a pointer to an int
  typename Traits::Type get() const {
    RMF_USAGE_CHECK(!get_is_null(), "Can't convert null value.");
    return v_;
  }
#endif

  bool get_is_null() const { return Traits::get_is_null_value(v_); }

};

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_NULLABLE_H */
