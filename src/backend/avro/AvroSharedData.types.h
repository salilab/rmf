/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_AVRO_SHARED_DATA_TYPES_H
#define RMF_INTERNAL_AVRO_SHARED_DATA_TYPES_H

#include <RMF/config.h>
#include <RMF/constants.h>
#include <backend/avro/AllJSON.h>
#include <boost/tuple/tuple.hpp>

RMF_ENABLE_WARNINGS
namespace RMF {
  namespace avro_backend {

#define RMF_AVRO_SHARED_TYPE(lcname, Ucname, PassValue, ReturnValue,    \
                             PassValues, ReturnValues)                  \
    private:                                                            \
    typedef std::vector<Ucname##Traits::AvroType> Ucname##Data;         \
    Ucname##Data empty_##lcname##_data_;                                \
    typedef boost::tuple<const Ucname##Data&, const KeyIndex&>          \
    Ucname##DataIndexConstPair;                                         \
    typedef boost::tuple<Ucname##Data&, KeyIndex&> Ucname##DataIndexPair; \
    const Ucname##DataIndexConstPair                                    \
    get_frame_type_data(Key<Ucname##Traits>, NodeID node,               \
                        Category category, FrameID frame) const {       \
      const RMF_avro_backend::Data& data = P::get_frame_data(category, frame); \
      typename std::map<std::string, Ucname##Data>::const_iterator it = \
        data.lcname##_data.nodes.find(P::get_node_string(node));        \
      if (it == data.lcname##_data.nodes.end()) {                       \
        return Ucname##DataIndexConstPair(empty_##lcname##_data_,       \
                                          data.lcname##_data.index);    \
      } else {                                                          \
        return Ucname##DataIndexConstPair(it->second, data.lcname##_data.index); \
      }                                                                 \
    }                                                                   \
    Ucname##DataIndexPair access_frame_type_data(Key<Ucname##Traits>,\
                                                 NodeID node, Category category,\
                                                 FrameID frame) {       \
      RMF_avro_backend::Data& data = P::access_frame_data(category, frame); \
      std::string ns = P::get_node_string(node);                        \
      return Ucname##DataIndexPair(data.lcname##_data.nodes[ns],        \
                                   data.lcname##_data.index);           \
    }                                                                   \
  public:                                                               \
  virtual Ucname##Traits::Type get_current_value(NodeID node,           \
                                                 Key<Ucname##Traits> k)\
    const RMF_OVERRIDE  {                                               \
    return get_value_impl(P::get_current_frame(), node, k);             \
  }                                                                     \
  virtual Ucname##Traits::Type get_static_value(NodeID node,            \
                                                Key<Ucname##Traits> k)\
    const RMF_OVERRIDE  {                                               \
    return get_value_impl(ALL_FRAMES, node, k);                         \
  }                                                                     \
  virtual Ucname##Traits::Type get_current_frame_value(                 \
                                                       Key<Ucname##Traits> k)\
    const RMF_OVERRIDE  {                                               \
    return get_value_impl(P::get_current_frame(), NodeID(), k);         \
  }                                                                     \
  virtual Ucname##Traits::Type get_static_frame_value(                  \
                                                      Key<Ucname##Traits> k)\
    const RMF_OVERRIDE  {                                               \
    return get_value_impl(ALL_FRAMES, NodeID(), k);                     \
  }                                                                     \
  virtual void set_current_value(NodeID node, Key<Ucname##Traits> k,\
                                 Ucname##Traits::Type v) RMF_OVERRIDE  { \
    set_value_impl(P::get_current_frame(), node, k, v);                 \
  }                                                                     \
  virtual void set_static_value(NodeID node, Key<Ucname##Traits> k,\
                                Ucname##Traits::Type v) RMF_OVERRIDE  { \
    set_value_impl(ALL_FRAMES, node, k, v);                             \
  }                                                                     \
  virtual void set_current_frame_value(Key<Ucname##Traits> k, \
                                       Ucname##Traits::Type v) RMF_OVERRIDE  { \
    set_value_impl(P::get_current_frame(), NodeID(), k, v);             \
  }                                                                     \
  virtual void set_static_frame_value(Key<Ucname##Traits> k,\
                                      Ucname##Traits::Type v) RMF_OVERRIDE  { \
    set_value_impl(ALL_FRAMES, NodeID(), k, v);                         \
  }                                                                     \
  virtual std::vector<Key<Ucname##Traits> > get_##lcname##_keys(Category cat)\
    RMF_OVERRIDE  {                                                     \
    internal::set<Key<Ucname##Traits> > ret;                            \
    const RMF_avro_backend::Data& data =                                \
      P::get_frame_data(cat, P::get_current_frame());                   \
    extract_keys(cat, data.lcname##_data.index, ret);                   \
    const RMF_avro_backend::Data& staticdata =                          \
      P::get_frame_data(cat, ALL_FRAMES);                               \
    extract_keys(cat, staticdata.lcname##_data.index, ret);             \
    return std::vector<Key<Ucname##Traits> >(ret.begin(), ret.end());   \
  }                                                                     \
  virtual  Key<Ucname##Traits> get_##lcname##_key(Category category,    \
                                                  std::string name)\
    RMF_OVERRIDE  {                                                     \
    return P::template get_key_helper<Ucname##Traits>(category, name);  \
  }                                                                     \
  virtual std::string get_name(Key<Ucname##Traits> k) const RMF_OVERRIDE  { \
    return P::get_key_name(k.get_id());                                 \
  }                                                                     \
  virtual Category get_category(Key<Ucname##Traits> k) const RMF_OVERRIDE { \
    return P::get_category(k.get_id());                                 \
  }

  }  // namespace avro_backend
}    /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_AVRO_SHARED_DATA_TYPES_H */
