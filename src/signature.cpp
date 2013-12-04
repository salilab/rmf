/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "RMF/utility.h"

#include <boost/shared_ptr.hpp>
#include <algorithm>
#include <iostream>
#include <string>

#include "RMF/signature.h"
#include "RMF/decorator/alias.h"
#include "RMF/decorator/physics.h"
#include "RMF/decorator/sequence.h"
#include "RMF/decorator/feature.h"
#include "RMF/decorator/shape.h"

RMF_ENABLE_WARNINGS

namespace RMF {

namespace {
std::string get_static_signature(
    FileConstHandle file, decorator::BondConstFactory bdcf,
    decorator::ColoredConstFactory ccf, decorator::ParticleConstFactory pcf,
    decorator::IntermediateParticleConstFactory ipcf,
    decorator::RigidParticleConstFactory rpcf, decorator::ScoreConstFactory scf,
    decorator::BallConstFactory bcf, decorator::CylinderConstFactory cycf,
    decorator::SegmentConstFactory segcf, decorator::ResidueConstFactory rcf,
    decorator::AtomConstFactory acf, decorator::ChainConstFactory chaincf,
    decorator::DomainConstFactory fragcf, decorator::CopyConstFactory copycf,
    decorator::DiffuserConstFactory diffusercf,
    decorator::TypedConstFactory typedcf) {
  std::ostringstream ret;
  ret << "hierarchy [\n";
  RMF_FOREACH(NodeID n, file.get_node_ids()) {
    NodeConstHandle nh = file.get_node(n);
    ret << n << ":";
    RMF_FOREACH(NodeConstHandle c, nh.get_children()) {
      ret << " " << c.get_id();
    }
    ret << "\n";
  }
  ret << "]\n";
  ret << "static [\n";
  RMF_FOREACH(NodeID n, file.get_node_ids()) {
    NodeConstHandle nh = file.get_node(n);
    ret << n << ":";
    if (bdcf.get_is_static(nh)) ret << " bond";
    if (ccf.get_is_static(nh)) ret << " color";
    if (pcf.get_is_static(nh)) ret << " particle";
    if (ipcf.get_is_static(nh)) ret << " iparticle";
    if (rpcf.get_is_static(nh)) ret << " rigid";
    if (scf.get_is_static(nh)) ret << " score";
    if (bcf.get_is_static(nh)) ret << " ball";
    if (cycf.get_is_static(nh)) ret << " cylinder";
    if (segcf.get_is_static(nh)) ret << " segment";
    if (rcf.get_is_static(nh)) ret << " residue";
    if (acf.get_is_static(nh)) ret << " atom";
    if (chaincf.get_is_static(nh)) ret << " chain";
    if (fragcf.get_is_static(nh)) ret << " domain";
    if (copycf.get_is_static(nh)) ret << " copy";
    if (typedcf.get_is_static(nh)) ret << " typed";
    if (diffusercf.get_is_static(nh)) ret << " diffuser";
    ret << "\n";
  }
  ret << "]\n";
  return ret.str();
}

std::string get_frame_signature(
    FileConstHandle file, decorator::BondConstFactory bdcf,
    decorator::ColoredConstFactory ccf, decorator::ParticleConstFactory pcf,
    decorator::IntermediateParticleConstFactory ipcf,
    decorator::RigidParticleConstFactory rpcf, decorator::ScoreConstFactory scf,
    decorator::BallConstFactory bcf, decorator::CylinderConstFactory cycf,
    decorator::SegmentConstFactory segcf, decorator::ResidueConstFactory rcf,
    decorator::AtomConstFactory acf, decorator::ChainConstFactory chaincf,
    decorator::DomainConstFactory fragcf, decorator::CopyConstFactory copycf,
    decorator::DiffuserConstFactory diffusercf,
    decorator::TypedConstFactory typedcf) {
  std::ostringstream ret;
  ret << file.get_current_frame() << " [\n";
  RMF_FOREACH(NodeID n, file.get_node_ids()) {
    NodeConstHandle nh = file.get_node(n);
    ret << n << ":";
    if (bdcf.get_is(nh)) ret << " bond";
    if (ccf.get_is(nh)) ret << " color";
    if (pcf.get_is(nh)) ret << " particle";
    if (ipcf.get_is(nh)) ret << " iparticle";
    if (rpcf.get_is(nh)) ret << " rigid";
    if (scf.get_is(nh)) ret << " score";
    if (bcf.get_is(nh)) ret << " ball";
    if (cycf.get_is(nh)) ret << " cylinder";
    if (segcf.get_is(nh)) ret << " segment";
    if (rcf.get_is(nh)) ret << " residue";
    if (acf.get_is(nh)) ret << " atom";
    if (chaincf.get_is(nh)) ret << " chain";
    if (fragcf.get_is(nh)) ret << " domain";
    if (copycf.get_is(nh)) ret << " copy";
    if (typedcf.get_is(nh)) ret << " typed";
    if (diffusercf.get_is(nh)) ret << " diffuser";
    ret << "\n";
  }
  ret << "]\n";
  return ret.str();
}
}

std::string get_signature_string(FileConstHandle file) {
  decorator::BondConstFactory bdf(file);
  decorator::ColoredConstFactory ccf(file);
  decorator::ParticleConstFactory pcf(file);
  decorator::IntermediateParticleConstFactory ipcf(file);
  decorator::RigidParticleConstFactory rpcf(file);
  decorator::ScoreConstFactory scf(file);
  decorator::BallConstFactory bcf(file);
  decorator::CylinderConstFactory cycf(file);
  decorator::SegmentConstFactory segcf(file);
  decorator::ResidueConstFactory rcf(file);
  decorator::AtomConstFactory acf(file);
  decorator::ChainConstFactory chaincf(file);
  decorator::DomainConstFactory fragcf(file);
  decorator::CopyConstFactory copycf(file);
  decorator::DiffuserConstFactory diffusercf(file);
  decorator::TypedConstFactory typedcf(file);

  std::string ret = get_static_signature(file, bdf, ccf, pcf, ipcf, rpcf, scf,
                                         bcf, cycf, segcf, rcf, acf, chaincf,
                                         fragcf, copycf, diffusercf, typedcf);
  RMF_FOREACH(FrameID frame, file.get_frames()) {
    file.set_current_frame(frame);
    ret += std::string("\n") + get_frame_signature(file, bdf, ccf, pcf, ipcf,
                                                   rpcf, scf, bcf, cycf, segcf,
                                                   rcf, acf, chaincf, fragcf,
                                                   copycf, diffusercf, typedcf);
  }
  return ret;
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
