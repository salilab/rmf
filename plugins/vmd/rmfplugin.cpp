/**
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */
#include "rmfplugin.h"
#include <RMF/exceptions.h>

namespace {

void close_rmf_read(void *mydata) {
  Data *data = reinterpret_cast<Data *>(mydata);
  delete data;
}

void *open_rmf_read(const char *filename, const char *, int *natoms) {
  RMF_TRACE("open_rmf_read");
  try {
    return new Data(filename, natoms);
  }
  catch (std::exception &e) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
    *natoms = MOLFILE_NUMATOMS_NONE;
    return NULL;
  }
}

int read_rmf_structure(void *mydata, int *optflags, molfile_atom_t *atoms) {
  RMF_TRACE("read_rmf_structure");
  try {
    Data *data = reinterpret_cast<Data *>(mydata);
    return data->read_structure(optflags, atoms);
  }
  catch (const std::exception &e) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
    return VMDPLUGIN_ERROR;
  }
}

int read_rmf_timestep(void *mydata, int natoms, molfile_timestep_t *frame) {
  RMF_TRACE("Begin read_rmf_timestep: " << natoms);
  try {
    Data *data = reinterpret_cast<Data *>(mydata);
    return data->read_timestep(frame);
  }
  catch (const std::exception &e) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
    return VMDPLUGIN_ERROR;
  }
}

int read_rmf_bonds(void *mydata, int *nbonds, int **fromptr, int **toptr,
                   float **bondorderptr, int **bondtype, int *nbondtypes,
                   char ***bondtypename) {
  RMF_TRACE("Begin read_rmf_bonds");
  try {
    Data *data = reinterpret_cast<Data *>(mydata);
    return data->read_bonds(nbonds, fromptr, toptr, bondorderptr, bondtype,
                            nbondtypes, bondtypename);
  }
  catch (const std::exception &e) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
    return VMDPLUGIN_ERROR;
  }
}

int read_rmf_graphics(void *mydata, int *nelem,
                      const molfile_graphics_t **gdata) {
  RMF_TRACE("read_rmf_graphics");
  try {
    Data *data = reinterpret_cast<Data *>(mydata);
    return data->read_graphics(nelem, gdata);
  }
  catch (const std::exception &e) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
    return VMDPLUGIN_ERROR;
  }
}
int read_rmf_timestep_metadata(void *mydata,
                               molfile_timestep_metadata_t *tdata) {
  RMF_TRACE("read_rmf_timestep_metadata");
  try {
    Data *data = reinterpret_cast<Data *>(mydata);
    return data->read_timestep_metadata(tdata);
  }
  catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return VMDPLUGIN_ERROR;
  }
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
  // try this
  plugin.is_reentrant = VMDPLUGIN_THREADSAFE;
  plugin.open_file_read = open_rmf_read;
  plugin.read_structure = read_rmf_structure;
  plugin.read_bonds = read_rmf_bonds;
  plugin.read_rawgraphics = read_rmf_graphics;
  plugin.close_file_read = close_rmf_read;
  plugin.read_timestep_metadata = read_rmf_timestep_metadata;
  plugin.read_next_timestep = read_rmf_timestep;
}

void init_plugins() {
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
  // init_plugin(pluginrestraints);
  // pluginrestraints.name = "rmf-with-restraints";
  // pluginrestraints.prettyname = "RMF with restraints";
  // pluginrestraints.filename_extension = "rmf-with-restraints";
  // pluginrestraints.read_bonds = read_rmf_bonds_and_restraints;
}
}

VMDPLUGIN_API int VMDPLUGIN_init() {
  init_plugins();
  RMF::set_log_level("trace");
  return VMDPLUGIN_SUCCESS;
}
VMDPLUGIN_API int VMDPLUGIN_register(void *v, vmdplugin_register_cb cb) {
  std::cout << "Register" << std::endl;
  (*cb)(v, (vmdplugin_t *)&plugin);
  (*cb)(v, (vmdplugin_t *)&plugin3);
  (*cb)(v, (vmdplugin_t *)&pluginz);
  return 0;
}
VMDPLUGIN_API int VMDPLUGIN_fini() {
  std::cout << "Fini" << std::endl;
  return VMDPLUGIN_SUCCESS;
}
