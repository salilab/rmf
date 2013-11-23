
#ifndef RMF_AVRO2_TYPE_ENCODE_DECODE_H
#define RMF_AVRO2_TYPE_ENCODE_DECODE_H

#include <RMF/constants.h>
#include <RMF/log.h>
#include "encode_decode.h"
#include "avrocpp/api/Specific.hh"
#include "avrocpp/api/Encoder.hh"
#include "avrocpp/api/Decoder.hh"

namespace RMF {
namespace avro2 {
template <class Traits>
inline void sort_key(const KeyInfo& ki, KeyMaps<Traits>& maps) {
  ID<Traits> k(ki.id);
  maps.category[k] = Category(ki.category);
  maps.name[k] = ki.name;
}

inline void sort_keys(const std::vector<KeyInfo>& in, KeyData& out) {
  RMF_FOREACH(const KeyInfo & k, in) {
    switch (k.type) {
      case INT:
        sort_key(k, out.int_keys);
        break;
      case FLOAT:
        sort_key(k, out.float_keys);
        break;
      case STRING:
        sort_key(k, out.string_keys);
        break;
      case INTS:
        sort_key(k, out.ints_keys);
        break;
      case FLOATS:
        sort_key(k, out.floats_keys);
        break;
      case STRINGS:
        sort_key(k, out.strings_keys);
        break;
      case VECTOR3:
        sort_key(k, out.vector3_keys);
        break;
      case VECTOR4:
        sort_key(k, out.vector4_keys);
        break;
      case VECTOR3S:
        sort_key(k, out.vector3s_keys);
        break;
      default:
        RMF_THROW(Message("No such type"), IOException);
    }
  }
}
}
}

namespace internal_avro {

template <>
struct codec_traits<RMF::avro2::StringAccumulator> {
  static void encode(Encoder& e, const RMF::avro2::StringAccumulator& v) {
    internal_avro::encode<std::string>(e, v);
  }
  static void decode(Decoder& d, RMF::avro2::StringAccumulator& v) {
    std::string cur;
    internal_avro::decode(d, cur);
    if (!cur.empty()) v.set(cur);
  }
};

template <class T, class Ty>
struct codec_traits<RMF::internal::HierarchyNode<T, Ty> > {
  static void encode(Encoder& e, const RMF::internal::HierarchyNode<T, Ty>& v) {
    RMF_TRACE(RMF::get_logger(), "Encoding node " << v.name << " " << v.type);
    internal_avro::encode(e, v.name);
    internal_avro::encode(e, v.type);
    internal_avro::encode(e, v.parents);
    internal_avro::encode(e, v.children);
  }
  static void decode(Decoder& d, RMF::internal::HierarchyNode<T, Ty>& v) {
    internal_avro::decode(d, v.name);
    internal_avro::decode(d, v.type);
    RMF_TRACE(RMF::get_logger(), "Found node " << v.name << " " << v.type);
    internal_avro::decode(d, v.parents);
    internal_avro::decode(d, v.children);
  }
};

template <>
struct codec_traits<RMF::avro2::HierarchyNode> {
  static void encode(Encoder& e, const RMF::avro2::HierarchyNode& v) {
    RMF_TRACE(RMF::get_logger(), "Encoding node " << v.id << " " << v.name
                                                  << " " << v.type);
    internal_avro::encode(e, v.id);
    internal_avro::encode(e, v.name);
    internal_avro::encode(e, v.type);
    internal_avro::encode(e, v.parents);
  }
  static void decode(Decoder& d, RMF::avro2::HierarchyNode& v) {
    internal_avro::decode(d, v.id);
    internal_avro::decode(d, v.name);
    internal_avro::decode(d, v.type);
    RMF_TRACE(RMF::get_logger(), "Found node " << v.id << " " << v.name << " "
                                               << v.type);
    internal_avro::decode(d, v.parents);
  }
};

template <>
struct codec_traits<RMF::avro2::KeyType> {
  static void encode(Encoder& e, RMF::avro2::KeyType v) { e.encodeEnum(v); }
  static void decode(Decoder& d, RMF::avro2::KeyType& v) {
    v = static_cast<RMF::avro2::KeyType>(d.decodeEnum());
  }
};

template <>
struct codec_traits<RMF::avro2::KeyInfo> {
  static void encode(Encoder& e, const RMF::avro2::KeyInfo& v) {
    internal_avro::encode(e, v.id);
    internal_avro::encode(e, v.name);
    internal_avro::encode(e, v.category);
    internal_avro::encode(e, v.type);
  }
  static void decode(Decoder& d, RMF::avro2::KeyInfo& v) {
    internal_avro::decode(d, v.id);
    internal_avro::decode(d, v.name);
    internal_avro::decode(d, v.category);
    internal_avro::decode(d, v.type);
  }
};

template <class Traits>
struct codec_traits<RMF::internal::KeyData<Traits> > {
  static void encode(Encoder& e, const RMF::internal::KeyData<Traits>& v) {
    internal_avro::encode<typename RMF::internal::KeyData<Traits>::P>(e, v);
  }
  static void decode(Decoder& d, RMF::internal::KeyData<Traits>& v) {
    internal_avro::decode<typename RMF::internal::KeyData<Traits>::P>(d, v);
  }
};

template <class Traits>
struct codec_traits<RMF::internal::TypeData<Traits> > {
  static void encode(Encoder& e, const RMF::internal::TypeData<Traits>& v) {
    internal_avro::encode<typename RMF::internal::TypeData<Traits>::P>(e, v);
  }
  static void decode(Decoder& d, RMF::internal::TypeData<Traits>& v) {
    internal_avro::decode<typename RMF::internal::TypeData<Traits>::P>(d, v);
  }
};

template <>
struct codec_traits<RMF::avro2::DataTypes> {
  static void encode(Encoder& e, const RMF::avro2::DataTypes& v) {
    internal_avro::encode(e, v.int_data);
    internal_avro::encode(e, v.float_data);
    internal_avro::encode(e, v.string_data);
    internal_avro::encode(e, v.ints_data);
    internal_avro::encode(e, v.floats_data);
    internal_avro::encode(e, v.strings_data);
    internal_avro::encode(e, v.vector3_data);
    internal_avro::encode(e, v.vector4_data);
    internal_avro::encode(e, v.vector3s_data);
  }
  static void decode(Decoder& d, RMF::avro2::DataTypes& v) {
    internal_avro::decode(d, v.int_data);
    internal_avro::decode(d, v.float_data);
    internal_avro::decode(d, v.string_data);
    internal_avro::decode(d, v.ints_data);
    internal_avro::decode(d, v.floats_data);
    internal_avro::decode(d, v.strings_data);
    internal_avro::decode(d, v.vector3_data);
    internal_avro::decode(d, v.vector4_data);
    internal_avro::decode(d, v.vector3s_data);
  }
};

template <>
struct codec_traits<RMF::avro2::Frame> {
  static void encode(Encoder& e, const RMF::avro2::Frame& v) {
    e.encodeUnionIndex(0);
    internal_avro::encode(e, v.id);
    internal_avro::encode(e, v.name);
    internal_avro::encode(e, v.type);
    internal_avro::encode(e, v.parents);

    internal_avro::encode(e, v.nodes);
    internal_avro::encode(e, v.keys);
    internal_avro::encode(e, v.data);
  }
  static void decode(Decoder& d, RMF::avro2::Frame& v) {
    size_t n = d.decodeUnionIndex();
    if (n == 0) {
      internal_avro::decode(d, v.id);
      internal_avro::decode(d, v.name);
      internal_avro::decode(d, v.type);
      internal_avro::decode(d, v.parents);
    } else {
      RMF::avro2::Skip<std::string> junk_string;
      internal_avro::decode(d, junk_string);
      internal_avro::decode(d, junk_string);
      RMF::avro2::Skip<std::vector<std::pair<int32_t, std::string> > >
          junk_labels;
      internal_avro::decode(d, junk_labels);
      internal_avro::decode(d, junk_labels);
      internal_avro::decode(d, junk_labels);
      RMF::avro2::Skip<std::vector<std::pair<int32_t, RMF::NodeIDs> > >
          junk_node_sets;
      internal_avro::decode(d, junk_node_sets);
    }
    RMF::avro2::Skip<std::vector<RMF::avro2::HierarchyNode> > nodes;
    internal_avro::decode(d, nodes);
    RMF::avro2::Skip<std::vector<RMF::avro2::KeyInfo> > keys;
    internal_avro::decode(d, keys);

    internal_avro::decode(d, v.data);
  }
};

template <>
struct codec_traits<RMF::avro2::FileData> {
  static void decode(Decoder& d, RMF::avro2::FileData& v) {
    size_t n = d.decodeUnionIndex();
    if (n == 0) {
      internal_avro::decode(d, v.cur_id);
      v.max_id = std::max(v.max_id, v.cur_id);
      RMF::avro2::Skip<std::string> junk_name;
      internal_avro::decode(d, junk_name);
      RMF::avro2::Skip<RMF::FrameType> junk_type;
      internal_avro::decode(d, junk_type);
      RMF::FrameIDs frame_parents;
      internal_avro::decode(d, frame_parents);
      RMF_INFO(RMF::get_logger(), "Found frame " << v.cur_id);
      RMF_FOREACH(RMF::FrameID p, frame_parents) {
        v.frame_children.resize(
            std::max<std::size_t>(v.frame_children.size(), p.get_index() + 1));
        v.frame_children[p.get_index()].push_back(v.cur_id);
      }
    } else {
      RMF_INFO(RMF::get_logger(), "Found static frame");
      v.cur_id = RMF::FrameID();
      internal_avro::decode(d, v.description);
      internal_avro::decode(d, v.producer);
      std::vector<std::pair<RMF::Category, std::string> > categories;
      internal_avro::decode(d, categories);
      v.categories.insert(v.categories.end(), categories.begin(),
                          categories.end());
      internal_avro::decode(d, v.extra_node_types);
      internal_avro::decode(d, v.extra_frame_types);

      internal_avro::decode(d, v.node_sets);
    }
    std::vector<RMF::avro2::HierarchyNode> nodes;
    internal_avro::decode(d, nodes);
    RMF_FOREACH(const RMF::avro2::HierarchyNode & hn, nodes) {
      v.nodes.resize(
          std::max<std::size_t>(v.nodes.size(), hn.id.get_index() + 1));
      v.nodes[hn.id.get_index()].name = hn.name;
      v.nodes[hn.id.get_index()].type = hn.type;
      v.nodes[hn.id.get_index()]
          .parents.insert(v.nodes[hn.id.get_index()].parents.end(),
                          hn.parents.begin(), hn.parents.end());
      RMF_FOREACH(RMF::NodeID ch, hn.parents) {
        v.nodes.resize(
            std::max<std::size_t>(v.nodes.size(), ch.get_index() + 1));
        v.nodes[ch.get_index()].children.push_back(hn.id);
      }
    }

    std::vector<RMF::avro2::KeyInfo> keys;
    internal_avro::decode(d, keys);
    RMF::avro2::sort_keys(keys, v.keys);

    if (n == 1) {
      internal_avro::decode(d, v.data);
    } else {
      RMF::avro2::Skip<RMF::avro2::DataTypes> skip_data;
      internal_avro::decode(d, skip_data);
    }
  }
};

template <>
struct codec_traits<RMF::avro2::FileDataChanges> {
  static void encode(Encoder& e, const RMF::avro2::FileDataChanges& v) {
    e.encodeUnionIndex(1);
    internal_avro::encode(e, v.description);
    internal_avro::encode(e, v.producer);
    internal_avro::encode(e, v.categories);
    internal_avro::encode(e, v.node_types);
    internal_avro::encode(e, v.frame_types);
    internal_avro::encode(e, v.node_sets);

    internal_avro::encode(e, v.nodes);
    internal_avro::encode(e, v.keys);
    internal_avro::encode(e, v.data);
  }
};
}
#endif  // RMF_AVRO2_TYPE_ENCODE_DECODE_H
