/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_AVRO_SHARED_DATA_H
#define RMF_INTERNAL_AVRO_SHARED_DATA_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include <RMF/infrastructure_macros.h>
#include <RMF/constants.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "AvroSharedData.types.h"
#include <utility>

RMF_ENABLE_WARNINGS namespace RMF {
  namespace avro_backend {

  template <class Base> class AvroSharedData : public Base {
    typedef Base P;

    typedef std::map<std::string, int32_t> KeyIndex;

    template <class TypeTraits>
    void extract_keys(Category cat,
                      const KeyIndex& index,
                      boost::unordered_set<Key<TypeTraits> >& ret) {
      for (typename KeyIndex::const_iterator iti = index.begin();
           iti != index.end();
           ++iti) {
        ret.insert(P::template get_key_helper<TypeTraits>(cat, iti->first));
      }
    }

    template <class TypeTraits>
    typename TypeTraits::Type get_one_value(
        const std::vector<typename TypeTraits::AvroType>& data,
        const std::map<std::string, int>& index,
        Key<TypeTraits> k) const {
      std::string keyname = P::get_key_name(k.get_id());
      typename std::map<std::string, int>::const_iterator it =
          index.find(keyname);
      if (it == index.end() || it->second >= static_cast<int>(data.size())) {
        return TypeTraits::get_null_value();
      } else {
        typename TypeTraits::Type ret =
            get_as<typename TypeTraits::Type>(data[it->second]);
        return ret;
      }
    }

    template <class TypeTraits>
    void set_one_value(std::vector<typename TypeTraits::AvroType>& data,
                       std::map<std::string, int>& index,
                       Key<TypeTraits> k,
                       const typename TypeTraits::Type& val) {
      std::string keyname = P::get_key_name(k.get_id());
      typename std::map<std::string, int>::const_iterator it =
          index.find(keyname);
      int index_value;
      if (it == index.end()) {
        index_value = index.size();
        index[keyname] = index_value;
      } else {
        index_value = it->second;
      }
      if (static_cast<int>(data.size()) <= index_value) {
        typename TypeTraits::AvroType null =
            get_as<typename TypeTraits::AvroType>(TypeTraits::get_null_value());
        data.resize(index_value + 1, null);
      }
      data[index_value] = get_as<typename TypeTraits::AvroType>(val);
    }
    template <class TypeTraits>
    typename TypeTraits::Type get_value_impl(FrameID frame,
                                             NodeID node,
                                             Key<TypeTraits> k) const {
      typedef std::vector<typename TypeTraits::AvroType> Data;
      typedef boost::tuple<const Data&, const KeyIndex&> Pair;

      Category cat = get_category(k);
      {
        Pair data = get_frame_type_data(k, node, cat, frame);
        typename TypeTraits::Type ret =
            get_one_value(data.template get<0>(), data.template get<1>(), k);
          return ret;
      }
    }
    template <class TypeTraits>
    void set_value_impl(FrameID frame,
                        NodeID node,
                        Key<TypeTraits> k,
                        typename TypeTraits::Type v) {
      typedef std::vector<typename TypeTraits::AvroType> Data;
      typedef boost::tuple<Data&, KeyIndex&> Pair;

      Category cat = get_category(k);
      Pair data = access_frame_type_data(k, node, cat, frame);
      set_one_value(data.template get<0>(), data.template get<1>(), k, v);
    }

   public:
    using P::get_category;
    RMF_FOREACH_TYPE(RMF_AVRO_SHARED_TYPE);

    AvroSharedData(std::string g, bool create, bool read_only);
    // buffer versions
    AvroSharedData(std::string& buffer, bool create);
    AvroSharedData(const std::string& buffer);

    virtual ~AvroSharedData() {}
    virtual std::string get_name(NodeID node) const RMF_OVERRIDE;
    NodeType get_type(NodeID node) const RMF_OVERRIDE;
    NodeID add_child(NodeID node, std::string name, NodeType t) RMF_OVERRIDE;
    void add_child(NodeID node, NodeID child_node) RMF_OVERRIDE;
    NodeIDs get_children(NodeID node) const RMF_OVERRIDE;
    void save_frames_hint(int) {}
    std::string get_description() const;
    void set_description(std::string str);
    std::string get_producer() const;
    void set_producer(std::string str);
    using Base::get_name;
    using Base::get_type;
    using Base::add_child;
    using Base::get_children;
  };

  }  // namespace avro_backend
}    /* namespace RMF */

RMF_DISABLE_WARNINGS

#include "AvroSharedData.impl.h"

#endif /* RMF_INTERNAL_AVRO_SHARED_DATA_H */
