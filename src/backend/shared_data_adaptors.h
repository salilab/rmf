/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_ADAPTORS_H
#define RMF_INTERNAL_SHARED_DATA_ADAPTORS_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace backends {

class BackwardsAdaptor {
  internal::SharedData* sync_;
  IndexKey old_chain_id;
  StringKey new_chain_id;
 public:
  BackwardsAdaptor(internal::SharedData* sd) : sync_(sd) {
    Category sc = sync_->get_category("sequence");
    old_chain_id = sync_->get_key(sc, "chain id", IndexTraits());
    new_chain_id = sync_->get_key(sc, "chain id", StringTraits());
  }

  template <class Traits>
  void set_static_value(NodeID n, Key<Traits> k,
                        typename Traits::ArgumentType d) {
    sync_->set_static_value(n, k, d);
  }
  void set_static_value(NodeID n, Key<IndexTraits> k,
                        IndexTraits::ArgumentType d) {
    if (k == old_chain_id) {
      char buf[2];
      buf[1] = '\0';
      buf[0] = 'A' + d;
      sync_->set_static_value(n, new_chain_id, buf);
      RMF_TRACE(get_logger(), "Setting new chain to " << buf);
    } else {
      sync_->set_static_value(n, k, d);
    }
  }
  template <class Traits>
  void set_loaded_value(NodeID n, Key<Traits> k,
                        typename Traits::ArgumentType d) {
    sync_->set_loaded_value(n, k, d);
  }
  unsigned int get_number_of_nodes() const {
    return sync_->get_number_of_nodes();
  }
  template <class Traits>
  Key<Traits> get_key(Category c, std::string name, Traits) const {
    return sync_->get_key(c, name, Traits());
  }
  template <class Traits>
  std::string get_name(Key<Traits> k) const {
    return sync_->get_name(k);
  }
  std::string get_name(Category c) const {
    return sync_->get_name(c);
  }
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_ADAPTORS_H */
