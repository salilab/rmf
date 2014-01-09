/**
 *  \file rmfplugin.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_RMFPLUGIN_H
#define RMF_RMFPLUGIN_H

#include <boost/scoped_array.hpp>
#include <stddef.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "RMF/CoordinateTransformer.h"
#include "RMF/FileConstHandle.h"
#include "RMF/ID.h"
#include "RMF/NodeConstHandle.h"
#include "RMF/Vector.h"
#include "RMF/decorator/feature.h"
#include "RMF/decorator/physics.h"
#include "RMF/decorator/sequence.h"
#include "RMF/decorator/shape.h"
#include "RMF/decorator/alternatives.h"
#include "RMF/enums.h"
#include "RMF/exceptions.h"
#include "RMF/infrastructure_macros.h"
#include "RMF/log.h"
#include "RMF/types.h"
#include "molfile_plugin.h"

namespace {
molfile_plugin_t plugin;
molfile_plugin_t plugin3;
molfile_plugin_t pluginz;

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
  struct Body {
    std::vector<RMF::decorator::ReferenceFrameConst> frames;
    RMF::NodeIDs atoms;
    RMF::NodeIDs balls;
    RMF::NodeIDs representation;
  };
  std::vector<Body> bodies_;
  unsigned int num_atoms_;
  boost::unordered_map<RMF::NodeID, int> index_;
  boost::unordered_map<RMF::NodeID, int> rep_index_;
  boost::scoped_array<molfile_graphics_t> graphics_;
  boost::scoped_array<int> bonds_to_, bonds_from_, bond_types_;
  boost::scoped_array<char> bond_type_, restraint_bond_type_;
  char *bt_char_, *rbt_char_;
  double resolution_;
  int states_;
  RMF::Vector3 upper_bound_;
  double max_radius_;
  bool done_;

  // find nodes to push to vmd
  boost::array<int, 3> get_structure(RMF::NodeConstHandle cur, int first_index,
                                     molfile_atom_t *atoms, int body,
                                     std::string chain, int resid,
                                     std::string resname, double resolution);
  int get_graphics(RMF::NodeConstHandle cur, RMF::CoordinateTransformer tr,
                   molfile_graphics_t *graphics);
  int get_bonds(RMF::NodeConstHandle cur, int offset, int *from, int *to,
                int *type);

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
      resolution_(1.0),
      states_(1),
      upper_bound_(0, 0, 0),
      max_radius_(0),
      done_(false) {
  RMF::Floats resolutions =
      RMF::decorator::get_resolutions(file_.get_root_node());
  if (resolutions.size() > 1) {
    std::cout << "RMF: Resolutions are " << RMF::Showable(resolutions)
              << ". Please enter desired resolution: " << std::endl;
    std::cin >> resolution_;
  }
  file_.set_current_frame(RMF::FrameID(0));
  RMF::FrameIDs fch = file_.get_children(RMF::FrameID(0));
  states_ = fch.size() + 1;
  std::cout << "RMF: found " << states_ << " states." << std::endl;
  bodies_.push_back(Body());

  boost::array<int, 3> na = get_structure(file_.get_root_node(), 0, NULL, 0,
                                          " ", -1, std::string(), resolution_);
  num_atoms_ = na[0] + na[1] + na[2];

  RMF_FOREACH(RMF::NodeID id, file_.get_node_ids()) {
    if (pf_.get_is(file_.get_node(id))) {
      RMF::decorator::ParticleConst pc = pf_.get(file_.get_node(id));
      double r = pc.get_radius();
      max_radius_ = std::max<double>(r, max_radius_);
      RMF::Vector3 c = pc.get_coordinates();
      for (unsigned int i = 0; i < 3; ++i) {
        upper_bound_[i] = std::max<double>(upper_bound_[i], c[i] + r);
      }
    }
  }
}

//
boost::array<int, 3> Data::get_structure(RMF::NodeConstHandle cur,
                                         int next_index, molfile_atom_t *atoms,
                                         int body, std::string chain, int resid,
                                         std::string resname,
                                         double resolution) {
  boost::array<int, 3> ret = {{0}};
  if (cur.get_type() == RMF::ALIAS) return ret;
  if (rff_.get_is(cur)) {
    bodies_.push_back(Body());
    bodies_.back().frames = bodies_[body].frames;
    bodies_.back().frames.push_back(rff_.get(cur));
    body = bodies_.size() - 1;
  }
  if (chf_.get_is(cur)) {
    chain = chf_.get(cur).get_chain_id();
  }
  if (rf_.get_is(cur)) {
    resid = rf_.get(cur).get_residue_index();
    resname = rf_.get(cur).get_residue_type();
  }
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    boost::array<int, 3> count = get_structure(
        c, next_index, atoms, body, chain, resid, resname, resolution);
    for (unsigned int i = 0; i < 3; ++i) {
      ret[i] += count[i];
      next_index += count[i];
      if (atoms) atoms += count[i];
    }
  }

  if (ret[0] == 0 && pf_.get_is(cur)) {
    if (atoms) {
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
      if (nm.size() >= 16) {
        std::copy(nm.begin(), nm.begin() + std::min<std::size_t>(nm.size(), 15),
                  atoms->name);
        nm[15] = '\0';
      } else {
        std::copy(nm.begin(), nm.end(), atoms->name);
      }
      std::copy(at.begin(), at.end(), atoms->type);
      std::copy(resname.begin(), resname.end(), atoms->resname);
      atoms->resid = resid;
      atoms->chain[0] = chain[0];
      atoms->chain[1] = '\0';
      if (cpf_.get_is(cur)) {
        sprintf(atoms->segid, "%d", cpf_.get(cur).get_copy_index());
      } else {
        atoms->segid[0] = '\0';
      }
      atoms->mass = pf_.get(cur).get_mass();
      atoms->radius = pf_.get(cur).get_radius();
    } else {
      bodies_[body].atoms.push_back(cur.get_id());
      index_[cur.get_id()] = next_index;
    }
    ++next_index;
    ++ret[0];
  }
  if (bf_.get_is(cur)) {
    if (atoms) {
      std::string nm = cur.get_name();
      std::copy(nm.begin(), nm.end(), atoms->name);
      std::string ball("ball");
      std::copy(ball.begin(), ball.end(), atoms->type);
      std::copy(resname.begin(), resname.end(), atoms->resname);
      atoms->resid = resid;
      atoms->chain[0] = chain[0];
      atoms->chain[1] = '\0';
      atoms->segid[0] = '\0';
      atoms->mass = 0;
      atoms->radius = bf_.get(cur).get_radius();
    } else {
      bodies_[body].balls.push_back(cur.get_id());
      index_[cur.get_id()] = next_index;
    }
    ++next_index;
    ++ret[1];
  }
  if (cur.get_type() == RMF::FEATURE && ret[2] == 0) {
    RMF::Ints all_nodes = rcf_.get(cur).get_static_representation();
    RMF_FOREACH(int in, all_nodes) {
      RMF::NodeID n(in);
      if (pf_.get_is(file_.get_node(n)) && index_.find(n) != index_.end()) {
        if (atoms) {
          std::string nm = cur.get_name();
          std::copy(nm.begin(), nm.end(), atoms->name);
          std::string restraint("member");
          std::copy(restraint.begin(), restraint.end(), atoms->type);
          std::string resname = "RES";
          std::copy(resname.begin(), resname.end(), atoms->resname);
          atoms->resid = 0;
          atoms->chain[0] = '\0';
          atoms->segid[0] = '\0';
          atoms->mass = 0;
          atoms->radius = .1;
        } else {
          bodies_[body].representation.push_back(n);
          rep_index_[n] = next_index;
        }
        ++next_index;
        ++ret[2];
      }
    }
  }
  return ret;
}

void Data::read_structure(molfile_atom_t *atoms) {
  RMF_INFO("Reading structure");
  unsigned int so_far = 0;
  for (unsigned int i = 0; i < states_; ++i) {
    RMF_INFO("Getting structure (" << so_far << ")");
    boost::array<int, 3> ret =
        get_structure(file_.get_root_node(), so_far, atoms + so_far, 0, " ", -1,
                      "NONE", resolution_);
    so_far += ret[0] + ret[1] + ret[2];
  }
}

bool Data::read_next_frame(molfile_timestep_t *frame) {
  if (done_) return false;
  RMF::FrameID curf = file_.get_current_frame();
  RMF_INFO("Reading next frame at " << curf);
  frame->physical_time = curf.get_index() / states_;
  float *coords = frame->coords;
  for (unsigned int state = 0; state < states_; ++state) {
    file_.set_current_frame(RMF::FrameID(curf.get_index() + state));
    for (unsigned int i = 0; i < bodies_.size(); ++i) {
      RMF::CoordinateTransformer tr;
      RMF_FOREACH(RMF::decorator::ReferenceFrameConst rf, bodies_[i].frames) {
        tr = RMF::CoordinateTransformer(tr, rf);
      }
      RMF_FOREACH(RMF::NodeID n, bodies_[i].atoms) {
        RMF::Vector3 cc;
        cc = pf_.get(file_.get_node(n)).get_coordinates();
        cc = tr.get_global_coordinates(cc);
        cc[0] += (max_radius_ * 3 + upper_bound_[0]) * state;
        std::copy(cc.begin(), cc.end(), coords);
        coords += 3;
      }
      RMF_FOREACH(RMF::NodeID n, bodies_[i].balls) {
        RMF::Vector3 cc;
        cc = bf_.get(file_.get_node(n)).get_coordinates();
        cc = tr.get_global_coordinates(cc);
        cc[0] += upper_bound_[0] * state;
        std::copy(cc.begin(), cc.end(), coords);
        coords += 3;
      }

      RMF_FOREACH(RMF::NodeID n, bodies_[i].representation) {
        RMF::Vector3 cc;
        cc = pf_.get(file_.get_node(n)).get_coordinates();
        cc = tr.get_global_coordinates(cc);
        cc[0] += upper_bound_[0] * state;
        std::copy(cc.begin(), cc.end(), coords);
        coords += 3;
      }
    }
  }
  unsigned int next = curf.get_index() + states_;
  if (next >= file_.get_number_of_frames()) {
    done_ = true;
  } else {
    file_.set_current_frame(RMF::FrameID(curf.get_index() + states_));
  }
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
  int base_bonds = get_bonds(file_.get_root_node(), 0, NULL, NULL, NULL);
  *nbonds = base_bonds * states_;
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

  for (unsigned int i = 0; i < states_; ++i) {
    RMF_INFO("Getting bonds (" << num_atoms_ * i << ")");
    get_bonds(file_.get_root_node(), num_atoms_ * i, *fromptr + base_bonds * i,
              *toptr + base_bonds * i, *bondtype + base_bonds * i);
  }
}

int Data::get_graphics(RMF::NodeConstHandle cur, RMF::CoordinateTransformer tr,
                       molfile_graphics_t *graphics) {
  RMF_INFO("Getting graphics");
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
int Data::get_bonds(RMF::NodeConstHandle cur, int offset, int *from, int *to,
                    int *type) {
  int ret = 0;
  if (bdf_.get_is(cur)) {
    RMF::NodeID bonded0(bdf_.get(cur).get_bonded_0());
    RMF::NodeID bonded1(bdf_.get(cur).get_bonded_1());

    if (index_.find(bonded0) != index_.end() &&
        index_.find(bonded1) != index_.end()) {
      if (from) {
        /*std::cout << "Adding bond " << index_.find(bonded0)->second + 1 << " "
                  << index_.find(bonded1)->second + 1 << " " << bonded0 << " "
                  << bonded1 << std::endl;*/
        *from = index_.find(bonded0)->second + 1 + offset;
        *to = index_.find(bonded1)->second + 1 + offset;
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
      RMF::Ints reps = rcf_.get(cur).get_representation();
      for (unsigned int i = 0; i < reps.size(); ++i) {
        RMF::NodeID bonded0(reps[i]);
        for (unsigned int j = 0; j < i; ++j) {
          RMF::NodeID bonded1(reps[j]);
          if (rep_index_.find(bonded0) != rep_index_.end() &&
              rep_index_.find(bonded1) != rep_index_.end()) {
            if (from) {
              std::cout << "Adding bond "
                        << rep_index_.find(bonded0)->second + 1 << " "
                        << rep_index_.find(bonded1)->second + 1 << std::endl;
              *from = rep_index_.find(bonded0)->second + 1 + offset;
              *to = rep_index_.find(bonded1)->second + 1 + offset;
              *type = 1;
              ++from;
              ++to;
              ++type;
            }
            ++ret;
          }
        }
      }
    }
  }
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    int ci = get_bonds(c, offset, from, to, type);
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
  if (data->count == 0) data->count = 1;
  data->avg_bytes_per_timestep =
      sizeof(float) * 3 * get_number_of_atoms() * states_;
  data->has_velocities = 0;
}
}
#endif /* RMF_RMFPLUGIN_H */
