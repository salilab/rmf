#include "molfile_plugin.h"

#include <RMF/FileConstHandle.h>
#include <RMF/physics_decorators.h>
#include <RMF/sequence_decorators.h>
#include <RMF/shape_decorators.h>
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
    RMF::BallConstFactory bf_;
    RMF::SegmentConstFactory sf_;
    RMF::CylinderConstFactory cf_;
    RMF::BondConstFactory bdf_;
    RMF::NodeConstHandles atoms_;

    boost::scoped_array<molfile_graphics_t> graphics_;
    boost::scoped_array<int> bonds_to_, bonds_from_;

    // find nodes to push to vmd
    int get_structure(RMF::NodeConstHandle cur, molfile_atom_t *atoms,
                      char chain, int resid, std::string resname);
    int get_graphics(RMF::NodeConstHandle cur, molfile_graphics_t *graphics);
    int get_bonds(RMF::NodeConstHandle cur,
                  const std::map<RMF::NodeConstHandle, int> &index,
                  int *from, int *to);
  public:
    Data(std::string name);
    void fill_structure(molfile_atom_t *atoms) {
      get_structure(file_.get_root_node(), atoms,
                    ' ', -1, "NONE");
    }
    void read_next_frame(molfile_timestep_t *frame);
    void read_graphics(int *nelem, const molfile_graphics_t **gdata);
    void read_bonds(int *nbonds, int **fromptr, int **toptr,
                    float **bondorderptr,  int **bondtype,
                    int *nbondtypes, char ***bondtypename);
    unsigned int get_number_of_atoms() const {
      return atoms_.size();
    }
  };

  Data::Data(std::string name):
    file_(RMF::open_rmf_file_read_only(name)),
    af_(file_), rf_(file_), chf_(file_),
    pf_(file_), bf_(file_), sf_(file_), cf_(file_), bdf_(file_) {
    get_structure(file_.get_root_node(), NULL,
                  ' ', -1, std::string());
  }

  //
  int Data::get_structure(RMF::NodeConstHandle cur, molfile_atom_t *atoms,
                          char chain, int resid, std::string resname) {
    if (chf_.get_is(cur)) {
      chain = chf_.get(cur).get_chain_id();
    }
    if (rf_.get_is(cur)) {
      resid = rf_.get(cur).get_index();
      resname = rf_.get(cur).get_type();
    }
    int ret = 0;
    BOOST_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
      int count = get_structure(c, atoms,
                                chain, resid, resname);
      ret += count;
      if (atoms) atoms += count;
    }
    if (ret == 0 && pf_.get_is(cur)) {
      std::string nm = cur.get_name();
      std::copy(nm.begin(), nm.end(), atoms->name);
      std::string at;
      if (af_.get_is(cur)) {
        at = cur.get_name();
      }
      std::copy(at.begin(), at.end(), atoms->type);
      std::copy(resname.begin(), resname.end(), atoms->resname);
      atoms->resid = resid;
      atoms->chain[0] = chain;
      atoms->chain[1] = '\0';
      atoms->segid[0] = '\0';
      atoms->mass = pf_.get(cur).get_mass();
      atoms->radius = pf_.get(cur).get_radius();
      ++ret;
    }
    return ret;
  }

  void Data::read_next_frame(molfile_timestep_t *frame) {
    RMF::FrameConstHandle curf = file_.get_current_frame();
    RMF::FrameConstHandle nextf = file_.get_frame(curf.get_id().get_index() + 1);
    nextf.set_as_current_frame();
    frame->physical_time = 0;
    for (unsigned int i = 0; i < atoms_.size(); ++i) {
      RMF::Floats coords = pf_.get(atoms_[i]).get_coordinates();
      std::copy(coords.begin(), coords.end(), frame->coords);
      frame->coords += 3;
    }
  }

  void Data::read_graphics(int *nelem, const molfile_graphics_t **gdata) {
    *nelem = get_graphics(file_.get_root_node(), NULL);
    graphics_.reset(new molfile_graphics_t[*nelem]);
    *gdata = graphics_.get();
    get_graphics(file_.get_root_node(), graphics_.get());
  }

  void Data::read_bonds(int *nbonds, int **fromptr, int **toptr,
                        float **bondorderptr,  int **bondtype,
                        int *nbondtypes, char ***bondtypename) {
    std::map<RMF::NodeConstHandle, int> index;
    for (unsigned int i = 0; i < atoms_.size(); ++i) {
      index[atoms_[i]] = i;
    }
    *nbonds = get_bonds(file_.get_root_node(), index, NULL, NULL);
    bonds_from_.reset(new int[*nbonds]);
    *fromptr = bonds_from_.get();
    bonds_to_.reset(new int[*nbonds]);
    *toptr = bonds_to_.get();
    *bondorderptr = NULL;
    *bondtype = NULL;
    *nbondtypes = 0;
    *bondtypename = NULL;
    get_bonds(file_.get_root_node(), index, *fromptr, *toptr);
  }

  int Data::get_graphics(RMF::NodeConstHandle cur, molfile_graphics_t *graphics) {
    int ret = 0;
    if (bf_.get_is(cur)) {
      if (graphics) {
        graphics->type = MOLFILE_SPHERE;
        graphics->size = bf_.get(cur).get_radius();
        graphics->style = 0;
        RMF::Floats coords = bf_.get(cur).get_coordinates();
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
      for (unsigned int i = 1; i < coords[0].size(); ++i) {
        if (graphics) {
          graphics->type = type;
          graphics->size = size;
          graphics->style = 0;
          graphics->data[0] = coords[0][i-1];
          graphics->data[1] = coords[1][i-1];
          graphics->data[2] = coords[2][i-1];
          graphics->data[3] = coords[0][i];
          graphics->data[4] = coords[1][i];
          graphics->data[5] = coords[2][i];
          ++graphics;
        }
        ++ret;
      }
    }
    BOOST_FOREACH(RMF::NodeConstHandle c, cur.get_children()) {
      ret += get_graphics(c, graphics);
      if (graphics) graphics += ret;
    }
    return ret;
  }
  int Data::get_bonds(RMF::NodeConstHandle cur,
                      const std::map<RMF::NodeConstHandle, int> &index,
                      int *from, int *to) {
    int ret;
    if (bdf_.get_is(cur)) {
      RMF::NodeConstHandles bonded = bdf_.get(cur).get_bonded();
      if (bonded.size() == 2 && index.find(bonded[0]) != index.end()
          && index.find(bonded[1]) != index.end()) {
        if (from) {
          *from = index.find(bonded[0])->second;
          *to = index.find(bonded[1])->second;
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

  void close_rmf_read(void *mydata) {
    Data *data = reinterpret_cast<Data*>(mydata);
    delete data;
  }
  void *open_rmf_read(const char *filename, const char *,
                      int *natoms) {
    Data *data = new Data(filename);
    *natoms = data->get_number_of_atoms();
    return data;
  }
  int read_rmf_structure(void *mydata, int *optflags,
                         molfile_atom_t *atoms) {
    Data *data = reinterpret_cast<Data*>(mydata);
    *optflags = MOLFILE_RADIUS | MOLFILE_MASS;
    // copy from atoms
    data->fill_structure(atoms);
    return VMDPLUGIN_SUCCESS;
  }

  int read_rmf_timestep(void *mydata, int natoms, molfile_timestep_t *frame,
                        molfile_qm_metadata_t *, molfile_qm_timestep_t *) {
    Data *data = reinterpret_cast<Data*>(mydata);
    data->read_next_frame(frame);
    return VMDPLUGIN_SUCCESS;
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
    Data *data = reinterpret_cast<Data*>(mydata);
    data->read_graphics(nelem, gdata);
    return VMDPLUGIN_SUCCESS;
  }
}

VMDPLUGIN_API int VMDPLUGIN_init () {
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
  return VMDPLUGIN_SUCCESS;
}
VMDPLUGIN_API int VMDPLUGIN_register (void *v, vmdplugin_register_cb cb) {
  plugin.filename_extension = "rmf";
  (*cb)(v, (vmdplugin_t *)&plugin);
  plugin.filename_extension = "rmf2";
  (*cb)(v, (vmdplugin_t *)&plugin);
  plugin.filename_extension = "rmfa";
  (*cb)(v, (vmdplugin_t *)&plugin);
  return 0;
}
VMDPLUGIN_API int VMDPLUGIN_fini () {
  return VMDPLUGIN_SUCCESS;
}
