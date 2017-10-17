/**
 *  \file RMF/decorator/provenance.h
 *  \brief Add a pointer from a node to another provenance node.
 *
 *  Copyright 2007-2017 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_DECORATOR_PROVENANCE_H
#define RMF_DECORATOR_PROVENANCE_H

#include <RMF/config.h>
#include <RMF/infrastructure_macros.h>
#include <RMF/NodeHandle.h>
#include <RMF/FileHandle.h>
#include <RMF/Decorator.h>
#include <RMF/constants.h>
#include <RMF/Vector.h>
#include <RMF/decorator/provenance_types.h>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

RMF_ENABLE_WARNINGS
namespace RMF {
namespace decorator {

/** See also Provenance and ProvenanceFactory.
  */
class ProvenanceConst : public Decorator {
  friend class ProvenanceFactory;
  friend class Provenance;
  IntKey provenance_;
  ProvenanceConst(NodeConstHandle nh, IntKey provenance)
      : Decorator(nh), provenance_(provenance) {}

 public:
  NodeConstHandle get_provenance() const {
    try {
      return get_node().get_file().get_node(
          NodeID(get_node().get_value(provenance_)));
    }
    RMF_DECORATOR_CATCH();
  }
  NodeConstHandle get_frame_provenance() const {
    try {
      return get_node().get_file().get_node(
          NodeID(get_node().get_frame_value(provenance_)));
    }
    RMF_DECORATOR_CATCH();
  }
  NodeConstHandle get_static_provenance() const {
    try {
      return get_node().get_file().get_node(
          NodeID(get_node().get_static_value(provenance_)));
    }
    RMF_DECORATOR_CATCH();
  }

  static std::string get_decorator_type_name() { return "ProvenanceConst"; }
  RMF_SHOWABLE(ProvenanceConst, "Provenance: " << get_node());
};
/** See also ProvenanceFactory.
 */
class Provenance : public ProvenanceConst {
  friend class ProvenanceFactory;
  Provenance(NodeHandle nh, IntKey provenance)
      : ProvenanceConst(nh, provenance) {}

 public:

  NodeHandle get_provenance() const {
    try {
      return get_node().get_file().get_node(
          NodeID(get_node().get_value(provenance_)));
    }
    RMF_DECORATOR_CATCH();
  }
  NodeHandle get_frame_provenance() const {
    try {
      return get_node().get_file().get_node(
          NodeID(get_node().get_frame_value(provenance_)));
    }
    RMF_DECORATOR_CATCH();
  }
  NodeHandle get_static_provenance() const {
    try {
      return get_node().get_file().get_node(
          NodeID(get_node().get_static_value(provenance_)));
    }
    RMF_DECORATOR_CATCH();
  }

  void set_provenance(NodeConstHandle v) {
    try {
      get_node().set_value(provenance_, v.get_id().get_index());
    }
    RMF_DECORATOR_CATCH();
  }
  void set_frame_provenance(NodeConstHandle v) {
    try {
      get_node().set_frame_value(provenance_, v.get_id().get_index());
    }
    RMF_DECORATOR_CATCH();
  }
  void set_static_provenance(NodeConstHandle v) {
    try {
      get_node().set_static_value(provenance_, v.get_id().get_index());
    }
    RMF_DECORATOR_CATCH();
  }

  static std::string get_decorator_type_name() { return "Provenance"; }
};

/** Create decorators of type Provenance.
  */
class ProvenanceFactory : public Factory {
  Category cat_;
  IntKey provenance_;

 public:
  ProvenanceFactory(FileConstHandle fh)
      : cat_(fh.get_category("sequence")),
        provenance_(fh.get_key<IntTag>(cat_, "provenance")) {}
  ProvenanceFactory(FileHandle fh)
      : cat_(fh.get_category("sequence")),
        provenance_(fh.get_key<IntTag>(cat_, "provenance")) {}
  /** Get a ProvenanceConst for nh.*/
  ProvenanceConst get(NodeConstHandle nh) const {
    RMF_USAGE_CHECK((nh.get_type() == RMF::REPRESENTATION),
                    std::string("Bad node type. Got \"") +
                        boost::lexical_cast<std::string>(nh.get_type()) +
                        "\" in decorator type Provenance");
    return ProvenanceConst(nh, provenance_);
  }
  /** Get a Provenance for nh.*/
  Provenance get(NodeHandle nh) const {
    RMF_USAGE_CHECK((nh.get_type() == RMF::REPRESENTATION),
                    std::string("Bad node type. Got \"") +
                        boost::lexical_cast<std::string>(nh.get_type()) +
                        "\" in decorator type Provenance");
    return Provenance(nh, provenance_);
  }
  /** Check whether nh has all the attributes required to be a
      ProvenanceConst.*/
  bool get_is(NodeConstHandle nh) const {
    return (nh.get_type() == RMF::REPRESENTATION) &&
           !nh.get_value(provenance_).get_is_null();
  }
  bool get_is_static(NodeConstHandle nh) const {
    return (nh.get_type() == RMF::REPRESENTATION) &&
           !nh.get_static_value(provenance_).get_is_null();
  }
  RMF_SHOWABLE(ProvenanceFactory, "ProvenanceFactory");
};

} /* namespace decorator */
} /* namespace RMF */
RMF_DISABLE_WARNINGS

#endif /* RMF_DECORATOR_PROVENANCE_H */
