/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "RMF/TraverseHelper.h"

RMF_ENABLE_WARNINGS

namespace RMF {
TraverseHelper::TraverseHelper(NodeConstHandle root, std::string molecule_name,
                               double resolution, int state_filter)
    : active_(boost::make_shared<Index>()),
      chain_factory_(root.get_file()),
      residue_factory_(root.get_file()),
      reference_frame_factory_(root.get_file()),
      colored_factory_(root.get_file()),
      alternatives_factory_(root.get_file()),
      state_factory_(root.get_file()),
      copy_factory_(root.get_file()),
      state_filter_(state_filter),
      color_(Vector3Traits::get_null_value()),
      residue_index_(IntTraits::get_null_value()),
      residue_type_(StringTraits::get_null_value()),
      chain_id_(StringTraits::get_null_value()),
      molecule_name_(molecule_name),
      state_(0),
      copy_index_(IntTraits::get_null_value()),
      resolution_(resolution) {
  visit_impl(root);
}

void TraverseHelper::visit_impl(NodeConstHandle n) {
  if (state_factory_.get_is(n)) {
    int state = state_factory_.get(n).get_state_index();
    state_ = state;
  }
  if (alternatives_factory_.get_is(n)) {
    NodeConstHandle nh =
        alternatives_factory_.get(n).get_alternative(PARTICLE, resolution_);
    static_cast<NodeConstHandle&>(*this) = nh;
  } else {
    static_cast<NodeConstHandle&>(*this) = n;
  }

  if (reference_frame_factory_.get_is(*this)) {
    coordinate_transformer_ = CoordinateTransformer(
        coordinate_transformer_, reference_frame_factory_.get(*this));
  }
  if (colored_factory_.get_is(*this)) {
    color_ = colored_factory_.get(*this).get_rgb_color();
  }
  if (residue_factory_.get_is(*this)) {
    residue_index_ = residue_factory_.get(*this).get_residue_index();
    residue_type_ = residue_factory_.get(*this).get_residue_type();
  }
  if (chain_factory_.get_is(*this)) {
    chain_id_ = chain_factory_.get(*this).get_chain_id();
  }
  if (copy_factory_.get_is(*this)) {
    copy_index_ = copy_factory_.get(*this).get_copy_index();
  }
}

TraverseHelper TraverseHelper::visit(NodeConstHandle n) const {
  TraverseHelper ret = *this;
  ret.visit_impl(n);
  return ret;
}

unsigned int TraverseHelper::set_is_displayed() {
  unsigned int i = active_->size();
  active_->insert(std::make_pair(get_id(), i));
  return i;
}

unsigned int TraverseHelper::get_index(NodeID n) const {
  return active_->find(n)->second;
}

std::vector<TraverseHelper> TraverseHelper::get_children() const {
  std::vector<TraverseHelper> ret;
  // handle alternatives later
  RMF_FOREACH(NodeConstHandle ch, NodeConstHandle::get_children()) {
    if (state_filter_ != -1 && state_factory_.get_is(ch) &&
        state_factory_.get(ch).get_state_index() != state_filter_)
      continue;
    ret.push_back(visit(ch));
  }
  return ret;
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
