#include "molfile_plugin.h"

#include <RMF/FileConstHandle.h>
#include <RMF/physics_decorators.h>
#include <RMF/sequence_decorators.h>
#include <RMF/shape_decorators.h>
#include <RMF/CoordinateTransformer.h>
#include <RMF/log.h>
#include <boost/scoped_array.hpp>
#include <boost/foreach.hpp>

namespace {
  molfile_plugin_t plugin;
  class Data {
    RMF::FileConstHandle file_;
    RMF::AtomConstFactory af_;
    RMF::ResidueConstFactory rf_;
    RMF::ChainConstFactory chf_;
    RMF::ParticleConstFactory pf_;
    RMF::ReferenceFrameConstFactory rff_;
    RMF::BallConstFactory bf_;
    RMF::SegmentConstFactory sf_;
    RMF::CylinderConstFactory cf_;
    RMF::BondConstFactory bdf_;
    struct Body {
      std::vector<RMF::ReferenceFrameConst> frames;
      RMF::NodeConstHandles atoms;
    };
    std::vector<Body> bodies_;
    unsigned int num_atoms_;

    boost::scoped_array<molfile_graphics_t> graphics_;
    boost::scoped_array<int> bonds_to_, bonds_from_;
    boost::scoped_array<char> bond_type_;
    char *bt_char_;
    bool done_;

    // find nodes to push to vmd
    int get_structure(RMF::NodeConstHandle cur, molfile_atom_t *atoms,
                      int body,
                      char chain, int resid, std::string resname);
    int get_graphics(RMF::NodeConstHandle cur,
                       RMF::CoordinateTransformer tr,
                       molfile_graphics_t *graphics);
    int get_bonds(RMF::NodeConstHandle cur,
                  const std::map<RMF::NodeConstHandle, int> &index,
                  int *from, int *to);
  public:
    Data(std::string name);
    void read_structure(molfile_atom_t *atoms);
    bool read_next_frame(molfile_timestep_t *frame);
    void read_graphics(int *nelem, const molfile_graphics_t **gdata);
    void read_bonds(int *nbonds, int **fromptr, int **toptr,
                    float **bondorderptr,  int **bondtype,
                    int *nbondtypes, char ***bondtypename);
    void read_timestep_data(molfile_timestep_metadata_t *data);
    unsigned int get_number_of_atoms() const {
      return num_atoms_;
    }
  };

  Data::Data(std::string name):
    file_(RMF::open_rmf_file_read_only(name)),
    af_(file_), rf_(file_), chf_(file_),
    pf_(file_), rff_(file_), bf_(file_), sf_(file_), cf_(file_), bdf_(file_),
    done_(false) {
    file_.set_current_frame(0);
    bodies_.push_back(Body());
    num_atoms_ = get_structure(file_.get_root_node(), NULL, 0,
                               ' ', -1, std::string());
  }

  //
  int Data::get_structure(RMF::NodeConstHandle cur, molfile_atom_t *atoms,
                          int body,
                          char chain, int resid, std::string resname) {
    if (cur.get_type() == RMF::ALIAS) return 0;
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
    int ret = 0;
    BOOST_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
      int count = get_structure(c, atoms, body, chain, resid, resname);
      ret += count;
      if (atoms) atoms += count;
    }

    if (ret == 0 && pf_.get_is(cur)) {
      if (atoms) {
        std::string nm = cur.get_name();
        std::string at;
        if (af_.get_is(cur)) {
          at = cur.get_name();
        }
        std::copy(nm.begin(), nm.end(), atoms->name);
        std::copy(at.begin(), at.end(), atoms->type);
        std::copy(resname.begin(), resname.end(), atoms->resname);
        atoms->resid = resid;
        atoms->chain[0] = chain;
        atoms->chain[1] = '\0';
        atoms->segid[0] = '\0';
        atoms->mass = pf_.get(cur).get_mass();
        atoms->radius = pf_.get(cur).get_radius();
      } else {
        bodies_[body].atoms.push_back(cur);
      }
      ++ret;
    }
    return ret;
  }

  void Data::read_structure(molfile_atom_t *atoms) {
    int found = get_structure(file_.get_root_node(),
                              atoms, 0, ' ', -1, "NONE");
    std::cout << "found " << found << " structural particles" << std::endl;
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
        RMF::Floats cc = pf_.get(bodies_[i].atoms[j]).get_coordinates();
        cc = tr.get_global_coordinates(cc);
        std::copy(cc.begin(), cc.end(), coords);
        coords += 3;
      }
    }
    RMF::FrameConstHandle curf = file_.get_current_frame();
    unsigned int next = curf.get_id().get_index() + 1;
    if (next == file_.get_number_of_frames()) {
      done_ = true;
    } else {
      file_.set_current_frame(curf.get_id().get_index() + 1);
  }
    return true;
 }

  void Data::read_graphics(int *nelem,
                           const molfile_graphics_t **gdata) {
    *nelem = get_graphics(file_.get_root_node(),
                          RMF::CoordinateTransformer(),
                          NULL);
    graphics_.reset(new molfile_graphics_t[*nelem]);
    *gdata = graphics_.get();
    get_graphics(file_.get_root_node(), RMF::CoordinateTransformer(),
                 graphics_.get());
  }

  void Data::read_bonds(int *nbonds, int **fromptr, int **toptr,
                        float **bondorderptr,  int **bondtype,
                        int *nbondtypes, char ***bondtypename) {
    std::map<RMF::NodeConstHandle, int> index;
    for (unsigned int i = 0; i < bodies_.size(); ++i) {
      for (unsigned int j = 0; j < bodies_[i].atoms.size(); ++j) {
        int c = index.size();
        index[bodies_[i].atoms[j]] = c;
      }
    }
    *nbonds = get_bonds(file_.get_root_node(), index, NULL, NULL);
    bonds_from_.reset(new int[*nbonds]);
    *fromptr = bonds_from_.get();
    bonds_to_.reset(new int[*nbonds]);
    *toptr = bonds_to_.get();
    *bondorderptr = NULL;
    *bondtype = NULL;
    *nbondtypes = 1;
    bond_type_.reset(new char[2]);
    bond_type_[0] = 'B';
    bond_type_[1] = '\0';
    bt_char_ = bond_type_.get();
    *bondtypename = &bt_char_;
    get_bonds(file_.get_root_node(), index, *fromptr, *toptr);
  }

  int Data::get_graphics(RMF::NodeConstHandle cur,
                         RMF::CoordinateTransformer tr,
                         molfile_graphics_t *graphics) {
    int ret = 0;
    if (graphics && rff_.get_is(cur)) {
      tr = RMF::CoordinateTransformer(tr, rff_.get(cur));
    }
    if (bf_.get_is(cur)) {
      if (graphics) {
        graphics->type = MOLFILE_SPHERE;
        graphics->size = bf_.get(cur).get_radius();
        graphics->style = 0;
        RMF::Floats coords = tr.get_global_coordinates(bf_.get(cur).get_coordinates());
        std::copy(coords.begin(), coords.end(), graphics->data);
        ++graphics;
      }
      ++ret;
    } else if (sf_.get_is(cur)) {
      RMF::SegmentConst s = sf_.get(cur);
      RMF::FloatsList coords = s.get_coordinates();
      int type = MOLFILE_LINE;
      double size = 0;
      if (cf_.get_is(cur)) {
        type = MOLFILE_CYLINDER;
        size = cf_.get(cur).get_radius();
      }
      RMF::Floats last_coords(3);
      if (graphics) {
        last_coords[0] = coords[0][0];
        last_coords[1] = coords[1][0];
        last_coords[2] = coords[2][0];
        last_coords = tr.get_global_coordinates(last_coords);
      }
      for (unsigned int i = 1; i < coords[0].size(); ++i) {
        if (graphics) {
          graphics->type = type;
          graphics->size = size;
          graphics->style = 0;
          std::copy(last_coords.begin(), last_coords.end(), graphics->data);
          last_coords[0] = coords[0][i];
          last_coords[1] = coords[1][i];
          last_coords[2] = coords[2][i];
          last_coords = tr.get_global_coordinates(last_coords);
          std::copy(last_coords.begin(), last_coords.end(), graphics->data+3);
          ++graphics;
        }
        ++ret;
      }
    }
    BOOST_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
      ret += get_graphics(c, tr, graphics);
      if (graphics) graphics += ret;
    }
    return ret;
  }
  int Data::get_bonds(RMF::NodeConstHandle cur,
                      const std::map<RMF::NodeConstHandle, int> &index,
                      int *from, int *to) {
    int ret = 0;
    if (bdf_.get_is(cur)) {
      RMF::NodeConstHandles bonded = bdf_.get(cur).get_bonded();
      if (bonded.size() == 2 && index.find(bonded[0]) != index.end()
          && index.find(bonded[1]) != index.end()) {
        if (from) {
          *from = index.find(bonded[0])->second + 1;
          *to = index.find(bonded[1])->second + 1;
          ++from;
          ++to;
        }
        ++ret;
      }
    }
    BOOST_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
      int ci = get_bonds(c, index, from, to);
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
    data->avg_bytes_per_timestep = sizeof(float)*3*get_number_of_atoms();
    data->has_velocities = 0;
  }

  void close_rmf_read(void *mydata) {
    Data *data = reinterpret_cast<Data*>(mydata);
    delete data;
  }

  void *open_rmf_read(const char *filename, const char *,
                      int *natoms) {
    Data *data = new Data(filename);
    *natoms = data->get_number_of_atoms();
    if (*natoms == 0) {
      *natoms = MOLFILE_NUMATOMS_NONE;
    }
    std::cout << "open_rmf_read " << filename
              << ": " << *natoms << std::endl;
    return data;
  }

  int read_rmf_structure(void *mydata, int *optflags,
                         molfile_atom_t *atoms) {
    std::cout << "read structure" << std::endl;
    Data *data = reinterpret_cast<Data*>(mydata);
    *optflags = MOLFILE_RADIUS | MOLFILE_MASS;
    // copy from atoms
    data->read_structure(atoms);

    return VMDPLUGIN_SUCCESS;
  }

  int read_rmf_timestep(void *mydata, int /*natoms*/, molfile_timestep_t *frame) {
    std::cout << "read timestep" << std::endl;
    Data *data = reinterpret_cast<Data*>(mydata);
    if (data->read_next_frame(frame)) {
    return VMDPLUGIN_SUCCESS;
    } else {
      return MOLFILE_EOF;
  }
  }

  int read_rmf_bonds(void *mydata, int *nbonds, int **fromptr, int **toptr,
                     float **bondorderptr,  int **bondtype,
                     int *nbondtypes, char ***bondtypename) {
    Data *data = reinterpret_cast<Data*>(mydata);
    data->read_bonds(nbonds, fromptr, toptr, bondorderptr, bondtype, nbondtypes,
                     bondtypename);
    // scan hierarchy and extract bonds
    return VMDPLUGIN_SUCCESS;
  }
  int read_rmf_graphics (void *mydata, int *nelem,
                         const molfile_graphics_t **gdata) {
    std::cout << "read graphics" << std::endl;
    Data *data = reinterpret_cast<Data*>(mydata);
    data->read_graphics(nelem, gdata);
    return VMDPLUGIN_SUCCESS;
  }
  int read_rmf_timestep_metadata(void *mydata,
                                 molfile_timestep_metadata_t *tdata) {
    std::cout << "read metdata" << std::endl;
    Data *data = reinterpret_cast<Data*>(mydata);
    data->read_timestep_data(tdata);
    return VMDPLUGIN_SUCCESS;
}
}

VMDPLUGIN_API int VMDPLUGIN_init () {
  std::cout << "Init" << std::endl;
  memset(&plugin, 0, sizeof(molfile_plugin_t));
  plugin.abiversion = vmdplugin_ABIVERSION;
  plugin.type = MOLFILE_PLUGIN_TYPE;
  plugin.name = "rmf";
  plugin.prettyname = "Rich Molecular Format";
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
  return VMDPLUGIN_SUCCESS;
}
VMDPLUGIN_API int VMDPLUGIN_register (void *v, vmdplugin_register_cb cb) {
  std::cout << "Register" << std::endl;
  plugin.filename_extension = "rmf";
  (*cb)(v, (vmdplugin_t *)&plugin);
  plugin.filename_extension = "rmf2";
  (*cb)(v, (vmdplugin_t *)&plugin);
  plugin.filename_extension = "rmfa";
  (*cb)(v, (vmdplugin_t *)&plugin);
  return 0;
}
VMDPLUGIN_API int VMDPLUGIN_fini () {
  std::cout << "Fini" << std::endl;
  return VMDPLUGIN_SUCCESS;
}
