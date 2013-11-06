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
#include <boost/foreach.hpp>
#include <boost/range/distance.hpp>
#include <boost/unordered_map.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

struct StaticValues {
  template <class Traits, class SD>
  static typename Traits::Type get(SD* sd, NodeID n, ID<Traits> k) {
    return sd->get_static_value(n, k);
  }
  template <class Traits, class SD>
  static void set(SD* sd, NodeID n, ID<Traits> k, typename Traits::Type v) {
    sd->set_static_value(n, k, v);
  }
};
struct LoadedValues {
  template <class Traits, class SD>
  static typename Traits::Type get(SD* sd, NodeID n, ID<Traits> k) {
    return sd->get_loaded_value(n, k);
  }
  template <class Traits, class SD>
  static void set(SD* sd, NodeID n, ID<Traits> k, typename Traits::Type v) {
    sd->set_loaded_value(n, k, v);
  }
};

template <class SDA, class SDB>
void clone_hierarchy(SDA* sda, SDB* sdb) {
  RMF_INTERNAL_CHECK(boost::distance(get_nodes(sda)) >= 1,
                     "No root node found.");
  RMF_INTERNAL_CHECK(boost::distance(get_nodes(sdb)) >= 1,
                     "No root node found.");
  boost::unordered_map<NodeID, NodeID> parents;
  BOOST_FOREACH(NodeID na, get_nodes(sda)) {
    NodeIDs children = sda->get_children(na);
    BOOST_FOREACH(NodeID c, children) {
      if (parents.find(c) == parents.end() &&
          c.get_index() >= sdb->get_number_of_nodes()) {
        parents[c] = na;
      }
    }
  }
  if (parents.empty()) return;
  BOOST_FOREACH(NodeID na, get_nodes(sda)) {
    if (parents.find(na) != parents.end()) {
      NodeID nid = sdb->add_child(parents.find(na)->second, sda->get_name(na),
                                  sda->get_type(na));
      RMF_UNUSED(nid);
      RMF_INTERNAL_CHECK(nid == na, "Don't match");
    }
  }
  BOOST_FOREACH(NodeID na, get_nodes(sda)) {
    NodeIDs children = sda->get_children(na);
    BOOST_FOREACH(NodeID c, children) {
      if (parents.find(c) != parents.end() && parents.find(c)->second != na) {
        sdb->add_child(na, c);
      }
    }
  }
}

template <class SDA, class SDB>
void clone_file(SDA* sda, SDB* sdb) {
  sdb->set_description(sda->get_description());
  sdb->set_producer(sda->get_producer());
}

template <class Traits, class SDA, class SDB, class H>
void clone_values_type(SDA* sda, Category cata, SDB* sdb, Category catb,
                       H) {
    boost::unordered_map<ID<Traits>, ID<Traits> > keys =
        get_key_map<Traits>(sda, cata, sdb, catb);
  if (keys.empty()) return;
  BOOST_FOREACH(NodeID n, get_nodes(sda)) {
    RMF_TRACE(get_logger(), "Cloning static node " << n);
    typedef std::pair<ID<Traits>, ID<Traits> > KP;
    BOOST_FOREACH(KP ks, keys) {
      typename Traits::ReturnType rt = H::get(sda, n, ks.first);
      if (!Traits::get_is_null_value(rt)) {
        H::set(sdb, n, ks.second, rt);
      }
    }
  }
}

template <class SDA, class SDB, class H>
void clone_values_category(SDA* sda, Category cata, SDB* sdb, Category catb,
                           H) {
  RMF_INTERNAL_CHECK(sda->get_number_of_nodes() <= sdb->get_number_of_nodes(),
                     "Number of nodes don't match.");
  RMF_INTERNAL_CHECK(boost::distance(get_nodes(sda)) >= 1,
                     "No root node found.");
  RMF_INTERNAL_CHECK(boost::distance(get_nodes(sdb)) >= 1,
                     "No root node found.");
  clone_values_type<IntTraits>(sda, cata, sdb, catb, H());
  clone_values_type<FloatTraits>(sda, cata, sdb, catb, H());
  clone_values_type<StringTraits>(sda, cata, sdb, catb, H());
  clone_values_type<IndexTraits>(sda, cata, sdb, catb, H());
  clone_values_type<NodeIDTraits>(sda, cata, sdb, catb, H());
  clone_values_type<IntsTraits>(sda, cata, sdb, catb, H());
  clone_values_type<FloatsTraits>(sda, cata, sdb, catb, H());
  clone_values_type<StringsTraits>(sda, cata, sdb, catb, H());
  clone_values_type<IndexesTraits>(sda, cata, sdb, catb, H());
  clone_values_type<NodeIDsTraits>(sda, cata, sdb, catb, H());
}

template <class SDA, class SDB>
void clone_static_data(SDA* sda, SDB* sdb) {
  BOOST_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    clone_values_category(sda, cata, sdb, catb, StaticValues());
  }
}

template <class SDA, class SDB>
void clone_loaded_data(SDA* sda, SDB* sdb) {
  BOOST_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    clone_values_category(sda, cata, sdb, catb, LoadedValues());
  }
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_CLONE_SHARED_DATA_H */
