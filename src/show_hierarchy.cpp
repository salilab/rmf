/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "RMF/show_hierarchy.h"
#include "RMF/NodeConstHandle.h"
#include "RMF/Nullable.h"
#include "RMF/compiler_macros.h"
#include "RMF/decorator/feature.h"
#include "RMF/decorator/physics.h"
#include "RMF/decorator/sequence.h"
#include "RMF/decorator/alternatives.h"
#include "RMF/decorator/shape.h"
#include "RMF/enums.h"
#include "RMF/infrastructure_macros.h"
#include "RMF/types.h"

RMF_ENABLE_WARNINGS

namespace RMF {

namespace {
template <class Traits>
void show_data(NodeConstHandle n, std::ostream& out,
               const std::vector<ID<Traits> >& ks, std::string prefix) {
  using std::operator<<;
  FileConstHandle f = n.get_file();
  RMF_FOREACH(ID<Traits> k, ks) {
    if (n.get_file().get_current_frame() != FrameID() &&
        !n.get_frame_value(k).get_is_null()) {
      out << std::endl << prefix << n.get_file().get_name(k) << ": "
          << Showable(n.get_frame_value(k));
    } else {
      Nullable<Traits> ts = n.get_static_value(k);
      if (!ts.get_is_null()) {
        out << std::endl << prefix << f.get_name(k) << " ("
            << f.get_name(f.get_category(k)) << ")"
            << ": " << Showable(ts.get()) << "(s)";
      }
    }
  }
}

void show_node(NodeConstHandle n, std::string node_suffix, std::ostream& out,
               std::string prefix = "") {
  using std::operator<<;
  out << prefix << "\"" << n.get_name() << "\"" << node_suffix << " ["
      << n.get_type() << "]";
}
void show_node(NodeConstHandle n, std::string node_suffix, std::ostream& out,
               FloatKeys fks, FloatsKeys fsks, IntKeys iks, IntsKeys isks,
               StringKeys sks, StringsKeys ssks, Vector3Keys v3ks,
               Vector4Keys v4ks, Vector3sKeys v3sks, std::string prefix) {
  using std::operator<<;
  if (true) {
    show_node(n, node_suffix, out);
    show_data(n, out, fks, prefix + "  ");
    show_data(n, out, iks, prefix + "  ");
    show_data(n, out, sks, prefix + "  ");
    show_data(n, out, fsks, prefix + "  ");
    show_data(n, out, isks, prefix + "  ");
    show_data(n, out, ssks, prefix + "  ");
    show_data(n, out, v3ks, prefix + "  ");
    show_data(n, out, v4ks, prefix + "  ");
    show_data(n, out, v3sks, prefix + "  ");
  }
}

void show_node_decorators(
    NodeConstHandle n, std::string node_suffix, std::ostream& out,
    decorator::BondFactory bdcf, decorator::ColoredFactory ccf,
    decorator::ParticleFactory pcf, decorator::IntermediateParticleFactory ipcf,
    decorator::RigidParticleFactory rpcf, decorator::ScoreFactory scf,
    decorator::RepresentationFactory repcf, decorator::BallFactory bcf,
    decorator::CylinderFactory cycf, decorator::SegmentFactory segcf,
    decorator::ResidueFactory rcf, decorator::AtomFactory acf,
    decorator::ChainFactory chaincf, decorator::DomainFactory fragcf,
    decorator::CopyFactory copycf, decorator::DiffuserFactory diffusercf,
    decorator::TypedFactory typedcf, std::string) {
  using std::operator<<;
  out << "\"" << n.get_name() << "\"" << node_suffix << " [" << n.get_type()
      << ":";
  if (bdcf.get_is(n)) out << " bond";
  if (ccf.get_is(n)) out << " color";
  if (pcf.get_is(n))
    out << " particle";
  else if (ipcf.get_is(n))
    out << " iparticle";
  if (rpcf.get_is(n)) out << " rigid";
  if (scf.get_is(n)) out << " score";
  if (repcf.get_is(n)) out << " representation";
  if (bcf.get_is(n)) out << " ball";
  if (cycf.get_is(n)) out << " cylinder";
  if (segcf.get_is(n)) out << " segment";
  if (rcf.get_is(n)) out << " residue";
  if (acf.get_is(n)) out << " atom";
  if (chaincf.get_is(n)) out << " chain";
  if (fragcf.get_is(n)) out << " domain";
  if (copycf.get_is(n)) out << " copy";
  if (typedcf.get_is(n)) out << " typed";
  if (diffusercf.get_is(n)) out << " diffuser";
  out << "]";
}

template <class TypeT>
std::vector<ID<TypeT> > get_keys(FileConstHandle f) {
  Categories kcs = f.get_categories();
  std::vector<ID<TypeT> > ret;
  for (unsigned int i = 0; i < kcs.size(); ++i) {
    std::vector<ID<TypeT> > curp = f.get_keys<TypeT>(kcs[i]);
    ret.insert(ret.end(), curp.begin(), curp.end());
  }
  return ret;
}

// Note that older g++ is confused by queue.back().get<2>()
#define RMF_PRINT_TREE(stream, start, show)                                  \
  {                                                                          \
    decorator::AlternativesFactory altcf(root.get_file());                   \
    typedef boost::tuple<std::string, std::string, std::string,              \
                         NodeConstHandle> QI;                                \
    std::vector<QI> queue;                                                   \
    queue.push_back(QI(std::string(), std::string(), std::string(), start)); \
    do {                                                                     \
      const QI& back = queue.back();                                         \
      NodeConstHandle n = back.get<3>();                                     \
      std::string prefix0 = back.get<0>();                                   \
      std::string prefix1 = back.get<1>();                                   \
      std::string node_suffix = back.get<2>();                               \
      queue.pop_back();                                                      \
      if (altcf.get_is(n)) {                                                 \
        decorator::AlternativesConst ad = altcf.get(n);                      \
        NodeConstHandles alts = ad.get_alternatives(decorator::PARTICLE);    \
        std::reverse(alts.begin(), alts.end());                              \
        alts.pop_back();                                                     \
        RMF_FOREACH(NodeConstHandle cur, alts) {                             \
          std::ostringstream oss;                                            \
          oss << "[" << ad.get_resolution(cur.get_id()) << "]";              \
          queue.push_back(QI(prefix0, prefix1, oss.str(), cur));             \
        }                                                                    \
        std::ostringstream oss;                                              \
        oss << "[" << ad.get_resolution(n.get_id()) << "]";                  \
        node_suffix = oss.str();                                             \
      }                                                                      \
      stream << prefix0;                                                     \
      NodeConstHandles children = n.get_children();                          \
      if (children.size() > 0)                                               \
        stream << " + ";                                                     \
      else                                                                   \
        stream << " - ";                                                     \
      show;                                                                  \
      stream << std::endl;                                                   \
      for (int i = static_cast<int>(children.size()) - 1; i >= 0; --i) {     \
        queue.push_back(                                                     \
            QI(prefix1 + "  ", prefix1 + "  ", node_suffix, children[i]));   \
      }                                                                      \
    } while (!queue.empty());                                                \
  }
}

void show_hierarchy(NodeConstHandle root, std::ostream& out) {
  using std::operator<<;
  decorator::AlternativesFactory altcf(root.get_file());
  RMF_PRINT_TREE(out, root, show_node(n, node_suffix, out));
}

namespace {
struct LessName {
  FileConstHandle fh_;
  LessName(FileConstHandle fh) : fh_(fh) {}
  template <class Traits>
  bool operator()(ID<Traits> a, ID<Traits> b) const {
    return fh_.get_name(a) < fh_.get_name(b);
  }
};
}

void show_hierarchy_with_values(NodeConstHandle root, std::ostream& out) {
  FloatKeys fks;
  IntKeys iks;
  StringKeys sks;
  FloatsKeys fsks;
  IntsKeys isks;
  StringsKeys ssks;
  Vector3Keys v3ks;
  Vector4Keys v4ks;
  Vector3sKeys v3sks;
  fks = get_keys<FloatTraits>(root.get_file());
  std::sort(fks.begin(), fks.end(), LessName(root.get_file()));
  iks = get_keys<IntTraits>(root.get_file());
  std::sort(iks.begin(), iks.end(), LessName(root.get_file()));
  sks = get_keys<StringTraits>(root.get_file());
  std::sort(sks.begin(), sks.end(), LessName(root.get_file()));
  fsks = get_keys<FloatsTraits>(root.get_file());
  std::sort(fsks.begin(), fsks.end(), LessName(root.get_file()));
  isks = get_keys<IntsTraits>(root.get_file());
  std::sort(isks.begin(), isks.end(), LessName(root.get_file()));
  ssks = get_keys<StringsTraits>(root.get_file());
  std::sort(ssks.begin(), ssks.end(), LessName(root.get_file()));
  v3ks = get_keys<Vector3Traits>(root.get_file());
  std::sort(v3ks.begin(), v3ks.end(), LessName(root.get_file()));
  v4ks = get_keys<Vector4Traits>(root.get_file());
  std::sort(v4ks.begin(), v4ks.end(), LessName(root.get_file()));
  v3sks = get_keys<Vector3sTraits>(root.get_file());
  std::sort(v3sks.begin(), v3sks.end(), LessName(root.get_file()));
  using std::operator<<;

  RMF_PRINT_TREE(out, root,
                 show_node(n, node_suffix, out, fks, fsks, iks, isks, sks, ssks,
                           v3ks, v4ks, v3sks, prefix0 + "   "));
}

void show_hierarchy_with_decorators(NodeConstHandle root, bool,
                                    std::ostream& out) {
  decorator::BondFactory bdf(root.get_file());
  decorator::ColoredFactory ccf(root.get_file());
  decorator::ParticleFactory pcf(root.get_file());
  decorator::IntermediateParticleFactory ipcf(root.get_file());
  decorator::RigidParticleFactory rpcf(root.get_file());
  decorator::ScoreFactory scf(root.get_file());
  decorator::RepresentationFactory repcf(root.get_file());
  decorator::BallFactory bcf(root.get_file());
  decorator::CylinderFactory cycf(root.get_file());
  decorator::SegmentFactory segcf(root.get_file());
  decorator::ResidueFactory rcf(root.get_file());
  decorator::AtomFactory acf(root.get_file());
  decorator::ChainFactory chaincf(root.get_file());
  decorator::DomainFactory fragcf(root.get_file());
  decorator::CopyFactory copycf(root.get_file());
  decorator::DiffuserFactory diffusercf(root.get_file());
  decorator::TypedFactory typedcf(root.get_file());
  using std::operator<<;
  RMF_PRINT_TREE(
      out, root,
      show_node_decorators(n, node_suffix, out, bdf, ccf, pcf, ipcf, rpcf, scf,
                           repcf, bcf, cycf, segcf, rcf, acf, chaincf, fragcf,
                           copycf, diffusercf, typedcf, prefix0 + "   "));
}

namespace {
void show_frames_impl(FileConstHandle fh, FrameID root, std::string prefix,
                      std::ostream& out) {
  FrameIDs ch = fh.get_children(root);
  out << prefix;
  if (ch.empty()) {
    out << " - ";
  } else {
    out << " + ";
  }
  out << fh.get_name(root) << " [" << fh.get_type(root) << "]" << std::endl;
  RMF_FOREACH(FrameID id, ch) { show_frames_impl(fh, id, prefix + "   ", out); }
}
}

void show_frames(FileConstHandle fh, std::ostream& out) {
  RMF_FOREACH(FrameID fr, fh.get_root_frames()) {
    show_frames_impl(fh, fr, std::string(), out);
  }
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
