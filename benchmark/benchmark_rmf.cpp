/** \example benchmark/benchmark_rmf.cpp
 * \brief Benchmark typical creation, traversal and loading with different RMF
 *        backends.
 *
 * Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#include <boost/iterator/iterator_facade.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/timer.hpp>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "RMF/BufferHandle.h"
#include "RMF/FileConstHandle.h"
#include "RMF/FileHandle.h"
#include "RMF/ID.h"
#include "RMF/NodeConstHandle.h"
#include "RMF/NodeHandle.h"
#include "RMF/Vector.h"
#include "RMF/decorator/physics.h"
#include "RMF/decorator/sequence.h"
#include "RMF/enums.h"
#include "RMF/infrastructure_macros.h"
#include "RMF/log.h"

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

std::size_t create_residue(RMF::NodeHandle nh, RMF::decorator::AtomFactory af,
                           RMF::decorator::ParticleFactory pf) {
  std::size_t total_size = 0;
  for (unsigned int i = 0; i < 2 * scale; ++i) {
    RMF::NodeHandle child = nh.add_child("CA", RMF::REPRESENTATION);
    pf.get(child).set_static_mass(1);
    pf.get(child).set_static_radius(1.0 + i / 18.77);
    af.get(child).set_static_element(7);
    total_size += sizeof(int) * 1 + sizeof(float) * 2;
  }
  return total_size;
}
std::size_t create_chain(RMF::NodeHandle nh, RMF::decorator::ResidueFactory rf,
                         RMF::decorator::AtomFactory af,
                         RMF::decorator::ParticleFactory pf) {
  std::size_t total_size = 0;
  for (unsigned int i = 0; i < 60 * scale; ++i) {
    std::ostringstream oss;
    oss << i;
    RMF::NodeHandle child = nh.add_child(oss.str(), RMF::REPRESENTATION);
    rf.get(child).set_static_residue_type("cys");
    rf.get(child).set_static_residue_index(i);
    total_size += sizeof(int) + 4;
    total_size += create_residue(child, af, pf);
  }
  return total_size;
}
std::size_t create_hierarchy(RMF::FileHandle file) {
  RMF::decorator::ChainFactory cf(file);
  RMF::decorator::AtomFactory af(file);
  RMF::decorator::ResidueFactory rf(file);
  RMF::decorator::ParticleFactory pf(file);
  RMF::NodeHandle n = file.get_root_node();
  std::size_t total_size = 0;
  for (unsigned int i = 0; i < 3 * scale; ++i) {
    std::ostringstream oss;
    oss << i;
    RMF::NodeHandle child = n.add_child(oss.str(), RMF::REPRESENTATION);
    cf.get(child).set_static_chain_id(oss.str());
    total_size += oss.str().size();
    total_size += create_chain(child, rf, af, pf);
  }
  return total_size;
}

std::pair<double, std::size_t> create_frame(
    RMF::FileHandle fh, RMF::decorator::IntermediateParticleFactory ipf,
    const RMF::NodeIDs& atoms, int frame) {
  RMF::Vector3 ret(0, 0, 0);
  std::size_t total_size = 0;
  RMF_FOREACH(RMF::NodeID n, atoms) {
    RMF::Vector3 v((n.get_index() + 0 + frame) / 17.0,
                   (n.get_index() + 1 + frame) / 19.0,
                   (n.get_index() + 2 + frame) / 23.0);
    ret[0] += v[0];
    ret[1] += v[1];
    ret[2] += v[2];
    ipf.get(fh.get_node(n)).set_frame_coordinates(v);
    total_size += sizeof(float) * 3;
  }
  return std::make_pair(ret[0] + ret[1] + ret[2], total_size);
}

boost::tuple<double, std::size_t, std::size_t> create(RMF::FileHandle file) {
  std::size_t hierarchy_size = create_hierarchy(file);
  RMF::NodeIDs atoms;
  RMF_FOREACH(RMF::NodeID n, file.get_node_ids()) {
    if (file.get_node(n).get_children().empty()) {
      atoms.push_back(n);
    }
  }
  RMF::decorator::IntermediateParticleFactory ipf(file);
  double check_value = 0;
  std::size_t frame_size = 0;
  for (unsigned int i = 0; i < 20; ++i) {
    file.add_frame("frame", RMF::FRAME);
    std::pair<double, std::size_t> cur = create_frame(file, ipf, atoms, i);
    check_value += cur.first;
    frame_size += cur.second;
  }
  return boost::make_tuple(check_value, hierarchy_size, frame_size);
}

double traverse(RMF::FileConstHandle file) {
  double ret = 0;
  RMF::NodeConstHandles queue(1, file.get_root_node());
  RMF::decorator::IntermediateParticleConstFactory ipcf(file);
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
  RMF::decorator::IntermediateParticleConstFactory ipcf(file);
  RMF::Vector3 v(0, 0, 0);
  RMF_FOREACH(RMF::FrameID fr, file.get_frames()) {
    file.set_current_frame(fr);
    RMF_FOREACH(RMF::NodeID n, nodes) {
      RMF::Vector3 cur = ipcf.get(file.get_node(n)).get_coordinates();
      v[0] += cur[0];
      v[1] += cur[1];
      v[2] += cur[2];
    }
  }
  return v[0] + v[1] + v[2];
}

std::pair<std::size_t, std::size_t> benchmark_create(RMF::FileHandle file,
                                                     std::string type) {
  boost::timer timer;
  boost::tuple<double, std::size_t, std::size_t> cur = create(file);
  std::cout << type << ", create, " << timer.elapsed() << ", " << cur.get<0>()
            << std::endl;
  return std::make_pair(cur.get<1>(), cur.get<2>());
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
  RMF::decorator::IntermediateParticleConstFactory ipcf(file);
  RMF_FOREACH(RMF::NodeID n, file.get_node_ids()) {
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
    RMF::set_log_level("Off");
    std::string name_base = RMF::internal::get_unique_path();

    {
      const std::string name = name_base + ".rmf3";
      {
        RMF::FileHandle fh = RMF::create_rmf_file(name);
        std::pair<std::size_t, std::size_t> sizes =
            benchmark_create(fh, "rmf3");
        std::cout << "raw, total, " << (sizes.first + sizes.second) / 1000000
                  << "M, " << (sizes.first + sizes.second) << std::endl;
        std::cout << "raw, frame, " << sizes.second / 1000000 << "M, "
                  << sizes.second << std::endl;
      }
      {
        RMF::FileConstHandle fh = RMF::open_rmf_file_read_only(name);
        benchmark_traverse(fh, "rmf3");
        benchmark_load(fh, "rmf3");
      }
      benchmark_size(name, "rmf3");
    }
    {
      const std::string name = name_base + ".rmf";
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
      const std::string name = name_base + ".rmfz";
      {
        RMF::FileHandle fh = RMF::create_rmf_file(name);
        benchmark_create(fh, "rmfz");
      }
      {
        RMF::FileConstHandle fh = RMF::open_rmf_file_read_only(name);
        benchmark_traverse(fh, "rmfz");
        benchmark_load(fh, "rmfz");
      }
      benchmark_size(name, "rmfz");
    }
    {
      RMF::BufferHandle buffer;
      {
        RMF::FileHandle fh = RMF::create_rmf_buffer(buffer);
        benchmark_create(fh, "buffer");
      }
      {
        RMF::FileConstHandle fh = RMF::open_rmf_buffer_read_only(buffer);
        benchmark_traverse(fh, "buffer");
        benchmark_load(fh, "buffer");
      }
    }
    {
      const std::string name = name_base + ".rmf2";
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
      const std::string name = name_base + ".rmfa";
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
  }
  catch (const std::exception& e) {
    std::cerr << "Exception thrown: " << e.what() << std::endl;
  }
  return 0;
}
