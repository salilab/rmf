/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "RMF/decorator/alternatives.h"
#include "RMF/decorator/physics.h"

RMF_ENABLE_WARNINGS

namespace RMF {
namespace decorator {
AlternativesFactory::AlternativesFactory(FileHandle fh)
    : cat_(fh.get_category("alternatives")),
      types_key_(fh.get_key<IntsTraits>(cat_, "types")),
      roots_key_(fh.get_key<IntsTraits>(cat_, "roots")) {}

AlternativesFactory::AlternativesFactory(FileConstHandle fh)
    : cat_(fh.get_category("alternatives")),
      types_key_(fh.get_key<IntsTraits>(cat_, "types")),
      roots_key_(fh.get_key<IntsTraits>(cat_, "roots")) {}

namespace {
double get_resolution_metric(double a, double b) {
  if (a < b) std::swap(a, b);
  return a / b - 1;
}

NodeID get_alternative_impl(NodeConstHandle cur, IntsKey types_key,
                            IntsKey roots_key, RepresentationType type,
                            float resolution) {
  RMF_USAGE_CHECK(type == PARTICLE, "Only particle type supported currently");
  if (!cur.get_has_value(types_key)) return cur.get_id();

  double closest_resolution = get_resolution(cur);
  int closest_index = -1;
  Nullable<IntsTraits> types = cur.get_value(types_key);
  if (!types.get_is_null()) {
    Ints roots = cur.get_value(roots_key);
    for (unsigned int i = 0; i < types.get().size(); ++i) {
      if (types.get()[i] != type) continue;
      double cur_resolution =
          get_resolution(cur.get_file().get_node(NodeID(roots[i])));
      if (get_resolution_metric(resolution, cur_resolution) <
          get_resolution_metric(resolution, closest_resolution)) {
        closest_index = i;
        closest_resolution = cur_resolution;
      }
    }
  }
  if (closest_index == -1) {
    return cur.get_id();
  } else {
    return NodeID(cur.get_value(roots_key).get()[closest_index]);
  }
}

NodeIDs get_alternatives_impl(NodeConstHandle cur, IntsKey roots_key) {
  NodeIDs ret(1, cur.get_id());

  if (cur.get_has_value(roots_key)) {
    RMF_FOREACH(int i, cur.get_value(roots_key).get()) {
      ret.push_back(NodeID(i));
    }
  }
  return ret;
}

std::pair<double, double> get_resolution_impl(
    NodeConstHandle root, IntermediateParticleFactory ipcf) {
  std::pair<double, double> ret(0.0, 0.0);
  RMF_FOREACH(NodeConstHandle ch, root.get_children()) {
    std::pair<double, double> cur = get_resolution_impl(ch, ipcf);
    ret.first += cur.first;
    ret.second += cur.second;
  }
  if (ret.second == 0 && ipcf.get_is(root)) {
    ret.first = ipcf.get(root).get_radius();
    ret.second = 1.0;
  }
  return ret;
}

}

double get_resolution(NodeConstHandle root) {
  IntermediateParticleFactory ipcf(root.get_file());
  std::pair<double, double> total = get_resolution_impl(root, ipcf);
  RMF_USAGE_CHECK(total.first != 0,
                  std::string("No particles were found at ") + root.get_name());
  return total.second / total.first;
}

Alternatives::Alternatives(NodeHandle nh, IntsKey types_key, IntsKey roots_key)
    : AlternativesConst(nh, types_key, roots_key) {}

void Alternatives::add_alternative(NodeHandle root, RepresentationType type) {
  get_node()
      .get_shared_data()
      ->access_static_value(get_node().get_id(), types_key_)
      .push_back(type);
  get_node()
      .get_shared_data()
      ->access_static_value(get_node().get_id(), roots_key_)
      .push_back(root.get_id().get_index());
}

NodeConstHandle AlternativesConst::get_alternative(RepresentationType type,
                                                   double resolution) const {
  return get_node().get_file().get_node(get_alternative_impl(
      get_node(), types_key_, roots_key_, type, resolution));
}

NodeConstHandles AlternativesConst::get_alternatives(RepresentationType type)
    const {
  RMF_USAGE_CHECK(type == PARTICLE, "Only particles supported");
  NodeConstHandles ret;
  RMF_FOREACH(NodeID nid,
              get_alternatives_impl(get_node(), types_key_)) {
    ret.push_back(get_node().get_file().get_node(nid));
  }
  return ret;
}

RepresentationType AlternativesConst::get_representation_type(NodeID id) const {
  if (id == get_node().get_id()) {
    return PARTICLE;
  }
  Ints roots = get_node().get_value(roots_key_);
  for (unsigned int i = 0; i < roots.size(); ++i) {
    if (roots[i] == static_cast<int>(id.get_index())) {
      return RepresentationType(get_node().get_value(types_key_).get()[i]);
    }
  }
  RMF_THROW(Message("No such alternative representation"), UsageException);
}

namespace {
Floats get_resolutions_impl(NodeConstHandle root, AlternativesFactory af,
                            RepresentationType type) {
  Floats ret;
  if (af.get_is(root)) {
    RMF_FOREACH(NodeConstHandle a, af.get(root).get_alternatives(type)) {
      ret.push_back(get_resolution(a));
    }
  } else {
    RMF_FOREACH(NodeConstHandle ch, root.get_children()) {
      Floats cur = get_resolutions_impl(ch, af, type);
      ret.insert(ret.end(), cur.begin(), cur.end());
    }
  }
  return ret;
}
}

Floats get_resolutions(NodeConstHandle root, RepresentationType type,
                       double accuracy) {
  AlternativesFactory af(root.get_file());
  Floats unclustered = get_resolutions_impl(root, af, type);
  if (unclustered.empty()) unclustered.push_back(1.0);
  std::sort(unclustered.begin(), unclustered.end());
  double lb = unclustered[0];
  double ub = lb;
  Floats ret;
  RMF_FOREACH(double r, unclustered) {
    if (r > lb + accuracy) {
      ret.push_back(.5 * (lb + ub));
      lb = r;
    }
    ub = r;
  }
  ret.push_back(.5 * (lb + ub));
  return ret;
}

} /* namespace decorator */
} /* namespace RMF */

RMF_DISABLE_WARNINGS
