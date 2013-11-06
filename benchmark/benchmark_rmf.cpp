/**
 * Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#include <RMF/FileHandle.h>
#include <boost/timer.hpp>
#include <RMF/physics_decorators.h>
#include <RMF/sequence_decorators.h>
#include <boost/filesystem.hpp>
#include <sstream>

namespace {
#ifndef NDEBUG
const int scale = 1;
#else
const int scale = 5;
#endif

void benchmark_size(std::string path, std::string type) {
  unsigned int size = 0;
  if (boost::filesystem::is_directory(path)) {
    for (boost::filesystem::directory_iterator it(path);
         it != boost::filesystem::directory_iterator(); it++) {
      size += boost::filesystem::file_size(*it);
    }
  } else {
    size = boost::filesystem::file_size(path);
  }
  std::cout << type << ", size, " << size / 1000000 << "M, " << size
            << std::endl;
}

void create_residue(RMF::NodeHandle nh, RMF::AtomFactory af,
                    RMF::ParticleFactory pf) {
  for (unsigned int i = 0; i < 2 * scale; ++i) {
    RMF::NodeHandle child = nh.add_child("CA", RMF::REPRESENTATION);
    pf.get(child).set_mass(1);
    pf.get(child).set_radius(1.0 + i / 18.77);
    af.get(child).set_element(7);
  }
}
void create_chain(RMF::NodeHandle nh, RMF::ResidueFactory rf,
                  RMF::AtomFactory af, RMF::ParticleFactory pf) {
  for (unsigned int i = 0; i < 60 * scale; ++i) {
    std::ostringstream oss;
    oss << i;
    RMF::NodeHandle child = nh.add_child(oss.str(), RMF::REPRESENTATION);
    rf.get(child).set_type("cys");
    rf.get(child).set_index(i);
    create_residue(child, af, pf);
  }
}
void create_hierarchy(RMF::FileHandle file) {
  RMF::ChainFactory cf(file);
  RMF::AtomFactory af(file);
  RMF::ResidueFactory rf(file);
  RMF::ParticleFactory pf(file);
  RMF::NodeHandle n = file.get_root_node();
  for (unsigned int i = 0; i < 3 * scale; ++i) {
    std::ostringstream oss;
    oss << i;
    RMF::NodeHandle child = n.add_child(oss.str(), RMF::REPRESENTATION);
    cf.get(child).set_chain_id(oss.str());
    create_chain(child, rf, af, pf);
  }
}

double create_frame(RMF::FileHandle fh, RMF::IntermediateParticleFactory ipf,
                    const RMF::NodeIDs& atoms, int frame) {
  RMF::Vector3 ret(0, 0, 0);
  BOOST_FOREACH(RMF::NodeID n, atoms) {
    RMF::Vector3 v((n.get_index() + 0 + frame) / 17.0,
                   (n.get_index() + 1 + frame) / 19.0,
                   (n.get_index() + 2 + frame) / 23.0);
    ret[0] += v[0];
    ret[1] += v[1];
    ret[2] += v[2];
    ipf.get(fh.get_node(n)).set_coordinates(v);
  }
  return ret[0] + ret[1] + ret[2];
}

double create(RMF::FileHandle file) {
  create_hierarchy(file);
  RMF::NodeIDs atoms;
  BOOST_FOREACH(RMF::NodeID n, file.get_node_ids()) {
    if (file.get_node(n).get_children().empty()) {
      atoms.push_back(n);
    }
  }
  RMF::IntermediateParticleFactory ipf(file);
  double ret = 0;
  for (unsigned int i = 0; i < 20; ++i) {
    file.add_frame("frame", RMF::FRAME);
    ret += create_frame(file, ipf, atoms, i);
  }
  return ret;
}

double traverse(RMF::FileConstHandle file) {
  double ret = 0;
  RMF::NodeConstHandles queue(1, file.get_root_node());
  RMF::IntermediateParticleConstFactory ipcf(file);
  do {
    RMF::NodeConstHandle cur = queue.back();
    queue.pop_back();
    if (ipcf.get_is(cur)) {
      ret += ipcf.get(cur).get_radius();
    }
    RMF::NodeConstHandles children = cur.get_children();
    queue.insert(queue.end(), children.begin(), children.end());
  } while (!queue.empty());
  return ret;
}

double load(RMF::FileConstHandle file, const RMF::NodeIDs& nodes) {
  RMF::IntermediateParticleConstFactory ipcf(file);
  RMF::Vector3 v(0, 0, 0);
  BOOST_FOREACH(RMF::FrameID fr, file.get_frames()) {
    file.set_current_frame(fr);
    BOOST_FOREACH(RMF::NodeID n, nodes) {
      RMF::Vector3 cur = ipcf.get(file.get_node(n)).get_coordinates();
      v[0] += cur[0];
      v[1] += cur[1];
      v[2] += cur[2];
    }
  }
  return v[0] + v[1] + v[2];
}

void benchmark_create(RMF::FileHandle file, std::string type) {
  boost::timer timer;
  double dist = create(file);
  std::cout << type << ", create, " << timer.elapsed() << ", " << dist
            << std::endl;
}

void benchmark_traverse(RMF::FileConstHandle file, std::string type) {
  boost::timer timer;
  file.set_current_frame(RMF::FrameID(0));
  double t = traverse(file);
  std::cout << type << ", traverse, " << timer.elapsed() << ", " << t
            << std::endl;
}

void benchmark_load(RMF::FileConstHandle file, std::string type) {
  RMF::NodeIDs nodes;
  RMF::IntermediateParticleConstFactory ipcf(file);
  BOOST_FOREACH(RMF::NodeID n, file.get_node_ids()) {
    if (ipcf.get_is(file.get_node(n))) nodes.push_back(n);
  }
  boost::timer timer;
  double dist = load(file, nodes);
  std::cout << type << ", load, " << timer.elapsed() << ", " << dist
            << std::endl;
}
}

int main(int, char**) {
  try {
    {
      boost::filesystem::path temp = boost::filesystem::unique_path();
      const std::string name = temp.native() + ".rmf";
      {
        RMF::FileHandle fh = RMF::create_rmf_file(name);
        benchmark_create(fh, "rmf");
      }
      {
        RMF::FileConstHandle fh = RMF::open_rmf_file_read_only(name);
        benchmark_traverse(fh, "rmf");
        benchmark_load(fh, "rmf");
      }
      benchmark_size(name, "rmf");
    }
    {
      boost::filesystem::path temp = boost::filesystem::unique_path();
      const std::string name = temp.native() + ".rmf2";
      {
        RMF::FileHandle fh = RMF::create_rmf_file(name);
        benchmark_create(fh, "rmf2");
      }
      {
        RMF::FileConstHandle fh = RMF::open_rmf_file_read_only(name);
        benchmark_traverse(fh, "rmf2");
        benchmark_load(fh, "rmf2");
      }
      benchmark_size(name, "rmf2");
    }
    {
      boost::filesystem::path temp = boost::filesystem::unique_path();
      const std::string name = temp.native() + ".rmfa";
      {
        RMF::FileHandle fh = RMF::create_rmf_file(name);
        benchmark_create(fh, "rmfa");
      }
      {
        RMF::FileConstHandle fh = RMF::open_rmf_file_read_only(name);
        benchmark_traverse(fh, "rmfa");
        benchmark_load(fh, "rmfa");
      }
      benchmark_size(name, "rmfa");
    }
    {
      boost::filesystem::path temp = boost::filesystem::unique_path();
      const std::string name = temp.native() + ".rmft";
      {
        RMF::FileHandle fh = RMF::create_rmf_file(name);
        benchmark_create(fh, "rmft");
      }
      {
        RMF::FileConstHandle fh = RMF::open_rmf_file_read_only(name);
        benchmark_traverse(fh, "rmft");
        benchmark_load(fh, "rmft");
      }
      benchmark_size(name, "rmft");
    }
    {
      std::string buffer;
      RMF::FileHandle fh = RMF::create_rmf_buffer();
      benchmark_create(fh, "buffer");
      benchmark_traverse(fh, "buffer");
      benchmark_load(fh, "buffer");
    }
  }
  catch (const std::exception& e) {
    std::cerr << "Exception thrown: " << e.what() << std::endl;
  }
  return 0;
}
