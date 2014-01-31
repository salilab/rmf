/**
 *  \file RMF/Nullable.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_NULLABLE_H
#define RMF_NULLABLE_H

#include "RMF/config.h"
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
  std::string get_string() const {
    if (get_is_null())
      return "<null>";
    else {
      std::ostringstream oss;
      oss << v_;
      return oss.str();
    }
  }

 public:
  Nullable(const Nullable& o) : v_(o.v_) {}
  Nullable(typename Traits::ReturnType v) : v_(v) {}
#ifndef SWIG
  /** \pre !get_is_null() */
  operator const typename Traits::Type&() const { return get(); }
  /** \pre !get_is_null() */
  const typename Traits::Type& get() const {
    RMF_USAGE_CHECK(!get_is_null(), "Can't convert null value.");
    return v_;
  }
#endif
  /** For python since it nicely becomes None. */
  const typename Traits::Type* get_ptr() const {
    if (get_is_null())
      return NULL;
    else
      return &v_;
  }
#if !defined(RMF_DOXYGEN) && !defined(SWIG)
  void show(std::ostream& out) const { out << get_string(); }
#endif

  bool get_is_null() const { return Traits::get_is_null_value(v_); }
};

#if !defined(SWIG) && !defined(RMF_DOXYGEN)
template <class Traits>
inline std::ostream& operator<<(std::ostream& out, Nullable<Traits> null) {
  null.show(out);
  return out;
}
#endif

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_NULLABLE_H */
