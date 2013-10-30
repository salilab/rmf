/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_EQUALITY_H
#define RMF_INTERNAL_SHARED_DATA_EQUALITY_H

#include <RMF/config.h>
#include <RMF/log.h>
#include "shared_data_maps.h"
#include <RMF/internal/shared_data_ranges.h>
#include <boost/foreach.hpp>
#include <boost/range/distance.hpp>
#include <boost/unordered_set.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

template <class Traits, class SDA, class SDB>
bool get_equal_current_values_type(SDA* sda, Category cata, SDB* sdb,
                                   Category catb) {
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
bool get_equal_current_values_category(SDA* sda, Category cata, SDB* sdb,
                                       Category catb) {
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
bool get_equal_current_values(SDA* sda, SDB* sdb) {
  BOOST_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    if (!get_equal_current_values_category(sda, cata, sdb, catb)) return false;
  }
  return true;
}

template <class Traits, class SDA, class SDB>
bool get_equal_static_values_type(SDA* sda, Category cata, SDB* sdb,
                                  Category catb) {
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
bool get_equal_static_values_category(SDA* sda, Category cata, SDB* sdb,
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
bool get_equal_static_values(SDA* sda, SDB* sdb) {
  BOOST_FOREACH(Category cata, sda->get_categories()) {
    Category catb = sdb->get_category(sda->get_name(cata));
    if (!get_equal_static_values_category(sda, cata, sdb, catb)) return false;
  }
  return true;
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_EQUALITY_H */