/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_MAPS_H
#define RMF_INTERNAL_SHARED_DATA_MAPS_H

#include <RMF/config.h>
#include <RMF/log.h>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace internal {

template <class TraitsA, class TraitsB, class SDA, class SDB>
boost::unordered_map<ID<TraitsA>, ID<TraitsB> > get_key_map(SDA* sda,
                                                            Category cata,
                                                            SDB* sdb,
                                                            Category catb) {
  boost::unordered_map<ID<TraitsA>, ID<TraitsB> > ret;
  std::vector<ID<TraitsA> > keysa = sda->get_keys(cata, TraitsA());
  BOOST_FOREACH(ID<TraitsA> keya, keysa) {
    ret[keya] = sdb->get_key(catb, sda->get_name(keya), TraitsB());
    RMF_TRACE(get_logger(), sda->get_name(keya) << " maps to "
                                                << sdb->get_name(ret[keya]))
  }
  return ret;
}


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

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_MAPS_H */
