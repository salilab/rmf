/**
 *  \file RMF/traits.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_TRAITS_H
#define RMF_TRAITS_H

#include "RMF/config.h"
#include "RMF/types.h"
#include "RMF/HDF5/types.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cmath>
#include <cstdlib>
#include <limits>

RMF_ENABLE_WARNINGS

namespace RMF {

template <class T>
struct Traits {};

template <>
struct Traits<Int> {
  typedef Int Type;
  typedef Ints Types;
  typedef Type ReturnType;
  typedef Type ArgumentType;
  static bool get_is_null_value(const Type& t) { return t == get_null_value(); }
  static ReturnType get_null_value() {
    return std::numeric_limits<Type>::max();
  }
  typedef HDF5::IntTraits HDF5Traits;
  typedef boost::int32_t AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) { return a == b; }
  static std::string get_tag() { return "ki"; }
  static std::string get_name() { return "int"; }
};

typedef Traits<Int> IntTraits;

template <>
struct Traits<Float> {
  typedef Float Type;
  typedef Floats Types;
  typedef Type ReturnType;
  typedef Type ArgumentType;
  static bool get_is_null_value(const Type& t) {
    return t >= std::numeric_limits<Float>::max();
  }
  static ReturnType get_null_value() {
    return std::numeric_limits<Type>::infinity();
  }
  typedef HDF5::FloatTraits HDF5Traits;
  typedef double AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) {
    return std::abs(a - b) < .0000001 * std::abs(a + b) + .000000001;
  }
  static std::string get_tag() { return "kf"; }
  static std::string get_name() { return "float"; }
};
typedef Traits<Float> FloatTraits;

template <>
struct Traits<String> {
  typedef String Type;
  typedef Strings Types;
  typedef Type ReturnType;
  typedef const Type& ArgumentType;
  static bool get_is_null_value(const Type& t) { return t.empty(); }
  static ReturnType get_null_value() {
    static Type r;
    return r;
  }
  typedef HDF5::StringTraits HDF5Traits;
  typedef Type AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) { return a == b; }
  static std::string get_tag() { return "ks"; }
  static std::string get_name() { return "string"; }
};
typedef Traits<String> StringTraits;

template <>
struct Traits<Ints> {
  typedef Ints Type;
  typedef IntsList Types;
  typedef const Type& ReturnType;
  typedef const Type& ArgumentType;
  static bool get_is_null_value(const Type& t) { return t.empty(); }
  static ReturnType get_null_value() {
    static Type r;
    return r;
  }
  typedef HDF5::IntsTraits HDF5Traits;
  typedef std::vector<IntTraits::AvroType> AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) {
    if (a.size() != b.size()) return false;
    for (unsigned int i = 0; i < a.size(); ++i) {
      if (!IntTraits::get_are_equal(a[i], b[i])) return false;
    }
    return true;
  }
  static std::string get_tag() { return "kis"; }
  static std::string get_name() { return "ints"; }
};
typedef Traits<Ints> IntsTraits;

template <>
struct Traits<Floats> {
  typedef Floats Type;
  typedef FloatsList Types;
  typedef const Type& ReturnType;
  typedef const Type& ArgumentType;
  static bool get_is_null_value(const Type& t) { return t.empty(); }
  static ReturnType get_null_value() {
    static Type r;
    return r;
  }
  typedef HDF5::FloatsTraits HDF5Traits;
  typedef std::vector<double> AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) {
    if (a.size() != b.size()) return false;
    for (unsigned int i = 0; i < a.size(); ++i) {
      if (!FloatTraits::get_are_equal(a[i], b[i])) return false;
    }
    return true;
  }
  static std::string get_tag() { return "kfs"; }
  static std::string get_name() { return "floats"; }
};
typedef Traits<Floats> FloatsTraits;

template <>
struct Traits<Strings> {
  typedef Strings Type;
  typedef StringsList Types;
  typedef const Type& ReturnType;
  typedef const Type& ArgumentType;
  static bool get_is_null_value(const Type& t) { return t.empty(); }
  static ReturnType get_null_value() {
    static Type r;
    return r;
  }
  typedef HDF5::StringsTraits HDF5Traits;
  typedef Type AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) {
    if (a.size() != b.size()) return false;
    for (unsigned int i = 0; i < a.size(); ++i) {
      if (!StringTraits::get_are_equal(a[i], b[i])) return false;
    }
    return true;
  }
  static std::string get_tag() { return "kss"; }
  static std::string get_name() { return "strings"; }
};
typedef Traits<Strings> StringsTraits;


// swig gets confused
template <unsigned int D>
class Traits<Vector<D> > {
  static std::string make_tag() {
    std::ostringstream oss;
    oss << "v" << D;
    return oss.str();
  }
  static std::string make_name() {
    std::ostringstream oss;
    oss << "vector" << D;
    return oss.str();
  }

 public:
  typedef RMF_VECTOR<D> Type;
  typedef std::vector<RMF_VECTOR<D> > Types;
  typedef const Type& ReturnType;
  typedef const Type& ArgumentType;
  static bool get_is_null_value(const Type& t) {
    return t[0] > std::numeric_limits<double>::max();
  }
  static ReturnType get_null_value() {
    static const RMF_VECTOR<D> null(
        Floats(D, std::numeric_limits<typename FloatTraits::Type>::infinity()));
    return null;
  }
  typedef boost::int32_t AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) {
    for (unsigned int i = 0; i < D; ++i) {
      if (!FloatTraits::get_are_equal(a[i], b[i])) return false;
    }
    return true;
  }
  static std::string get_tag() {
    static std::string tag = make_tag();
    return tag;
  }

  static std::string get_name() {
    static std::string name = make_name();
    return name;
  }
};

typedef Traits<Vector<3> > Vector3Traits;
typedef Traits<Vector<4> > Vector4Traits;

template <unsigned int D>
struct Traits<std::vector<Vector<D> > > {
  typedef Vector3s Type;
  typedef std::vector<std::vector<RMF_VECTOR<3> > > Types;
  typedef const Type& ReturnType;
  typedef const Type& ArgumentType;
  static bool get_is_null_value(const Type& t) { return t.empty(); }
  static ReturnType get_null_value() {
    static const Type null;
    return null;
  }
  typedef boost::int32_t AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) {
    if (a.size() != b.size()) return false;
    for (unsigned int i = 0; i < a.size(); ++i) {
      if (!Traits<Vector<3> >::get_are_equal(a[i], b[i])) return false;
    }
    return true;
  }
  static std::string get_tag() {
    std::ostringstream oss;
    oss << "vs" << 3;
    return oss.str();
  }
  static std::string get_name() { return "vector3s"; }
};

typedef Traits<std::vector<Vector<3> > > Vector3sTraits;
typedef Traits<std::vector<Vector<4> > > Vector4sTraits;

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_TRAITS_H */
