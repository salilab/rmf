/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_HIERARCHY_H
#define RMF_INTERNAL_SHARED_DATA_HIERARCHY_H

#include <RMF/config.h>
#include <boost/unordered_map.hpp>
#include <string>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

class SharedDataHierarchy {
  struct HierarchyNode {
    std::string name;
    NodeType type;
    std::vector<NodeID> parents;
    std::vector<NodeID> children;
  };
  typedef boost::unordered_map<NodeID, HierarchyNode> Map;
  Map hierarchy_;
  bool dirty_;

 public:
  SharedDataHierarchy() { clear(); }

  unsigned int get_number_of_nodes() const { return hierarchy_.size(); }

  std::string get_name(NodeID node) const {
    return hierarchy_.find(node)->second.name;
  }
  NodeType get_type(NodeID node) const {
    return hierarchy_.find(node)->second.type;
  }
  void set_name(NodeID node, std::string name) { hierarchy_[node].name = name; }
  void set_type(NodeID node, NodeType t) { hierarchy_[node].type = t; }

  NodeID add_child(NodeID id, std::string name, NodeType t) {
    NodeID ret(hierarchy_.size());
    hierarchy_[ret].name = name;
    hierarchy_[ret].type = t;
    add_child(id, ret);
    dirty_ = true;
    return ret;
  }

  void add_child(NodeID parent, NodeID child) {
    RMF_USAGE_CHECK(
        parent != NodeID() && parent != NodeID(-1, NodeID::SpecialTag()),
        "Bad parent");
    hierarchy_[parent].children.push_back(child);
    hierarchy_[child].parents.push_back(child);
    dirty_ = true;
  }

  const std::vector<NodeID>& get_children(NodeID node) const {
    static std::vector<NodeID> missing;
    Map::const_iterator it = hierarchy_.find(node);
    if (it == hierarchy_.end()) return missing;
    return it->second.children;
  }

  const std::vector<NodeID>& get_parents(NodeID node) const {
    static std::vector<NodeID> missing;
    Map::const_iterator it = hierarchy_.find(node);
    if (it == hierarchy_.end()) return missing;
    return it->second.parents;
  }

  bool get_is_dirty() const { return dirty_; }
  void set_is_dirty(bool tf) { dirty_ = tf; }

  void clear() {
    hierarchy_.clear();
    dirty_ = true;
    hierarchy_[NodeID(0)].name = "root";
    hierarchy_[NodeID(0)].type = NodeType(0);
  }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_HIERARCHY_H */
