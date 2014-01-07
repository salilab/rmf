/**
 *  \file RMF/decorator/alternatives.h
 *  \brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_ALTERNATIVES_DECORATORS_H
#define RMF_ALTERNATIVES_DECORATORS_H

#include <RMF/config.h>
#include <RMF/infrastructure_macros.h>
#include <RMF/NodeHandle.h>
#include <RMF/FileHandle.h>
#include <RMF/Decorator.h>
#include <RMF/constants.h>
#include <RMF/Vector.h>
#include <RMF/internal/utility.h>
#include <RMF/internal/paths.h>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

RMF_ENABLE_WARNINGS
namespace RMF {
namespace decorator {

/** The different ways of representing a structure. */
enum RepresentationType {
  /** Representation via particles, the default. */
  PARTICLE = 0,
  /** Representation via arbitrary geometry. */
  SHAPE = 1,
  SURFACE = 2,
};

/** See also Alternatives and AlternativesConstFactory.
  */
class AlternativesConst : public Decorator<NodeConstHandle> {
  friend class AlternativesConstFactory;
  FloatKey base_resolution_key_;
  IntsKey types_key_;
  IntsKey roots_key_;
  FloatsKey resolutions_key_;
  typedef Decorator<NodeConstHandle> P;
  AlternativesConst(NodeConstHandle nh, FloatKey base_resolution_key,
                    IntsKey types_key, IntsKey roots_key,
                    FloatsKey resolutions_key)
      : P(nh),
        base_resolution_key_(base_resolution_key),
        types_key_(types_key),
        roots_key_(roots_key),
        resolutions_key_(resolutions_key) {}

 public:
  /** Get the alternative root that best matches the criteria. */
  NodeConstHandle get_alternative(RepresentationType type,
                                  double resolution) const;

  /** Get the resolution of a given node associated with this one.*/
  float get_resolution(NodeID id) const;

  /** Get the type of the resolution with the given node id. */
  RepresentationType get_representation_type(NodeID id) const;

  /** Get all the alternatives (including this node).

     You can use get_resolution() and get_representation_type() to get info
     about them. */
  NodeConstHandles get_alternatives(RepresentationType type) const;

  static std::string get_decorator_type_name() { return "AlternativesConst"; }
};

/** See also AlternativesConst and AlternativesFactory.
  */
class Alternatives : public Decorator<NodeHandle> {
  friend class AlternativesFactory;
  FloatKey base_resolution_key_;
  IntsKey types_key_;
  IntsKey roots_key_;
  FloatsKey resolutions_key_;
  typedef Decorator<NodeHandle> P;
  IntKey alternativesed_;
  Alternatives(NodeHandle nh, FloatKey base_resolution_key, IntsKey types_key,
               IntsKey roots_key, FloatsKey resolutions_key);

 public:
  /** \copydoc AlternativesConst::get_alternative() */
  NodeHandle get_alternative(RepresentationType type, double resolution) const;

  void add_alternative(NodeHandle root, RepresentationType type);

  /** \copydoc AlternativesConst::get_alternatives() */
  NodeHandles get_alternatives(RepresentationType type) const;

  static std::string get_decorator_type_name() { return "Alternatives"; }
};

/** Create decorators of type Alternatives.

     See also AlternativesConst and AlternativesFactory.
  */
class AlternativesConstFactory {
  Category cat_;
  FloatKey base_resolution_key_;
  IntsKey types_key_;
  IntsKey roots_key_;
  FloatsKey resolutions_key_;
 public:
  AlternativesConstFactory(FileConstHandle fh);

  AlternativesConst get(NodeConstHandle nh) const {
    return AlternativesConst(nh, base_resolution_key_, types_key_, roots_key_,
                             resolutions_key_);
  }
  /** Check whether nh has all the attributes required to be a
      AlternativesConst.*/
  bool get_is(NodeConstHandle nh) const {
    return nh.get_has_value(base_resolution_key_);
  }
  bool get_is_static(NodeConstHandle nh) const {
    return nh.get_has_value(base_resolution_key_);
  }
};

/** Create decorators of type Alternatives.

     See also Alternatives and AlternativesConstFactory.
  */
class AlternativesFactory {
  Category cat_;
  FloatKey base_resolution_key_;
  IntsKey types_key_;
  IntsKey roots_key_;
  FloatsKey resolutions_key_;
 public:
  AlternativesFactory(FileHandle fh);

  Alternatives get(NodeHandle nh) const {
    return Alternatives(nh, base_resolution_key_, types_key_, roots_key_,
                        resolutions_key_);
  }
  bool get_is(NodeHandle nh) const {
    return nh.get_has_value(base_resolution_key_);
  }
  bool get_is_static(NodeHandle nh) const {
    return nh.get_has_value(base_resolution_key_);
  }
};

} /* namespace decorator */
} /* namespace RMF */
RMF_DISABLE_WARNINGS

#endif /* RMF_ALTERNATIVES_DECORATORS_H */
