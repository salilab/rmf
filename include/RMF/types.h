/**
 *  \file RMF/types.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_TYPES_H
#define RMF_TYPES_H

#include <RMF/config.h>
#include "ID.h"
#include "Vector.h"
#include "infrastructure_macros.h"
#include "internal/errors.h"
#include <RMF/HDF5/types.h>

#include <algorithm>
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cmath>
#include <cstdlib>
#include <limits>

RMF_ENABLE_WARNINGS

namespace RMF {

/** The type used to store integral values.*/
typedef int Int;
/** The type used to store lists of integral values.*/
typedef std::vector<Int> Ints;
/** The type used to store lists of floating point values.*/
typedef float Float;
/** The type used to store lists of floating point values.*/
typedef std::vector<Float> Floats;
/** The type used to store lists of floating point values.*/
typedef std::vector<Floats> FloatsList;
/** The type used to store lists of index values.*/
typedef int Index;
/** The type used to store lists of index values.*/
typedef std::vector<Index> Indexes;
/** The type used to store lists of indexes.*/
typedef std::vector<Indexes> IndexesList;
/** The type used to store lists of string values.*/
typedef std::string String;
/** The type used to store lists of string values.*/
typedef std::vector<String> Strings;
/** The type used to store lists of strings values.*/
typedef std::vector<Strings> StringsList;
/** The type used to store lists of NodeID values.*/
typedef std::vector<NodeIDs> NodeIDsList;
/** The type used to store lists of lists of integers values.*/
typedef std::vector<Ints> IntsList;
/** The type used to store char values.*/
typedef char Char;
/** The type used to store lists of char values.*/
typedef std::string Chars;

typedef std::pair<Index, Index> IndexRange;
typedef std::pair<Int, Int> IntRange;

struct IntTraits {
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
};
struct FloatTraits {
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
};
struct StringTraits {
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
};
struct IntsTraits {
  typedef Ints Type;
  typedef IntsList Types;
  typedef Type ReturnType;
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
};
struct FloatsTraits {
  typedef Floats Type;
  typedef FloatsList Types;
  typedef Type ReturnType;
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
};
struct StringsTraits {
  typedef Strings Type;
  typedef StringsList Types;
  typedef Type ReturnType;
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
};

template <unsigned int D>
struct VectorTraits {
  typedef Vector<D> Type;
  typedef std::vector<Vector<D> > Types;
  typedef Type ReturnType;
  typedef Type ArgumentType;
  static bool get_is_null_value(const Type& t) {
    return t[0] > std::numeric_limits<double>::max();
  }
  static ReturnType get_null_value() {
    return Vector<D>(
        Floats(D, std::numeric_limits<typename FloatTraits::Type>::infinity()));
  }
  typedef boost::int32_t AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) {
    for (unsigned int i = 0; i < D; ++i) {
      if (!FloatTraits::get_are_equal(a[i], b[i])) return false;
    }
    return true;
  }
  static std::string get_tag() {
    std::ostringstream oss;
    oss << "v" << D;
    return oss.str();
  }
};

typedef VectorTraits<3> Vector3Traits;
typedef VectorTraits<4> Vector4Traits;

  //template <unsigned int D>
struct Vector3sTraits {
  typedef Vector3s Type;
  typedef std::vector<std::vector<Vector<3> > > Types;
  typedef Type ReturnType;
  typedef Type ArgumentType;
  static bool get_is_null_value(const Type& t) {
    return t.empty();
  }
  static ReturnType get_null_value() {
    return Type();
  }
  typedef boost::int32_t AvroType;
  static bool get_are_equal(ArgumentType a, ArgumentType b) {
    if (a.size() != b.size()) return false;
    for (unsigned int i = 0; i < a.size(); ++i) {
      if (!VectorTraits<3>::get_are_equal(a[i], b[i])) return false;
    }
    return true;
  }
  static std::string get_tag() {
    std::ostringstream oss;
    oss << "vs" << 3;
    return oss.str();
  }
};

  //typedef VectorsTraits<3> Vector3sTraits;
  //typedef VectorsTraits<4> Vector4sTraits;

/** Get one type as another, handling vectors or scalars.*/
template <class OutType, class InType>
inline OutType get_as(InType in) {
  return OutType(in);
}
/** NodeIDs require translation.*/
template <class Out>
inline Out get_as(NodeID ni) {
  if (ni == NodeID())
    return Out(-1);
  else
    return Out(ni.get_index());
}

template <>
inline NodeID get_as(int i) {
  if (i == -1)
    return NodeID();
  else
    return NodeID(i);
}

/** Get one type as another, handling vectors or scalars.*/
template <class OutType, class InType>
inline OutType get_as(const std::vector<InType> in) {
  OutType ret(in.size());
  for (unsigned int i = 0; i < ret.size(); ++i) {
    ret[i] = get_as<typename OutType::value_type>(in[i]);
  }
  return ret;
}

class BondEndpoints
#ifndef SWIG
    : public boost::array<NodeID, 2>
#endif
      {
  typedef boost::array<NodeID, 2> P;

 public:
  BondEndpoints(NodeID a, NodeID b) {
    P::operator[](0) = a;
    P::operator[](1) = b;
  }
  unsigned int __len__() const { return 2; }
  NodeID __getitem__(unsigned int i) const {
    if (i < 2)
      return P::operator[](i);
    else
      RMF_THROW(Message("Out of bounds"), UsageException);
  }
};

#define RMF_DECLARE_KEY(lcname, Ucname, PassValue, ReturnValue, PassValues, \
                        ReturnValues)                                       \
  typedef ID<Ucname##Traits> Ucname##Key;                                   \
  typedef std::vector<Ucname##Key> Ucname##Keys;

/** \name Key types
    RMF files support storing a variety of different types of data. These
    include
    - IntKey: store an arbitrary integher as a 64 bit integer
    - FloatKey: store an arbitrary floating point number as a double
    - StringKey: store an arbitrary length string
    - IndexKey: store non-negative indexes as 64 bit integers
    - NodeIDKey: store the ID of a node in the hierarchy
    - NodeIDsKey: store a list of NodeIDs
    - DataSetKey: store a reference to an HDF5 data set via the path to the
    data set
    - IntsKey: store a list of arbitrary integers
    @{
 */
RMF_FOREACH_TYPE(RMF_DECLARE_KEY);
/** @} */

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_TYPES_H */
