/**
 *  \file RMF/TraverseHelper.h
 *  \brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_TRAVERSE_HELPER_H
#define RMF_TRAVERSE_HELPER_H

#include <RMF/config.h>
#include "CoordinateTransformer.h"
#include "Nullable.h"
#include <RMF/decorator/physics.h>
#include <RMF/decorator/sequence.h>
#include <RMF/decorator/shape.h>
#include <RMF/decorator/alternatives.h>

RMF_ENABLE_WARNINGS

namespace RMF {

class TraverseHelper;
typedef std::vector<TraverseHelper> TraverseHelpers;

/** This class tracks common data that one needs to keep as one traverses the
    hierarchy.

    Feel free to request that other data types be added.

    \note In the case of non-default resolution with alternatives nodes, the
    node this inherits from may not be the one you visited. So you should just
    pass this object when you need to access the node.

*/
class RMFEXPORT TraverseHelper : public NodeConstHandle {
  struct Index : public RMF_LARGE_UNORDERED_MAP<NodeID, unsigned int> {};
  boost::shared_ptr<Index> active_;
  decorator::ChainFactory chain_factory_;
  decorator::ResidueFactory residue_factory_;
  decorator::ReferenceFrameFactory reference_frame_factory_;
  decorator::ColoredFactory colored_factory_;
  decorator::AlternativesFactory alternatives_factory_;
  decorator::StateFactory state_factory_;
  decorator::CopyFactory copy_factory_;
  int state_filter_;
  CoordinateTransformer coordinate_transformer_;
  Vector3 color_;
  int residue_index_;
  std::string residue_type_;
  std::string chain_id_;
  std::string molecule_name_;
  unsigned int state_;
  int copy_index_;
  double resolution_;

  void visit_impl(NodeConstHandle n);

 /** Return an updated TraverseHelper after inspecting the passed
      node. */
  TraverseHelper visit(NodeConstHandle n) const;

 public:
  TraverseHelper(NodeConstHandle root, std::string molecule_name,
                 double resolution = 10000, int state_filter = -1);

  /** Get the current chain id or None. */
  Nullable<StringTraits> get_chain_id() const {
    return Nullable<StringTraits>(chain_id_);
  }
  /** Get the current residue index or None. */
  Nullable<IntTraits> get_residue_index() const {
    return Nullable<IntTraits>(residue_index_);
  }
  /** Get the current residue type or None. */
  Nullable<StringTraits> get_residue_type() const {
    return Nullable<StringTraits>(residue_type_);
  }
  /** Get the current molecule name or None. */
  std::string get_molecule_name() const { return molecule_name_; }
  /** Get the current color or None. */
  Nullable<Vector3Traits> get_rgb_color() const {
    return Nullable<Vector3Traits>(color_);
  }
  /** Get the current state or 0. */
  unsigned int get_state_index() const { return state_; }
  /** Get the current copy inde or None. */
  Nullable<IntTraits> get_copy_index() const {
    return Nullable<IntTraits>(copy_index_);
  }
  Vector3 get_global_coordinates(const Vector3 &v) {
    return coordinate_transformer_.get_global_coordinates(v);
  }
  /** Set that the current node is dispayed and return its index. */
  unsigned int set_is_displayed();

  bool get_is_displayed(NodeID n) { return active_->find(n) != active_->end(); }

  /** Return a unique id for the current particle. */
  unsigned int get_index(NodeID n) const;

  /** Return other nodes to traverse.*/
  TraverseHelpers get_children() const;
};


} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_TRAVERSE_HELPER_H */
