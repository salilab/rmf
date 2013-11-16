/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_CLONE_SHARED_DATA_H
#define RMF_INTERNAL_CLONE_SHARED_DATA_H

#include <RMF/config.h>
#include <RMF/log.h>
#include <RMF/internal/shared_data_ranges.h>
#include "shared_data_maps.h"
#include "shared_data_equality.h"

#include <boost/range/distance.hpp>
#include <boost/unordered_map.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

template <class SDA, class SDB>
void clone_hierarchy(SDA* sda, SDB* sdb) {
  RMF_INTERNAL_CHECK(boost::distance(get_nodes(sda)) >= 1,
                     "No root node found.");
  RMF_INTERNAL_CHECK(boost::distance(get_nodes(sdb)) >= 1,
                     "No root node found.");
  boost::unordered_map<NodeID, NodeID> parents;
  RMF_FOREACH(NodeID na, get_nodes(sda)) {
    NodeIDs children = sda->get_children(na);
    RMF_FOREACH(NodeID c, children) {
      if (parents.find(c) == parents.end() &&
          c.get_index() >= sdb->get_number_of_nodes()) {
        parents[c] = na;
      }
    }
  }
  if (parents.empty()) return;
  RMF_FOREACH(NodeID na, get_nodes(sda)) {
    if (parents.find(na) != parents.end()) {
      NodeID nid = sdb->add_child(parents.find(na)->second, sda->get_name(na),
                                  sda->get_type(na));
      RMF_UNUSED(nid);
      RMF_INTERNAL_CHECK(nid == na, "Don't match");
    }
  }
  RMF_FOREACH(NodeID na, get_nodes(sda)) {
    NodeIDs children = sda->get_children(na);
    RMF_FOREACH(NodeID c, children) {
      if (parents.find(c) != parents.end() && parents.find(c)->second != na) {
        sdb->add_child(na, c);
      }
    }
  }
}

inline void clone_hierarchy(const SharedData* sda, SharedData* sdb) {
  sdb->access_node_hierarchy() = sda->get_node_hierarchy();
}

template <class SDA, class SDB>
void clone_file(SDA* sda, SDB* sdb) {
  sdb->set_description(sda->get_description());
  sdb->set_producer(sda->get_producer());
}

#define RMF_CLONE_KEYS(lcname, UCName, PassValue, ReturnValue, PassValues, \
                       ReturnValues)                                       \
  sdb->access_key_data(UCName##Traits()) = sda->get_key_data(UCName##Traits());

inline void clone_file(const SharedData* sda, SharedData* sdb) {
  sdb->set_description(sda->get_description());
  sdb->set_producer(sda->get_producer());
  sdb->access_category_data() = sda->get_category_data();
  RMF_FOREACH_TYPE(RMF_CLONE_KEYS);
}

template <class TraitsA, class TraitsB, class SDA, class SDB, class H>
void clone_values_type(SDA* sda, Category cata, SDB* sdb, Category catb, H) {
  boost::unordered_map<ID<TraitsA>, ID<TraitsB> > keys =
      get_key_map<TraitsA, TraitsB>(sda, cata, sdb, catb);
  if (keys.empty()) return;
  typedef std::pair<ID<TraitsA>, ID<TraitsB> > KP;
  RMF_FOREACH(KP ks, keys) {
    RMF_FOREACH(NodeID n, get_nodes(sda)) {
      typename TraitsA::ReturnType rt = H::get(sda, n, ks.first);
      if (!TraitsA::get_is_null_value(rt)) {
        H::set(sdb, n, ks.second, get_as<typename TraitsB::Type>(rt));
      }
    }
  }
}

#define RMF_CLONE_VALUES(lcname, UCName, PassValue, ReturnValue, PassValues, \
                         ReturnValues)                                       \
  clone_values_type<UCName##Traits, UCName##Traits>(sda, cata, sdb, catb, H());

template <class SDA, class SDB, class H>
void clone_values_category(SDA* sda, Category cata, SDB* sdb, Category catb,
                           H) {
  RMF_INTERNAL_CHECK(sda->get_number_of_nodes() <= sdb->get_number_of_nodes(),
                     "Number of nodes don't match.");
  RMF_INTERNAL_CHECK(boost::distance(get_nodes(sda)) >= 1,
                     "No root node found.");
  RMF_INTERNAL_CHECK(boost::distance(get_nodes(sdb)) >= 1,
                     "No root node found.");
  RMF_FOREACH_TYPE(RMF_CLONE_VALUES);
}

template <class SDA, class SDB>
void clone_static_data(SDA* sda, SDB* sdb) {
  RMF_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    clone_values_category(sda, cata, sdb, catb, StaticValues());
  }
}

template <class SDA, class SDB>
void clone_loaded_data(SDA* sda, SDB* sdb) {
  RMF_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    clone_values_category(sda, cata, sdb, catb, LoadedValues());
  }
}

#define RMF_CLONE_DATA(lcname, UCName, PassValue, ReturnValue, PassValues, \
                       ReturnValues)                                       \
  H::access_data(sdb, UCName##Traits()) = H::get_data(sda, UCName##Traits());

inline void clone_static_data(const SharedData* sda, SharedData* sdb) {
  typedef StaticValues H;
  RMF_FOREACH_TYPE(RMF_CLONE_DATA);
  sdb->set_static_is_dirty(true);
}

inline void clone_loaded_data(const SharedData* sda, SharedData* sdb) {
  typedef LoadedValues H;
  RMF_FOREACH_TYPE(RMF_CLONE_DATA);
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_CLONE_SHARED_DATA_H */
