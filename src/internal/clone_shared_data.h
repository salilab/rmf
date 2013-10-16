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
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

template <class SDA, class SDB>
void clone_hierarchy(SDA* sda, SDB* sdb) {
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

template <class Traits, class SDA, class SDB>
boost::unordered_map<Key<Traits>, Key<Traits> > get_key_map(
    SDA* sda, Category cata, SDB* sdb,
    Category catb) {
  boost::unordered_map<Key<Traits>, Key<Traits> > ret;
  std::vector<Key<Traits> > keysa = sda->get_keys(cata, Traits());
  BOOST_FOREACH(Key<Traits> keya, keysa) {
    ret[keya] = sdb->get_key(catb, sda->get_name(keya), Traits());
    RMF_INFO(get_logger(), sda->get_name(keya) << " maps to "
                                               << sdb->get_name(ret[keya]))
  }
  return ret;
}

template <class Traits, class SDA, class SDB>
void clone_static_data_type(SDA* sda, Category cata,
                            SDB* sdb, Category catb) {
  boost::unordered_map<Key<Traits>, Key<Traits> > keys =
    get_key_map<Traits>(sda, cata, sdb, catb);
  if (keys.empty()) return;
  BOOST_FOREACH(NodeID n, get_nodes(sda)) {
    RMF_INFO(get_logger(), "Cloning node " << n);
    typedef std::pair<Key<Traits>, Key<Traits> > KP;
    BOOST_FOREACH(KP ks, keys) {
      typename Traits::ReturnType rt = sda->get_static_value(n, ks.first);
      if (!Traits::get_is_null_value(rt)) {
        sdb->set_static_value(n, ks.second, rt);
      }
    }
  }
}

template <class SDA, class SDB>
void clone_static_data_category(SDA* sda, Category cata,
                                SDB* sdb, Category catb) {
  RMF_INTERNAL_CHECK(sda->get_number_of_nodes() == sdb->get_number_of_nodes(),
                     "Number of nodes don't match.");
  clone_static_data_type<IntTraits>(sda, cata, sdb, catb);
  clone_static_data_type<FloatTraits>(sda, cata, sdb, catb);
  clone_static_data_type<StringTraits>(sda, cata, sdb, catb);
  clone_static_data_type<IndexTraits>(sda, cata, sdb, catb);
  clone_static_data_type<NodeIDTraits>(sda, cata, sdb, catb);
  clone_static_data_type<IntsTraits>(sda, cata, sdb, catb);
  clone_static_data_type<FloatsTraits>(sda, cata, sdb, catb);
  clone_static_data_type<StringsTraits>(sda, cata, sdb, catb);
  clone_static_data_type<IndexesTraits>(sda, cata, sdb, catb);
  clone_static_data_type<NodeIDsTraits>(sda, cata, sdb, catb);
}

template <class SDA, class SDB>
void clone_static_data(SDA* sda, SDB* sdb) {
  BOOST_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    clone_static_data_category(sda, cata, sdb, catb);
  }
}

template <class Traits, class SDA, class SDB>
void clone_loaded_data_type(SDA* sda, Category cata,
                            SDB* sdb, Category catb) {
  boost::unordered_map<Key<Traits>, Key<Traits> > keys =
    get_key_map<Traits>(sda, cata, sdb, catb);
  BOOST_FOREACH(NodeID n, get_nodes(sda)) {
    typedef std::pair<Key<Traits>, Key<Traits> > KP;
    BOOST_FOREACH(KP ks, keys) {
      typename Traits::ReturnType rt = sda->get_loaded_value(n, ks.first);
      if (!Traits::get_is_null_value(rt)) {
        sdb->set_loaded_value(n, ks.second, rt);
      }
    }
  }
}

template <class SDA, class SDB>
void clone_loaded_data_category(SDA* sda, Category cata,
                                SDB* sdb, Category catb) {
  RMF_INTERNAL_CHECK(sda->get_number_of_nodes() == sdb->get_number_of_nodes(),
                     "Number of nodes don't match.");
  clone_loaded_data_type<IntTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<FloatTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<StringTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<IndexTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<NodeIDTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<IntsTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<FloatsTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<StringsTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<IndexesTraits>(sda, cata, sdb, catb);
  clone_loaded_data_type<NodeIDsTraits>(sda, cata, sdb, catb);
}

template <class SDA, class SDB>
void clone_loaded_data(SDA* sda, SDB* sdb) {
  BOOST_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    clone_loaded_data_category(sda, cata, sdb, catb);
  }
}

template <class Traits, class SDA, class SDB>
bool get_equal_current_values_type(SDA* sda, Category cata,
                                   SDB* sdb, Category catb) {
  boost::unordered_map<Key<Traits>, Key<Traits> > keys =
      get_key_map<Traits>(sda, cata, sdb, catb);
  bool ret = true;
  BOOST_FOREACH(NodeID n, get_nodes(sda)) {
    typedef std::pair<Key<Traits>, Key<Traits> > KP;
    BOOST_FOREACH(KP ks, keys) {
      typename Traits::ReturnType rta = sda->get_loaded_value(n, ks.first);
      typename Traits::ReturnType rtb = sdb->get_loaded_value(n, ks.second);
      bool ha = !Traits::get_is_null_value(rta);
      bool hb = !Traits::get_is_null_value(rtb);
      if (ha != hb) {
        std::cout << "Nodes " << sda->get_name(n) << " and " << sdb->get_name(n)
                  << " differ in having " << sda->get_name(ks.first)
                  << " bits are " << ha << " and " << hb << std::endl;
        ret = false;
      }
      if (ha && hb && !Traits::get_are_equal(rta, rtb)) {
        std::cout << "Nodes " << sda->get_name(n) << " and " << sdb->get_name(n)
                  << " differ in values " << sda->get_name(ks.first)
                  << " values are " << rta << " and " << rtb << std::endl;
        ret = false;
      }
    }
  }
  return ret;
}

template <class SDA, class SDB>
bool get_equal_current_values_category(SDA* sda, Category cata,
                                SDB* sdb, Category catb) {
  bool ret = true;
  ret &= get_equal_current_values_type<IntTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<FloatTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<StringTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<IndexTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<NodeIDTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<IntsTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<FloatsTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<StringsTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<IndexesTraits>(sda, cata, sdb, catb);
  ret &= get_equal_current_values_type<NodeIDsTraits>(sda, cata, sdb, catb);
  return ret;
}

template <class SDA, class SDB>
bool get_equal_current_values(SDA* sda,
                              SDB* sdb) {
  BOOST_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    if (!get_equal_current_values_category(sda, cata, sdb, catb)) return false;
  }
  return true;
}

template <class Traits, class SDA, class SDB>
bool get_equal_static_values_type(SDA* sda, Category cata,
                                  SDB* sdb, Category catb) {
  boost::unordered_map<Key<Traits>, Key<Traits> > keys =
      get_key_map<Traits>(sda, cata, sdb, catb);
  bool ret = true;
  BOOST_FOREACH(NodeID n, get_nodes(sda)) {
    typedef std::pair<Key<Traits>, Key<Traits> > KP;
    BOOST_FOREACH(KP ks, keys) {
      typename Traits::ReturnType rta = sda->get_static_value(n, ks.first);
      typename Traits::ReturnType rtb = sdb->get_static_value(n, ks.second);
      bool ha = !Traits::get_is_null_value(rta);
      bool hb = !Traits::get_is_null_value(rtb);
      if (ha != hb) {
        std::cout << "Nodes " << sda->get_name(n) << " and " << sdb->get_name(n)
                  << " differ in having " << sda->get_name(ks.first)
                  << " bits are " << ha << " and " << hb << std::endl;
        ret = false;
      }
      if (ha && hb && !Traits::get_are_equal(rta, rtb)) {
        std::cout << "Nodes " << sda->get_name(n) << " and " << sdb->get_name(n)
                  << " differ in values " << sda->get_name(ks.first)
                  << " values are " << rta << " and " << rtb << std::endl;
        ret = false;
      }
    }
  }
  return ret;
}

template <class SDA, class SDB>
bool get_equal_static_values_category(SDA* sda, Category cata,
                                      SDB* sdb,
                                      Category catb) {
  bool ret = true;
  ret &= get_equal_static_values_type<IntTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<FloatTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<StringTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<IndexTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<NodeIDTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<IntsTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<FloatsTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<StringsTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<IndexesTraits>(sda, cata, sdb, catb);
  ret &= get_equal_static_values_type<NodeIDsTraits>(sda, cata, sdb, catb);
  return ret;
}

template <class SDA, class SDB>
bool get_equal_static_values(SDA* sda,
                             SDB* sdb) {
  BOOST_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    if (!get_equal_static_values_category(sda, cata, sdb, catb)) return false;
  }
  return true;
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_CLONE_SHARED_DATA_H */
