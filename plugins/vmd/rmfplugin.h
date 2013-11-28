#include "molfile_plugin.h"

#include <RMF/FileConstHandle.h>
#include <RMF/decorator/physics.h>
#include <RMF/decorator/sequence.h>
#include <RMF/decorator/shape.h>
#include <RMF/decorator/feature.h>
#include <RMF/CoordinateTransformer.h>
#include <RMF/log.h>
#include <boost/scoped_array.hpp>

namespace {
molfile_plugin_t plugin;
molfile_plugin_t plugin3;
molfile_plugin_t pluginz;
molfile_plugin_t pluginrestraints;

class Data {
  RMF::FileConstHandle file_;
  RMF::decorator::AtomConstFactory af_;
  RMF::decorator::ResidueConstFactory rf_;
  RMF::decorator::ChainConstFactory chf_;
  RMF::decorator::ParticleConstFactory pf_;
  RMF::decorator::ReferenceFrameConstFactory rff_;
  RMF::decorator::BallConstFactory bf_;
  RMF::decorator::SegmentConstFactory sf_;
  RMF::decorator::CylinderConstFactory cf_;
  RMF::decorator::BondConstFactory bdf_;
  RMF::decorator::ScoreConstFactory scf_;
  RMF::decorator::RepresentationConstFactory rcf_;
  RMF::decorator::DomainConstFactory df_;
  RMF::decorator::FragmentConstFactory ff_;
  RMF::decorator::CopyConstFactory cpf_;
  RMF::decorator::TypedConstFactory tf_;
  struct Body {
    std::vector<RMF::decorator::ReferenceFrameConst> frames;
    RMF::NodeConstHandles atoms;
    RMF::NodeConstHandles balls;
  };
  std::vector<Body> bodies_;
  unsigned int num_atoms_;

  boost::scoped_array<molfile_graphics_t> graphics_;
  boost::scoped_array<int> bonds_to_, bonds_from_, bond_types_;
  boost::scoped_array<char> bond_type_, restraint_bond_type_;
  char *bt_char_, *rbt_char_;
  bool done_;

  // find nodes to push to vmd
  std::pair<int, int> get_structure(RMF::NodeConstHandle cur,
                                    molfile_atom_t *atoms, int body,
                                    std::string chain, int resid,
                                    std::string resname);
  int get_graphics(RMF::NodeConstHandle cur, RMF::CoordinateTransformer tr,
                   molfile_graphics_t *graphics);
  int get_bonds(RMF::NodeConstHandle cur,
                const std::map<RMF::NodeConstHandle, int> &index, int *from,
                int *to, int *type, bool restraints);

 public:
  Data(std::string name);
  void read_structure(molfile_atom_t *atoms);
  bool read_next_frame(molfile_timestep_t *frame);
  void read_graphics(int *nelem, const molfile_graphics_t **gdata);
  void read_bonds(int *nbonds, int **fromptr, int **toptr, float **bondorderptr,
                  int **bondtype, int *nbondtypes, char ***bondtypename,
                  bool restraints = false);
  void read_timestep_data(molfile_timestep_metadata_t *data);
  unsigned int get_number_of_atoms() const { return num_atoms_; }
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
      done_(false) {
  file_.set_current_frame(RMF::FrameID(0));
  bodies_.push_back(Body());
  std::pair<int, int> na =
      get_structure(file_.get_root_node(), NULL, 0, " ", -1, std::string());
  num_atoms_ = na.first + na.second;
}

//
std::pair<int, int> Data::get_structure(RMF::NodeConstHandle cur,
                                        molfile_atom_t *atoms, int body,
                                        std::string chain, int resid,
                                        std::string resname) {
  std::pair<int, int> ret(0, 0);
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
    resid = rf_.get(cur).get_index();
    resname = rf_.get(cur).get_type();
  }
  RMF_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
    std::pair<int, int> count =
        get_structure(c, atoms, body, chain, resid, resname);
    ret.first += count.first;
    ret.second += count.second;
    if (atoms) {
      atoms += count.first;
      atoms += count.second;
    }
  }

  if (ret.first == 0 && pf_.get_is(cur)) {
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
        std::copy_n(nm.begin(), 15, atoms->name);
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
      bodies_[body].atoms.push_back(cur);
    }
    ++ret.first;
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
      bodies_[body].balls.push_back(cur);
    }
    ++ret.second;
  }
  return ret;
}

void Data::read_structure(molfile_atom_t *atoms) {
  std::pair<int, int> found =
      get_structure(file_.get_root_node(), atoms, 0, " ", -1, "NONE");
  std::cout << "found " << found.first + found.second << " structural particles"
            << std::endl;
}

bool Data::read_next_frame(molfile_timestep_t *frame) {
  if (done_) return false;
  frame->physical_time = 0;
  float *coords = frame->coords;
  for (unsigned int i = 0; i < bodies_.size(); ++i) {
    RMF::CoordinateTransformer tr;
    for (unsigned int j = 0; j < bodies_[i].frames.size(); ++j) {
      tr = RMF::CoordinateTransformer(tr, bodies_[i].frames[j]);
    }
    for (unsigned int j = 0; j < bodies_[i].atoms.size(); ++j) {
      RMF::Vector3 cc;
      cc = pf_.get(bodies_[i].atoms[j]).get_coordinates();
      cc = tr.get_global_coordinates(cc);
      std::copy(cc.begin(), cc.end(), coords);
      coords += 3;
    }
    for (unsigned int j = 0; j < bodies_[i].balls.size(); ++j) {
      RMF::Vector3 cc;
      cc = bf_.get(bodies_[i].atoms[j]).get_coordinates();
      cc = tr.get_global_coordinates(cc);
      std::copy(cc.begin(), cc.end(), coords);
      coords += 3;
    }
  }
  RMF::FrameID curf = file_.get_current_frame();
  unsigned int next = curf.get_index() + 1;
  if (next == file_.get_number_of_frames()) {
    done_ = true;
  } else {
    file_.set_current_frame(RMF::FrameID(curf.get_index() + 1));
  }
  return true;
}

void Data::read_graphics(int *nelem, const molfile_graphics_t **gdata) {
  std::cout << "read graphics" << std::endl;
  *nelem =
      get_graphics(file_.get_root_node(), RMF::CoordinateTransformer(), NULL);
  graphics_.reset(new molfile_graphics_t[*nelem]);
  *gdata = graphics_.get();
  get_graphics(file_.get_root_node(), RMF::CoordinateTransformer(),
               graphics_.get());
}

void Data::read_bonds(int *nbonds, int **fromptr, int **toptr,
                      float **bondorderptr, int **bondtype, int *nbondtypes,
                      char ***bondtypename, bool restraints) {
  std::map<RMF::NodeConstHandle, int> index;
  for (unsigned int i = 0; i < bodies_.size(); ++i) {
    for (unsigned int j = 0; j < bodies_[i].atoms.size(); ++j) {
      int c = index.size();
      index[bodies_[i].atoms[j]] = c;
    }
  }
  *nbonds =
      get_bonds(file_.get_root_node(), index, NULL, NULL, NULL, restraints);
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
  get_bonds(file_.get_root_node(), index, *fromptr, *toptr, *bondtype,
            restraints);
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
    std::cout << coords << " read" << std::endl;
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
  std::cout << "Ret for " << cur.get_name() << " is " << ret << std::endl;
  return ret;
}
int Data::get_bonds(RMF::NodeConstHandle cur,
                    const std::map<RMF::NodeConstHandle, int> &index, int *from,
                    int *to, int *type, bool restraints) {
  int ret = 0;
  if (bdf_.get_is(cur)) {
    RMF::NodeConstHandle bonded0(
        cur.get_file().get_node(RMF::NodeID(bdf_.get(cur).get_bonded_0())));
    RMF::NodeConstHandle bonded1(
        cur.get_file().get_node(RMF::NodeID(bdf_.get(cur).get_bonded_1())));

    if (index.find(bonded0) != index.end() &&
        index.find(bonded1) != index.end()) {
      if (from) {
        *from = index.find(bonded0)->second + 1;
        *to = index.find(bonded1)->second + 1;
        *type = 0;
        ++from;
        ++to;
        ++type;
      }
      ++ret;
    }
  }
  if (restraints && cur.get_type() == RMF::FEATURE) {
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
        RMF::NodeConstHandle bonded0(
            cur.get_file().get_node(RMF::NodeID(reps[i])));
        for (unsigned int j = 0; j < i; ++j) {
          RMF::NodeConstHandle bonded1(
              cur.get_file().get_node(RMF::NodeID(reps[j])));
          if (index.find(bonded0) != index.end() &&
              index.find(bonded1) != index.end()) {
            if (from) {
              *from = index.find(bonded0)->second + 1;
              *to = index.find(bonded1)->second + 1;
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
    int ci = get_bonds(c, index, from, to, type, restraints);
    if (from) {
      from += ci;
      to += ci;
    }
    ret += ci;
  }
  return ret;
}
void Data::read_timestep_data(molfile_timestep_metadata_t *data) {
  data->count = file_.get_number_of_frames();
  if (data->count == 0) data->count = 1;
  data->avg_bytes_per_timestep = sizeof(float) * 3 * get_number_of_atoms();
  data->has_velocities = 0;
}

void close_rmf_read(void *mydata) {
  Data *data = reinterpret_cast<Data *>(mydata);
  delete data;
}

void *open_rmf_read(const char *filename, const char *, int *natoms) {
  Data *data = new Data(filename);
  *natoms = data->get_number_of_atoms();
  if (*natoms == 0) {
    *natoms = MOLFILE_NUMATOMS_NONE;
  }
  std::cout << "open_rmf_read " << filename << ": " << *natoms << std::endl;
  return data;
}

int read_rmf_structure(void *mydata, int *optflags, molfile_atom_t *atoms) {
  std::cout << "read structure" << std::endl;
  Data *data = reinterpret_cast<Data *>(mydata);
  *optflags = MOLFILE_RADIUS | MOLFILE_MASS;
  // copy from atoms
  data->read_structure(atoms);

  return VMDPLUGIN_SUCCESS;
}

int read_rmf_timestep(void *mydata, int /*natoms*/, molfile_timestep_t *frame) {
  std::cout << "read timestep" << std::endl;
  Data *data = reinterpret_cast<Data *>(mydata);
  if (data->read_next_frame(frame)) {
    return VMDPLUGIN_SUCCESS;
  } else {
    return MOLFILE_EOF;
  }
}

int read_rmf_bonds(void *mydata, int *nbonds, int **fromptr, int **toptr,
                   float **bondorderptr, int **bondtype, int *nbondtypes,
                   char ***bondtypename) {
  Data *data = reinterpret_cast<Data *>(mydata);
  data->read_bonds(nbonds, fromptr, toptr, bondorderptr, bondtype, nbondtypes,
                   bondtypename);
  // scan hierarchy and extract bonds
  return VMDPLUGIN_SUCCESS;
}

int read_rmf_bonds_and_restraints(void *mydata, int *nbonds, int **fromptr,
                                  int **toptr, float **bondorderptr,
                                  int **bondtype, int *nbondtypes,
                                  char ***bondtypename) {
  Data *data = reinterpret_cast<Data *>(mydata);
  data->read_bonds(nbonds, fromptr, toptr, bondorderptr, bondtype, nbondtypes,
                   bondtypename, true);
  // scan hierarchy and extract bonds
  return VMDPLUGIN_SUCCESS;
}

int read_rmf_graphics(void *mydata, int *nelem,
                      const molfile_graphics_t **gdata) {
  std::cout << "read graphics" << std::endl;
  Data *data = reinterpret_cast<Data *>(mydata);
  data->read_graphics(nelem, gdata);
  return VMDPLUGIN_SUCCESS;
}
int read_rmf_timestep_metadata(void *mydata,
                               molfile_timestep_metadata_t *tdata) {
  std::cout << "read metdata" << std::endl;
  Data *data = reinterpret_cast<Data *>(mydata);
  data->read_timestep_data(tdata);
  return VMDPLUGIN_SUCCESS;
}

void init_plugin(molfile_plugin_t &plugin) {
  memset(&plugin, 0, sizeof(molfile_plugin_t));
  plugin.abiversion = vmdplugin_ABIVERSION;
  plugin.type = MOLFILE_PLUGIN_TYPE;
  plugin.name = "rmf";
  plugin.prettyname = "RMF";
  plugin.author = "Daniel Russel";
  plugin.majorv = 0;
  plugin.minorv = 9;
  plugin.is_reentrant = VMDPLUGIN_THREADUNSAFE;
  plugin.open_file_read = open_rmf_read;
  plugin.read_structure = read_rmf_structure;
  plugin.read_bonds = read_rmf_bonds;
  plugin.read_rawgraphics = read_rmf_graphics;
  plugin.close_file_read = close_rmf_read;
  plugin.read_timestep_metadata = read_rmf_timestep_metadata;
  plugin.read_next_timestep = read_rmf_timestep;
}

void init_plugins() {
  std::cout << "Init" << std::endl;
  init_plugin(plugin);
  plugin.name = "rmf";
  plugin.prettyname = "RMF";
  plugin.filename_extension = "rmf";
  init_plugin(plugin3);
  plugin3.name = "rmf3";
  plugin3.prettyname = "RMF3";
  plugin3.filename_extension = "rmf3";
  init_plugin(pluginz);
  pluginz.name = "rmfz";
  pluginz.prettyname = "RMFz";
  pluginz.filename_extension = "rmfz";
  init_plugin(pluginrestraints);
  pluginrestraints.name = "rmf-with-restraints";
  pluginrestraints.prettyname = "RMF with restraints";
  pluginrestraints.filename_extension = "rmf-with-restraints";
  pluginrestraints.read_bonds = read_rmf_bonds_and_restraints;
}
}
