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

template <class Traits, class SDA, class SDB>
boost::unordered_map<ID<Traits>, ID<Traits> > get_key_map(SDA* sda,
                                                            Category cata,
                                                            SDB* sdb,
                                                            Category catb) {
  boost::unordered_map<ID<Traits>, ID<Traits> > ret;
  std::vector<ID<Traits> > keysa = sda->get_keys(cata, Traits());
  BOOST_FOREACH(ID<Traits> keya, keysa) {
    ret[keya] = sdb->get_key(catb, sda->get_name(keya), Traits());
    RMF_TRACE(get_logger(), sda->get_name(keya) << " maps to "
                                                << sdb->get_name(ret[keya]))
  }
  return ret;
}

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_MAPS_H */
