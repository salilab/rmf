/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_KEY_FILTER_H
#define RMF_INTERNAL_KEY_FILTER_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include "SharedDataAdaptor.h"
#include <RMF/log.h>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace backends {

template <class SD>
class KeyFilter: public SharedDataAdaptor<SD> {
  typedef SharedDataAdaptor<SD> P;
  IndexKeys index_keys_;
 public:
  KeyFilter(SD* sd) : P(sd) {}
  void add_index_key(Category cat, std::string name) {
    BOOST_FOREACH(IndexKey k, P::sync_->get_keys(cat, IndexTraits())) {
      if (P::sync_->get_name(k) == name) {
        index_keys_.push_back(k);
        std::sort(index_keys_.begin(), index_keys_.end());
        break;
      }
    }
  }
  template <class Traits>
  std::vector<Key<Traits> > get_keys(Category cat, Traits) {
    return P::sync_->get_keys(cat, Traits());
  }
  IndexKeys get_keys(Category cat, IndexTraits) {
    IndexKeys all = P::sync_->get_keys(cat, IndexTraits());
    std::sort(all.begin(), all.end());
    IndexKeys ret;
    std::set_difference(all.begin(), all.end(), index_keys_.begin(),
                        index_keys_.end(), std::back_inserter(ret));
    return ret;
  }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_KEY_FILTER_H */
