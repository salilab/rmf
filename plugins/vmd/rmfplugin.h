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
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm_ext/copy_n.hpp>
#include <boost/range/distance.hpp>
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
  if (boost::distance(range) <=n) {
    boost::range::copy(range, out);
  } else {
    // some boost object to the input being too small
    boost::range::copy_n(range, n, out);
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
    boost::array<char, 8> segment;
    RMF::NodeID node_id;
  };
  struct Body {
    std::vector<RMF::decorator::ReferenceFrameConst> frames;
    std::vector<AtomInfo> atoms;
    std::vector<AtomInfo> balls;
    int state;
    Body() : state(0) {}
  };
  std::vector<Body> bodies_;
  unsigned int num_atoms_;
  boost::unordered_map<RMF::NodeID, int> index_;
  boost::scoped_array<molfile_graphics_t> graphics_;
  boost::scoped_array<int> bonds_to_, bonds_from_, bond_types_;
  boost::scoped_array<char> bond_type_, restraint_bond_type_;
  char *bt_char_, *rbt_char_;
  double resolution_;
  int states_;
  RMF::Vector3 lower_bounds_;
  RMF::Vector3 upper_bounds_;
  double max_radius_;
  bool done_;

  // find nodes to push to vmd
  boost::array<int, 3> fill_bodies(RMF::NodeConstHandle cur, int body,
                                   boost::array<char, 2> chain, int resid,
                                   boost::array<char, 8> resname,
                                   double resolution,
                                   boost::array<char, 8> segment);
  void get_structure(molfile_atom_t *atoms);
  void fill_index();
  int get_graphics(RMF::NodeConstHandle cur, RMF::CoordinateTransformer tr,
                   molfile_graphics_t *graphics);
  int get_bonds(RMF::NodeConstHandle cur, int *from, int *to, int *type);

 public:
  Data(std::string name);
  void read_structure(molfile_atom_t *atoms);
  bool read_next_frame(molfile_timestep_t *frame);
  void read_graphics(int *nelem, const molfile_graphics_t **gdata);
  void read_bonds(int *nbonds, int **fromptr, int **toptr, float **bondorderptr,
                  int **bondtype, int *nbondtypes, char ***bondtypename);
  void read_timestep_data(molfile_timestep_metadata_t *data);
  unsigned int get_number_of_atoms() const { return num_atoms_; }
  unsigned int get_number_of_states() const { return states_; }
};

Data::Data(std::string name)
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
      resolution_(1.0),
      states_(1),
      lower_bounds_(std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max()),
      upper_bounds_(-std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max()),
      max_radius_(0),
      done_(false) {
  RMF::Floats resolutions =
      RMF::decorator::get_resolutions(file_.get_root_node());
  if (resolutions.size() > 1) {
    std::cout << "RMF: Resolutions are " << RMF::Showable(resolutions)
              << ". Please enter desired resolution: " << std::endl;
    std::cin >> resolution_;
  }
  if (file_.get_number_of_frames() > 0) {
    file_.set_current_frame(RMF::FrameID(0));
  }

  bodies_.push_back(Body());

  boost::array<char, 2> default_chain = {0};
  boost::array<char, 8> default_resname = {0};
  boost::array<char, 8> default_segment = {0};
  boost::array<int, 3> na =
      fill_bodies(file_.get_root_node(), 0, default_chain, -1, default_resname,
                  resolution_, default_segment);
  fill_index();
  num_atoms_ = na[0] + na[1] + na[2];
  std::cout << "RMF: found " << states_ << " states and " << num_atoms_
            << " atoms." << std::endl;

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

boost::array<int, 3> Data::fill_bodies(RMF::NodeConstHandle cur, int body,
                                       boost::array<char, 2> chain, int resid,
                                       boost::array<char, 8> resname,
                                       double resolution,
                                       boost::array<char, 8> segment) {
  boost::array<int, 3> ret = {{0}};
  if (cur.get_type() == RMF::ALIAS) return ret;
  if (cur.get_type() == RMF::REPRESENTATION && segment[0] == '\0') {
    my_copy_n(cur.get_name(), 8, segment.begin());
  }
  if (altf_.get_is(cur)) {
    cur = altf_.get(cur).get_alternative(RMF::decorator::PARTICLE, resolution);
  }
  if (rff_.get_is(cur)) {
    bodies_.push_back(Body());
    bodies_.back().frames = bodies_[body].frames;
    bodies_.back().frames.push_back(rff_.get(cur));
    bodies_.back().state = bodies_[body].state;
    body = bodies_.size() - 1;
  }
  if (stf_.get_is(cur)) {
    int state_index = stf_.get(cur).get_state_index();
    // don't create duplicate bodies for 0
    if (state_index != bodies_[body].state) {
      if (rff_.get_is(cur)) {
        bodies_.back().state = state_index;
      } else {
        bodies_.push_back(Body());
        bodies_.back().frames = bodies_[body].frames;
        bodies_.back().state = state_index;
        body = bodies_.size() - 1;
      }
      states_ = std::max<int>(states_, state_index + 1);
    }
  }

  if (chf_.get_is(cur)) {
    my_copy_n(chf_.get(cur).get_chain_id(), 2, chain.begin());
    //    chain = chf_.get(cur).get_chain_id();
  }
  if (rf_.get_is(cur)) {
    resid = rf_.get(cur).get_residue_index();
    my_copy_n(rf_.get(cur).get_residue_type(), 8, resname.begin());
  }
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    boost::array<int, 3> count =
        fill_bodies(c, body, chain, resid, resname, resolution, segment);
    for (unsigned int i = 0; i < 3; ++i) {
      ret[i] += count[i];
    }
  }

  if (ret[0] == 0 && pf_.get_is(cur)) {
    AtomInfo ai = {chain, resid, resname, segment, cur.get_id()};
    bodies_[body].atoms.push_back(ai);
    ++ret[0];
  }
  if (bf_.get_is(cur)) {
    AtomInfo ai = {chain, resid, resname, segment, cur.get_id()};
    bodies_[body].balls.push_back(ai);
    ++ret[1];
  }
  return ret;
}

void Data::get_structure(molfile_atom_t *atoms) {
  RMF_FOREACH(const Body & body, bodies_) {
    RMF_FOREACH(const AtomInfo & n, body.atoms) {
      RMF::NodeConstHandle cur = file_.get_node(n.node_id);
      std::string nm = cur.get_name();
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
      my_copy_n(nm, 16, atoms->name);
      my_copy_n(at, 16, atoms->type);
      atoms->resid = n.residue_index;
      boost::range::copy(n.chain_id, atoms->chain);
      atoms->mass = pf_.get(cur).get_mass();
      atoms->radius = pf_.get(cur).get_radius();
      ++atoms;
    }
    RMF_FOREACH(AtomInfo n, body.balls) {
      RMF::NodeConstHandle cur = file_.get_node(n.node_id);
      my_copy_n(cur.get_name(), 16, atoms->name);
      my_copy_n(std::string("ball"), 16, atoms->type);
      atoms->resid = n.residue_index;
      if (n.chain_id.size() > 0) {
        atoms->chain[0] = n.chain_id[0];
      } else {
        atoms->chain[0] = '-';
      }
      if (n.chain_id.size() > 1) {
        atoms->chain[1] = n.chain_id[1];
      } else {
        atoms->chain[1] = '\0';
      }
      if (cpf_.get_is(cur)) {
        sprintf(atoms->segid, "%d", cpf_.get(cur).get_copy_index());
      } else {
        atoms->segid[0] = '\0';
      }
      atoms->mass = 0;
      atoms->radius = bf_.get(cur).get_radius();
    }
  }
}

void Data::read_structure(molfile_atom_t *atoms) {
  RMF_INFO("Reading structure");
  get_structure(atoms);
}

void Data::fill_index() {
  int cur_index = -1;
  RMF_FOREACH(const Body & body, bodies_) {
    RMF_FOREACH(const AtomInfo & n, body.atoms) {
      index_[n.node_id] = ++cur_index;
    }
    RMF_FOREACH(const AtomInfo & n, body.balls) {
      index_[n.node_id] = ++cur_index;
    }
  }
}

bool Data::read_next_frame(molfile_timestep_t *frame) {
  if (done_) return false;
  RMF::FrameID curf = file_.get_current_frame();
  RMF_INFO("Reading next frame at " << curf);
  frame->physical_time = curf.get_index();
  float *coords = frame->coords;
  file_.set_current_frame(RMF::FrameID(curf.get_index()));
  RMF_FOREACH(const Body & body, bodies_) {
    std::cout << "Handling body with " << body.frames.size() << " frames"
              << std::endl;
    RMF::CoordinateTransformer tr;
    double offset =
        (upper_bounds_[0] - lower_bounds_[0] + max_radius_ * 3) * body.state;
    std::cout << "offset is " << offset << " (" << body.state << ")"
              << std::endl;
    RMF_FOREACH(RMF::decorator::ReferenceFrameConst rf, body.frames) {
      tr = RMF::CoordinateTransformer(tr, rf);
    }
    RMF_FOREACH(const AtomInfo & n, body.atoms) {
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
  /*std::copy(frame->coords, frame->coords + num_atoms_ * 3,
            std::ostream_iterator<float>(std::cout, ", "));
            std::cout << std::endl;*/
  return true;
}

void Data::read_graphics(int *nelem, const molfile_graphics_t **gdata) {
  RMF_INFO("Reading graphics");
  *nelem =
      get_graphics(file_.get_root_node(), RMF::CoordinateTransformer(), NULL);
  graphics_.reset(new molfile_graphics_t[*nelem]);
  *gdata = graphics_.get();
  get_graphics(file_.get_root_node(), RMF::CoordinateTransformer(),
               graphics_.get());
}

void Data::read_bonds(int *nbonds, int **fromptr, int **toptr,
                      float **bondorderptr, int **bondtype, int *nbondtypes,
                      char ***bondtypename) {
  RMF_INFO("Reading bonds");
  int base_bonds = get_bonds(file_.get_root_node(), NULL, NULL, NULL);
  *nbonds = base_bonds;
  RMF_TRACE("Found " << *nbonds << " bonds.");
  bonds_from_.reset(new int[*nbonds]);
  *fromptr = bonds_from_.get();
  bonds_to_.reset(new int[*nbonds]);
  *toptr = bonds_to_.get();
  *bondorderptr = NULL;
  bond_types_.reset(new int[*nbonds]);
  *bondtype = bond_types_.get();
  *nbondtypes = 2;
  bond_type_.reset(new char[2]);
  bond_type_[0] = 'B';
  bond_type_[1] = '\0';
  restraint_bond_type_.reset(new char[2]);
  restraint_bond_type_[0] = 'R';
  restraint_bond_type_[1] = '\0';
  bt_char_ = bond_type_.get();
  rbt_char_ = restraint_bond_type_.get();
  bondtypename[0] = &bt_char_;

  RMF_INFO("Getting bonds (" << num_atoms_ << ")");
  get_bonds(file_.get_root_node(), *fromptr, *toptr, *bondtype);
}

int Data::get_graphics(RMF::NodeConstHandle cur, RMF::CoordinateTransformer tr,
                       molfile_graphics_t *graphics) {
  int ret = 0;
  if (graphics && rff_.get_is(cur)) {
    tr = RMF::CoordinateTransformer(tr, rff_.get(cur));
  }
  if (sf_.get_is(cur)) {
    RMF::decorator::SegmentConst s = sf_.get(cur);
    RMF::Vector3s coords = s.get_coordinates_list();
    RMF_INTERNAL_CHECK(coords.size() > 0, "Empty coordinates");
    int type = MOLFILE_LINE;
    double size = 0;
    if (cf_.get_is(cur)) {
      type = MOLFILE_CYLINDER;
      size = cf_.get(cur).get_radius();
    }
    RMF::Vector3 last_coords;
    if (graphics) {
      last_coords = coords[0];
      last_coords = tr.get_global_coordinates(last_coords);
    }
    for (unsigned int i = 1; i < coords.size(); ++i) {
      if (graphics) {
        graphics->type = type;
        graphics->size = size;
        graphics->style = 0;
        std::copy(last_coords.begin(), last_coords.end(), graphics->data);
        last_coords = coords[i];
        last_coords = tr.get_global_coordinates(last_coords);
        std::copy(last_coords.begin(), last_coords.end(), graphics->data + 3);
        ++graphics;
      }
      ++ret;
    }
  }
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    int cur = get_graphics(c, tr, graphics);
    if (graphics) graphics += cur;
    ret += cur;
  }
  return ret;
}
int Data::get_bonds(RMF::NodeConstHandle cur, int *from, int *to, int *type) {
  int ret = 0;
  static const int bond_offset = 1;
  if (bdf_.get_is(cur)) {
    RMF::NodeID bonded0(bdf_.get(cur).get_bonded_0());
    RMF::NodeID bonded1(bdf_.get(cur).get_bonded_1());

    if (index_.find(bonded0) != index_.end() &&
        index_.find(bonded1) != index_.end()) {
      if (from) {
        /*std::cout << "Adding bond " << index_.find(bonded0)->second + 1 << " "
                  << index_.find(bonded1)->second + 1 << " " << bonded0 << " "
                  << bonded1 << std::endl;*/
        *from = index_.find(bonded0)->second + bond_offset;
        *to = index_.find(bonded1)->second + bond_offset;
        *type = 0;
        ++from;
        ++to;
        ++type;
      }
      ++ret;
    } else {
      std::cout << "Skipping bond " << bonded0 << " to " << bonded1
                << std::endl;
    }
  }
  if (cur.get_type() == RMF::FEATURE) {
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
            if (from) {
              std::cout << "Adding feature bond "
                        << index_.find(bonded0)->second + 1 << " "
                        << index_.find(bonded1)->second + 1 << std::endl;
              *from = index_.find(bonded0)->second + bond_offset;
              *to = index_.find(bonded1)->second + bond_offset;
              *type = 1;
              ++from;
              ++to;
              ++type;
            }
            ++ret;
          } else {
            std::cout << "Skipping feature bond " << bonded0 << " to "
                      << bonded1 << std::endl;
          }
        }
      }
    }
  }
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    int ci = get_bonds(c, from, to, type);
    if (from) {
      from += ci;
      to += ci;
    }
    ret += ci;
  }
  return ret;
}
void Data::read_timestep_data(molfile_timestep_metadata_t *data) {
  RMF_INFO("Reading timestep data");
  data->count = file_.get_number_of_frames();
  data->avg_bytes_per_timestep = sizeof(float) * 3 * get_number_of_atoms();
  data->has_velocities = 0;
}
}
#endif /* RMF_RMFPLUGIN_H */
