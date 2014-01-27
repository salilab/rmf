/**
 *  \file rmfplugin.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_RMFPLUGIN_H
#define RMF_RMFPLUGIN_H

#include "RMF/CoordinateTransformer.h"
#include "RMF/FileConstHandle.h"
#include "RMF/ID.h"
#include "RMF/NodeConstHandle.h"
#include "RMF/Vector.h"
#include "RMF/decorator/alternatives.h"
#include "RMF/decorator/bond.h"
#include "RMF/decorator/feature.h"
#include "RMF/decorator/physics.h"
#include "RMF/decorator/representation.h"
#include "RMF/decorator/sequence.h"
#include "RMF/decorator/shape.h"
#include "RMF/enums.h"
#include "RMF/exceptions.h"
#include "RMF/infrastructure_macros.h"
#include "RMF/log.h"
#include "RMF/types.h"
#include "molfile_plugin.h"
#include <algorithm>
#include <boost/range/distance.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/scoped_array.hpp>
#include <iostream>
#include <stddef.h>
#include <string>
#include <utility>
#include <vector>

namespace {
molfile_plugin_t plugin;
molfile_plugin_t plugin3;
molfile_plugin_t pluginz;

template <class R, class OIt>
void my_copy_n(const R &range, std::size_t n, OIt out) {
  if (boost::distance(range) <= n) {
    // older boost doesn't have range copy
    std::copy(range.begin(), range.end(), out);
  } else {
    std::copy(range.begin(), range.begin() + n, out);
  }
}

class Data {
  RMF::FileConstHandle file_;
  RMF::decorator::AtomFactory af_;
  RMF::decorator::ResidueFactory rf_;
  RMF::decorator::ChainFactory chf_;
  RMF::decorator::ParticleFactory pf_;
  RMF::decorator::ReferenceFrameFactory rff_;
  RMF::decorator::BallFactory bf_;
  RMF::decorator::SegmentFactory sf_;
  RMF::decorator::CylinderFactory cf_;
  RMF::decorator::BondFactory bdf_;
  RMF::decorator::ScoreFactory scf_;
  RMF::decorator::RepresentationFactory rcf_;
  RMF::decorator::DomainFactory df_;
  RMF::decorator::FragmentFactory ff_;
  RMF::decorator::CopyFactory cpf_;
  RMF::decorator::TypedFactory tf_;
  RMF::decorator::AlternativesFactory altf_;
  RMF::decorator::StateFactory stf_;
  struct AtomInfo {
    // can precumpte the actual molfile_atom_t data to simplify things
    boost::array<char, 2> chain_id;
    int residue_index;
    boost::array<char, 8> residue_name;
    boost::array<char, 2> altid;
    boost::array<char, 8> segment;
    RMF::NodeID node_id;
  };
  struct Body {
    std::vector<RMF::decorator::ReferenceFrameConst> frames;
    std::vector<AtomInfo> particles;
    std::vector<AtomInfo> balls;
    int state;
    Body() : state(0) {}
  };
  std::vector<Body> bodies_;
  boost::unordered_map<RMF::NodeID, int> index_;
  std::vector<molfile_graphics_t> graphics_;
  std::vector<int> bond_to_, bond_from_, bond_type_;
  boost::scoped_array<char> bond_type_name_, restraint_bond_type_name_;
  std::vector<char *> bond_type_names_;
  double resolution_;
  enum ShowRestraints {
    BONDS = 1,
    RESTRAINTS = 2
  };
  int show_restraints_;
  RMF::Vector3 lower_bounds_;
  RMF::Vector3 upper_bounds_;
  double max_radius_;
  bool done_;

  // find nodes to push to vmd
  boost::array<int, 2> fill_bodies(RMF::NodeConstHandle cur, int body,
                                   boost::array<char, 2> chain, int resid,
                                   boost::array<char, 8> resname,
                                   boost::array<char, 2> altid,
                                   boost::array<char, 8> segment,
                                   double resolution);
  void fill_index();
  void fill_bounds();
  void fill_graphics(RMF::NodeConstHandle cur, RMF::CoordinateTransformer tr);
  void fill_bonds(RMF::NodeConstHandle cur);
  int handle_reference_frame(int body, RMF::NodeConstHandle cur);
  int handle_state(int body, RMF::NodeConstHandle cur);
  boost::tuple<RMF::NodeConstHandle, boost::array<char, 2>,
               boost::array<int, 2> >
      handle_alternative(RMF::NodeConstHandle cur, int body,
                         boost::array<char, 2> chain, int resid,
                         boost::array<char, 8> resname,
                         boost::array<char, 2> altid,
                         boost::array<char, 8> segment, double resolution);
  void handle_bond(RMF::NodeConstHandle cur);
  void handle_restraint(RMF::NodeConstHandle cur);
  double get_resolution();
  int get_show_restraints();

  void copy_basics(const AtomInfo &ai, molfile_atom_t *out);
  molfile_atom_t *copy_particles(const std::vector<AtomInfo> &atoms,
                                 molfile_atom_t *out);
  molfile_atom_t *copy_balls(const std::vector<AtomInfo> &balls,
                             molfile_atom_t *out);

 public:
  Data(std::string name, int *num_atoms);
  int read_structure(int *optflags, molfile_atom_t *atoms);
  int read_timestep(molfile_timestep_t *frame);
  int read_graphics(int *nelem, const molfile_graphics_t **gdata);
  int read_bonds(int *nbonds, int **fromptr, int **toptr, float **bondorderptr,
                 int **bondtype, int *nbondtypes, char ***bondtypename);
  int read_timestep_metadata(molfile_timestep_metadata_t *data);
};

Data::Data(std::string name, int *num_atoms)
    : file_(RMF::open_rmf_file_read_only(name)),
      af_(file_),
      rf_(file_),
      chf_(file_),
      pf_(file_),
      rff_(file_),
      bf_(file_),
      sf_(file_),
      cf_(file_),
      bdf_(file_),
      scf_(file_),
      rcf_(file_),
      df_(file_),
      ff_(file_),
      cpf_(file_),
      tf_(file_),
      altf_(file_),
      stf_(file_),
      resolution_(get_resolution()),
      show_restraints_(get_show_restraints()),
      lower_bounds_(std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max()),
      upper_bounds_(-std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max()),
      max_radius_(0),
      done_(false) {
  if (file_.get_number_of_frames() > 0) {
    file_.set_current_frame(RMF::FrameID(0));
  }

  bodies_.push_back(Body());

  boost::array<char, 2> default_chain = {0};
  boost::array<char, 8> default_resname = {0};
  boost::array<char, 2> default_altid = {0};
  boost::array<char, 8> default_segment = {0};
  boost::array<int, 2> na =
      fill_bodies(file_.get_root_node(), 0, default_chain, -1, default_resname,
                  default_altid, default_segment, resolution_);
  fill_index();
  *num_atoms = na[0] + na[1];
  if (*num_atoms == 0) {
    *num_atoms = MOLFILE_NUMATOMS_NONE;
  }
  std::cout << "RMF: found " << *num_atoms << " atoms." << std::endl;

  fill_bounds();
}

double Data::get_resolution() {
  RMF::Floats resolutions = RMF::decorator::get_resolutions(
      file_.get_root_node(), RMF::decorator::PARTICLE, .1);
  if (resolutions.size() > 1) {
    std::cout << "RMF: Resolutions are " << RMF::Showable(resolutions)
              << ".\nPlease enter desired resolution (or -1 for all): "
              << std::flush;
    double r = -1;
    std::cin >> r;
    std::cout << "Using resolution " << r << std::endl;
    // fix divide by zero
    if (r == 0)
      return .0001;
    else
      return r;
  }
  return 1;
}

int Data::get_show_restraints() {
  bool has_restraints = false;
  RMF_FOREACH(RMF::NodeID n, file_.get_node_ids()) {
    if (rcf_.get_is(file_.get_node(n))) {
      has_restraints = true;
      break;
    }
  }
  if (has_restraints) {
    std::cout << "RMF: File has restraints. Please choose what to display.\n"
              << "0 for just bonds, 1 for just restraints or 2 for both:"
              << std::flush;
    int r = -1;
    std::cin >> r;
    switch (r) {
      case 0:
        return BONDS;
      case 1:
        return RESTRAINTS;
      default:
        return BONDS | RESTRAINTS;
    }
  } else {
    std::cout << "No restraints found in file." << std::endl;
    return BONDS;
  }
}

void Data::fill_bounds() {
  RMF_FOREACH(RMF::NodeID id, file_.get_node_ids()) {
    if (pf_.get_is(file_.get_node(id))) {
      RMF::decorator::ParticleConst pc = pf_.get(file_.get_node(id));
      double r = pc.get_radius();
      max_radius_ = std::max<double>(r, max_radius_);
      RMF::Vector3 c = pc.get_coordinates();
      for (unsigned int i = 0; i < 3; ++i) {
        upper_bounds_[i] = std::max<double>(upper_bounds_[i], c[i] + r);
        lower_bounds_[i] = std::min<double>(lower_bounds_[i], c[i] + r);
      }
    }
  }
}

int Data::handle_reference_frame(int body, RMF::NodeConstHandle cur) {
  bodies_.push_back(Body());
  bodies_.back().frames = bodies_[body].frames;
  bodies_.back().frames.push_back(rff_.get(cur));
  bodies_.back().state = bodies_[body].state;
  return bodies_.size() - 1;
}

int Data::handle_state(int body, RMF::NodeConstHandle cur) {
  int state_index = stf_.get(cur).get_state_index();
  // don't create duplicate bodies for 0
  if (state_index != bodies_[body].state) {
    if (rff_.get_is(cur)) {
      bodies_.back().state = state_index;
      return body;
    } else {
      bodies_.push_back(Body());
      bodies_.back().frames = bodies_[body].frames;
      bodies_.back().state = state_index;
      return bodies_.size() - 1;
    }
  } else {
    return body;
  }
}

boost::tuple<RMF::NodeConstHandle, boost::array<char, 2>, boost::array<int, 2> >
Data::handle_alternative(RMF::NodeConstHandle cur, int body,
                         boost::array<char, 2> chain, int resid,
                         boost::array<char, 8> resname,
                         boost::array<char, 2> altid,
                         boost::array<char, 8> segment, double resolution) {
  boost::array<int, 2> count = {{0}};
  if (resolution >= 0) {
    RMF::NodeConstHandle alt =
        altf_.get(cur).get_alternative(RMF::decorator::PARTICLE, resolution);
    return boost::make_tuple(alt, altid, count);
  } else {
    RMF::NodeConstHandles alts =
        altf_.get(cur).get_alternatives(RMF::decorator::PARTICLE);
    int alt = 1;
    RMF_FOREACH(RMF::NodeConstHandle c,
                boost::make_iterator_range(alts.begin() + 1, alts.end())) {
      altid[0] = 'A' + alt;
      boost::array<int, 2> cur = fill_bodies(c, body, chain, resid, resname,
                                             altid, segment, resolution);
      for (unsigned int i = 0; i < 2; ++i) {
        count[i] += cur[i];
      }
      ++alt;
    }
    altid[0] = 'A';
    return boost::make_tuple(alts.front(), altid, count);
  }
}

boost::array<int, 2> Data::fill_bodies(RMF::NodeConstHandle cur, int body,
                                       boost::array<char, 2> chain, int resid,
                                       boost::array<char, 8> resname,
                                       boost::array<char, 2> altid,
                                       boost::array<char, 8> segment,
                                       double resolution) {
  boost::array<int, 2> ret = {{0}};
  // must be firest due to ret
  if (altf_.get_is(cur))
    boost::tie(cur, altid, ret) = handle_alternative(
        cur, body, chain, resid, resname, altid, segment, resolution);

  if (cur.get_type() == RMF::ALIAS) return ret;
  if (cur.get_type() == RMF::REPRESENTATION && segment[0] == '\0') {
    my_copy_n(cur.get_name(), 8, segment.begin());
  }

  if (rff_.get_is(cur)) body = handle_reference_frame(body, cur);

  if (stf_.get_is(cur)) body = handle_state(body, cur);

  if (chf_.get_is(cur)) {
    my_copy_n(chf_.get(cur).get_chain_id(), 2, chain.begin());
    //    chain = chf_.get(cur).get_chain_id();
  }
  if (rf_.get_is(cur)) {
    resid = rf_.get(cur).get_residue_index();
    my_copy_n(rf_.get(cur).get_residue_type(), 8, resname.begin());
  }

  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    boost::array<int, 2> count =
        fill_bodies(c, body, chain, resid, resname, altid, segment, resolution);
    for (unsigned int i = 0; i < 2; ++i) {
      ret[i] += count[i];
    }
  }

  if (ret[0] == 0 && pf_.get_is(cur)) {
    AtomInfo ai = {chain, resid, resname, altid, segment, cur.get_id()};
    bodies_[body].particles.push_back(ai);
    ++ret[0];
  }
  if (bf_.get_is(cur)) {
    AtomInfo ai = {chain, resid, resname, altid, segment, cur.get_id()};
    bodies_[body].balls.push_back(ai);
    ++ret[1];
  }
  return ret;
}

void Data::fill_index() {
  int cur_index = -1;
  RMF_FOREACH(const Body & body, bodies_) {
    RMF_FOREACH(const AtomInfo & n, body.particles) {
      index_[n.node_id] = ++cur_index;
    }
    RMF_FOREACH(const AtomInfo & n, body.balls) {
      index_[n.node_id] = ++cur_index;
    }
  }
}

void Data::fill_graphics(RMF::NodeConstHandle cur,
                         RMF::CoordinateTransformer tr) {
  int ret = 0;
  if (rff_.get_is(cur)) {
    tr = RMF::CoordinateTransformer(tr, rff_.get(cur));
  }
  if (sf_.get_is_static(cur)) {
    RMF::decorator::SegmentConst s = sf_.get(cur);
    RMF::Vector3s coords = s.get_static_coordinates_list();
    RMF_INTERNAL_CHECK(coords.size() > 0, "Empty coordinates");
    int type = MOLFILE_LINE;
    double size = 0;
    if (cf_.get_is(cur)) {
      type = MOLFILE_CYLINDER;
      size = cf_.get(cur).get_radius();
    }
    RMF::Vector3 last_coords = tr.get_global_coordinates(coords[0]);
    RMF_FOREACH(const RMF::Vector3 & cc,
                boost::make_iterator_range(coords.begin() + 1, coords.end())) {
      molfile_graphics_t g;
      g.type = type;
      g.size = size;
      g.style = 0;
      std::copy(last_coords.begin(), last_coords.end(), g.data);
      last_coords = tr.get_global_coordinates(cc);
      std::copy(last_coords.begin(), last_coords.end(), g.data + 3);
      graphics_.push_back(g);
    }
  }
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    fill_graphics(c, tr);
  }
}

void Data::handle_bond(RMF::NodeConstHandle cur) {
  RMF::NodeID bonded0(bdf_.get(cur).get_bonded_0());
  RMF::NodeID bonded1(bdf_.get(cur).get_bonded_1());

  if (index_.find(bonded0) != index_.end() &&
      index_.find(bonded1) != index_.end()) {
    bond_from_.push_back(index_.find(bonded0)->second + 1);
    bond_to_.push_back(index_.find(bonded1)->second + 1);
    bond_type_.push_back(0);
  } else {
    std::cout << "Skipping bond " << bonded0 << " to " << bonded1 << std::endl;
  }
}

void Data::handle_restraint(RMF::NodeConstHandle cur) {
  bool child_feature = false;
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    if (c.get_type() == RMF::FEATURE) {
      child_feature = true;
      break;
    }
  }
  if (!child_feature && rcf_.get_is(cur)) {
    RMF::NodeConstHandles reps = rcf_.get(cur).get_representation();
    for (unsigned int i = 0; i < reps.size(); ++i) {
      RMF::NodeID bonded0(reps[i]);
      for (unsigned int j = 0; j < i; ++j) {
        RMF::NodeID bonded1(reps[j]);
        if (index_.find(bonded0) != index_.end() &&
            index_.find(bonded1) != index_.end()) {
          bond_from_.push_back(index_.find(bonded0)->second + 1);
          bond_to_.push_back(index_.find(bonded1)->second + 1);
          bond_type_.push_back(1);
        } else {
          std::cout << "Skipping feature bond " << bonded0 << " to " << bonded1
                    << std::endl;
        }
      }
    }
  }
}

void Data::fill_bonds(RMF::NodeConstHandle cur) {
  if (show_restraints_ & BONDS && bdf_.get_is(cur)) {
    handle_bond(cur);
  }
  if (show_restraints_ & RESTRAINTS && cur.get_type() == RMF::FEATURE) {
    handle_restraint(cur);
  }
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) { fill_bonds(c); }
}

void Data::copy_basics(const AtomInfo &n, molfile_atom_t *out) {
  out->resid = n.residue_index;
  std::copy(n.chain_id.begin(), n.chain_id.end(), out->chain);
  my_copy_n(n.altid, 2, out->altloc);
  my_copy_n(n.segment, 8, out->segid);
  my_copy_n(n.residue_name, 8, out->resname);
  RMF::NodeConstHandle cur = file_.get_node(n.node_id);
  my_copy_n(cur.get_name(), 16, out->name);
}

molfile_atom_t *Data::copy_particles(const std::vector<AtomInfo> &atoms,
                                     molfile_atom_t *out) {
  RMF_FOREACH(const AtomInfo & n, atoms) {
    copy_basics(n, out);

    RMF::NodeConstHandle cur = file_.get_node(n.node_id);
    std::string at;
    if (af_.get_is(cur)) {
      at = cur.get_name();
    } else if (ff_.get_is(cur)) {
      at = "FRAGMENT";
    } else if (df_.get_is(cur)) {
      at = "DOMAIN";
    } else if (tf_.get_is(cur)) {
      at = tf_.get(cur).get_type_name();
    }
    my_copy_n(at, 16, out->type);

    out->mass = pf_.get(cur).get_mass();
    out->radius = pf_.get(cur).get_radius();

    ++out;
  }
  return out;
}

molfile_atom_t *Data::copy_balls(const std::vector<AtomInfo> &balls,
                                 molfile_atom_t *out) {
  RMF_FOREACH(AtomInfo n, balls) {
    copy_basics(n, out);

    RMF::NodeConstHandle cur = file_.get_node(n.node_id);
    my_copy_n(std::string("ball"), 16, out->type);
    out->mass = 0;
    out->radius = bf_.get(cur).get_radius();
    ++out;
  }
  return out;
}

int Data::read_structure(int *optflags, molfile_atom_t *out) {
  RMF_INFO("Reading structure");
  RMF_FOREACH(const Body & body, bodies_) {
    out = copy_particles(body.particles, out);
    out = copy_balls(body.balls, out);
  }
  *optflags = MOLFILE_RADIUS | MOLFILE_MASS | MOLFILE_ALTLOC;
  return VMDPLUGIN_SUCCESS;
}

int Data::read_timestep(molfile_timestep_t *frame) {
  if (done_) return MOLFILE_EOF;
  RMF::FrameID curf = file_.get_current_frame();
  RMF_INFO("Reading next frame at " << curf);
  frame->physical_time = curf.get_index();
  float *coords = frame->coords;
  file_.set_current_frame(RMF::FrameID(curf.get_index()));
  RMF_FOREACH(const Body & body, bodies_) {
    RMF::CoordinateTransformer tr;
    double offset =
        (upper_bounds_[0] - lower_bounds_[0] + max_radius_ * 3) * body.state;
    RMF_FOREACH(RMF::decorator::ReferenceFrameConst rf, body.frames) {
      tr = RMF::CoordinateTransformer(tr, rf);
    }
    RMF_FOREACH(const AtomInfo & n, body.particles) {
      RMF::Vector3 cc = pf_.get(file_.get_node(n.node_id)).get_coordinates();
      cc = tr.get_global_coordinates(cc);
      cc[0] += offset;
      std::copy(cc.begin(), cc.end(), coords);
      coords += 3;
    }
    RMF_FOREACH(const AtomInfo & n, body.balls) {
      RMF::Vector3 cc = bf_.get(file_.get_node(n.node_id)).get_coordinates();
      cc = tr.get_global_coordinates(cc);
      cc[0] += offset;
      std::copy(cc.begin(), cc.end(), coords);
      coords += 3;
    }
  }
  unsigned int next = curf.get_index() + 1;
  if (next >= file_.get_number_of_frames()) {
    done_ = true;
  } else {
    file_.set_current_frame(RMF::FrameID(next));
  }
  return VMDPLUGIN_SUCCESS;
}

int Data::read_graphics(int *nelem, const molfile_graphics_t **gdata) {
  RMF_INFO("Reading graphics");
  fill_graphics(file_.get_root_node(), RMF::CoordinateTransformer());
  *nelem = graphics_.size();
  *gdata = &graphics_[0];
  return VMDPLUGIN_SUCCESS;
}

int Data::read_bonds(int *nbonds, int **fromptr, int **toptr,
                     float **bondorderptr, int **bondtype, int *nbondtypes,
                     char ***bondtypename) {
  RMF_INFO("Reading bonds");
  fill_bonds(file_.get_root_node());
  RMF_INTERNAL_CHECK(bond_type_.size() == bond_from_.size(),
                     "Sizes don't match");
  RMF_INTERNAL_CHECK(bond_type_.size() == bond_to_.size(), "Sizes don't match");
  *nbonds = bond_type_.size();
  RMF_TRACE("Found " << *nbonds << " bonds.");
  *fromptr = &bond_from_[0];
  *toptr = &bond_to_[0];
  *bondtype = &bond_type_[0];
  *bondorderptr = NULL;
  *nbondtypes = 2;
  bond_type_name_.reset(new char[2]);
  bond_type_name_[0] = 'B';
  bond_type_name_[1] = '\0';
  restraint_bond_type_name_.reset(new char[2]);
  restraint_bond_type_name_[0] = 'R';
  restraint_bond_type_name_[1] = '\0';
  bond_type_names_.push_back(bond_type_name_.get());
  bond_type_names_.push_back(restraint_bond_type_name_.get());
  *bondtypename = &bond_type_names_[0];

  RMF_FOREACH(int bt,
              boost::make_iterator_range(*bondtype, *bondtype + *nbonds)) {
    RMF_INTERNAL_CHECK(bt >= 0 && bt < *nbondtypes, "Invalid bond type ");
  }
  return VMDPLUGIN_SUCCESS;
}

int Data::read_timestep_metadata(molfile_timestep_metadata_t *data) {
  RMF_INFO("Reading timestep data");
  data->count = file_.get_number_of_frames();
  int na = 0;
  RMF_FOREACH(const Body & bd, bodies_) {
    na += bd.particles.size() + bd.balls.size();
  }
  data->avg_bytes_per_timestep = sizeof(float) * 3 * na;
  data->has_velocities = 0;
  return VMDPLUGIN_SUCCESS;
}
}
#endif /* RMF_RMFPLUGIN_H */
