/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/NodeConstHandle.h>
#include <boost/tuple/tuple.hpp>
#include <RMF/FileHandle.h>
#include <RMF/decorators.h>

RMF_ENABLE_WARNINGS RMF_VECTOR_DEF(NodeConstHandle);

namespace RMF {

NodeConstHandle::NodeConstHandle(NodeID node,
                                 boost::shared_ptr<internal::SharedData> shared)
    : node_(node), shared_(shared) {}

FileConstHandle NodeConstHandle::get_file() const {
  return FileConstHandle(shared_);
}

std::string NodeConstHandle::get_file_name() const {
  return get_file().get_name();
}
FrameID NodeConstHandle::get_current_frame_id() const {
  return get_file().get_current_frame();
}

#define RMF_HDF5_NODE_CONST_KEY_TYPE_METHODS_DEF(                       \
    lcname, UCName, PassValue, ReturnValue, PassValues, ReturnValues)   \
  std::string NodeConstHandle::get_category_name(UCName##Key k) const { \
    return get_file().get_name(get_file().get_category(k));             \
  }                                                                     \
  std::string NodeConstHandle::get_name(UCName##Key k) const {          \
    return get_file().get_name(k);                                      \
  }

RMF_FOREACH_TYPE(RMF_HDF5_NODE_CONST_KEY_TYPE_METHODS_DEF);

std::vector<NodeConstHandle> NodeConstHandle::get_children() const {
  try {
    NodeIDs children = shared_->get_children(node_);
    std::vector<NodeConstHandle> ret(children.size());
    for (unsigned int i = 0; i < ret.size(); ++i) {
      ret[i] = NodeConstHandle(children[i], shared_);
    }
    return ret;
  }
  RMF_NODE_CATCH();
}

std::string get_type_name(NodeType t) {
  switch (t) {
    case ROOT:
      return "root";
    case REPRESENTATION:
      return "rep";
    case GEOMETRY:
      return "geom";
    case FEATURE:
      return "feat";
    case ALIAS:
      return "alias";
    case BOND:
      return "bond";
    case CUSTOM:
      return "custom";
    case ORGANIZATIONAL:
      return "organizational";
    default:
      return "unknown";
  }
}

std::ostream& operator<<(std::ostream& out, NodeType t) {
  out << get_type_name(t);
  return out;
}
std::istream& operator>>(std::istream& in, NodeType& t) {
  std::string token;
  in >> token;
  for (NodeType i = ROOT; i < LINK; i = NodeType(i + 1)) {
    if (token == get_type_name(i)) {
      t = i;
      return in;
    }
  }
  t = CUSTOM;
  return in;
}

namespace {
template <class Traits>
void show_data(NodeConstHandle n, std::ostream& out,
               const std::vector<ID<Traits> >& ks, std::string prefix) {
  using std::operator<<;
  RMF_FOREACH(ID<Traits> k, ks) {
    Nullable<Traits> t = n.get_frame_value(k);
    if (!t.get_is_null()) {
      out << std::endl << prefix << n.get_file().get_name(k) << ": "
          << Showable(t.get());
    } else {
      Nullable<Traits> ts = n.get_static_value(k);
      if (!ts.get_is_null()) {
        out << std::endl << prefix << n.get_file().get_name(k) << ": "
            << Showable(ts.get()) << "(s)";
      }
    }
  }
}

void show_node(NodeConstHandle n, std::ostream& out, std::string prefix = "") {
  using std::operator<<;
  out << prefix << "\"" << n.get_name() << "\" [" << get_type_name(n.get_type())
      << "]";
}
void show_node(NodeConstHandle n, std::ostream& out, FloatKeys fks,
               FloatsKeys fsks, IntKeys iks, IntsKeys isks, StringKeys sks,
               StringsKeys ssks, std::string prefix) {
  using std::operator<<;
  if (true) {
    show_node(n, out);
    show_data(n, out, fks, prefix + "  ");
    show_data(n, out, iks, prefix + "  ");
    show_data(n, out, sks, prefix + "  ");
    show_data(n, out, fsks, prefix + "  ");
    show_data(n, out, isks, prefix + "  ");
    show_data(n, out, ssks, prefix + "  ");
  }
}

void show_node_decorators(NodeConstHandle n, std::ostream& out,
                          ColoredConstFactory ccf, ParticleConstFactory pcf,
                          IntermediateParticleConstFactory ipcf,
                          RigidParticleConstFactory rpcf, ScoreConstFactory scf,
                          BallConstFactory bcf, CylinderConstFactory cycf,
                          SegmentConstFactory segcf, ResidueConstFactory rcf,
                          AtomConstFactory acf, ChainConstFactory chaincf,
                          DomainConstFactory fragcf, CopyConstFactory copycf,
                          DiffuserConstFactory diffusercf,
                          TypedConstFactory typedcf, std::string) {
  using std::operator<<;
  out << "\"" << n.get_name() << "\" [" << get_type_name(n.get_type()) << ": ";
  if (ccf.get_is(n)) out << " color";
  if (pcf.get_is(n)) out << " particle";
  if (ipcf.get_is(n)) out << " iparticle";
  if (rpcf.get_is(n)) out << " rigid";
  if (scf.get_is(n)) out << " score";
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
#define RMF_PRINT_TREE(stream, NodeType, start, num_children, get_children,  \
                       show)                                                 \
  {                                                                          \
    std::vector<boost::tuple<std::string, std::string, NodeType> > queue;    \
    queue.push_back(boost::make_tuple(std::string(), std::string(), start)); \
    do {                                                                     \
      boost::tuple<std::string, std::string, NodeType>& back = queue.back(); \
      NodeType n = back.get<2>();                                            \
      std::string prefix0 = back.get<0>();                                   \
      std::string prefix1 = back.get<1>();                                   \
      queue.pop_back();                                                      \
      stream << prefix0;                                                     \
      std::vector<NodeType> children = get_children;                         \
      if (children.size() > 0)                                               \
        stream << " + ";                                                     \
      else                                                                   \
        stream << " - ";                                                     \
      show;                                                                  \
      stream << std::endl;                                                   \
      for (int i = static_cast<int>(children.size()) - 1; i >= 0; --i) {     \
        queue.push_back(                                                     \
            boost::make_tuple(prefix1 + " ", prefix1 + " ", children[i]));   \
      }                                                                      \
    } while (!queue.empty());                                                \
  }
}

void show_hierarchy(NodeConstHandle root, std::ostream& out) {
  using std::operator<<;
  RMF_PRINT_TREE(out, NodeConstHandle, root, n.get_children().size(),
                 n.get_children(), show_node(n, out));
}

void show_hierarchy_with_values(NodeConstHandle root, std::ostream& out) {
  FloatKeys fks;
  IntKeys iks;
  StringKeys sks;
  FloatsKeys fsks;
  IntsKeys isks;
  StringsKeys ssks;
  fks = get_keys<FloatTraits>(root.get_file());
  iks = get_keys<IntTraits>(root.get_file());
  sks = get_keys<StringTraits>(root.get_file());
  fsks = get_keys<FloatsTraits>(root.get_file());
  isks = get_keys<IntsTraits>(root.get_file());
  ssks = get_keys<StringsTraits>(root.get_file());
  using std::operator<<;
  RMF_PRINT_TREE(
      out, NodeConstHandle, root, n.get_children().size(), n.get_children(),
      show_node(n, out, fks, fsks, iks, isks, sks, ssks, prefix0 + "   "));
}

void show_hierarchy_with_decorators(NodeConstHandle root, bool,
                                    std::ostream& out) {
  ColoredConstFactory ccf(root.get_file());
  ParticleConstFactory pcf(root.get_file());
  IntermediateParticleConstFactory ipcf(root.get_file());
  RigidParticleConstFactory rpcf(root.get_file());
  ScoreConstFactory scf(root.get_file());
  BallConstFactory bcf(root.get_file());
  CylinderConstFactory cycf(root.get_file());
  SegmentConstFactory segcf(root.get_file());
  ResidueConstFactory rcf(root.get_file());
  AtomConstFactory acf(root.get_file());
  ChainConstFactory chaincf(root.get_file());
  DomainConstFactory fragcf(root.get_file());
  CopyConstFactory copycf(root.get_file());
  DiffuserConstFactory diffusercf(root.get_file());
  TypedConstFactory typedcf(root.get_file());
  using std::operator<<;
  RMF_PRINT_TREE(
      out, NodeConstHandle, root, n.get_children().size(), n.get_children(),
      show_node_decorators(n, out, ccf, pcf, ipcf, rpcf, scf, bcf, cycf, segcf,
                           rcf, acf, chaincf, fragcf, copycf, diffusercf,
                           typedcf, prefix0 + "   "));
}

} /* namespace RMF */

RMF_DISABLE_WARNINGS
