#ifndef RMF_AVRO2_ENCODE_DECODE_H
#define RMF_AVRO2_ENCODE_DECODE_H

#include <boost/any.hpp>
#include "avrocpp/api/Specific.hh"
#include "avrocpp/api/Encoder.hh"
#include "avrocpp/api/Decoder.hh"
#include "RMF/Vector.h"
#include "RMF/ID.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <set>
#if BOOST_VERSION >= 104900
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#endif

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {
template <class B>
struct Skip {};

template <class V>
void add_to_list(std::vector<V>& in, const std::vector<V>& to_add) {
  if (to_add.empty()) return;
  in.insert(in.end(), to_add.begin(), to_add.end());
  std::sort(in.begin(), in.end());
  in.erase(std::unique(in.begin(), in.end()), in.end());
}
template <class V>
void add_to_list(std::vector<V>& in, const V& to_add) {
  typename std::vector<V>::iterator it =
      std::lower_bound(in.begin(), in.end(), to_add);
  if (it == in.end() || *it != to_add) in.insert(it, to_add);
}
}
}

namespace internal_avro {

template <class B>
struct codec_traits<RMF::avro2::Skip<B> > {
  static void encode(Encoder&, RMF::avro2::Skip<B>) {}
  static void decode(Decoder& d, RMF::avro2::Skip<B>&) {
    B b;
    internal_avro::decode(d, b);
  }
};

template <unsigned int V>
struct codec_traits<RMF_VECTOR<V> > {
  static void encode(Encoder& e, const RMF_VECTOR<V>& v) {
    for (unsigned int i = 0; i < V; ++i) {
      internal_avro::encode<float>(e, v[i]);
    }
  }
  static void decode(Decoder& d, RMF_VECTOR<V>& v) {
    for (unsigned int i = 0; i < V; ++i) {
      internal_avro::decode<float>(d, v[i]);
    }
  }
};

template <>
struct codec_traits<RMF::NodeType> {
  static void encode(Encoder& e, const RMF::NodeType& v) {
    internal_avro::encode<int32_t>(e, v);
  }
  static void decode(Decoder& d, RMF::NodeType& v) {
    int32_t vv;
    internal_avro::decode(d, vv);
    v = RMF::NodeType(vv);
  }
};

template <>
struct codec_traits<RMF::FrameType> {
  static void encode(Encoder& e, const RMF::FrameType& v) {
    internal_avro::encode<int32_t>(e, v);
  }
  static void decode(Decoder& d, RMF::FrameType& v) {
    int32_t vv;
    internal_avro::decode(d, vv);
    v = RMF::FrameType(vv);
  }
};

template <class V>
struct codec_traits<RMF::ID<V> > {
  static void encode(Encoder& e, const RMF::ID<V>& v) {
    RMF_INTERNAL_CHECK(v != RMF::ID<V>(), "Uninitialized ID");
    internal_avro::encode<int32_t>(e, v.get_index_always());
  }
  static void decode(Decoder& d, RMF::ID<V>& v) {
    int32_t index;
    internal_avro::decode(d, index);
    if (index >= 0) {
      v = RMF::ID<V>(index);
    } else {
      v = RMF::ID<V>(index, typename RMF::ID<V>::SpecialTag());
    }
  }
};

template <class K, class V>
struct codec_traits<std::pair<K, V> > {
  static void encode(Encoder& e, const std::pair<K, V>& v) {
    internal_avro::encode(e, v.first);
    internal_avro::encode(e, v.second);
  }
  static void decode(Decoder& d, std::pair<K, V>& v) {
    internal_avro::decode(d, v.first);
    internal_avro::decode(d, v.second);
  }
};

template <class K, class V>
struct codec_traits<boost::unordered_map<K, V> > {
  typedef std::pair<K, V> KP;
  static void encode(Encoder& e, const boost::unordered_map<K, V>& v) {
    std::vector<KP> pairs(v.begin(), v.end());
    internal_avro::encode(e, pairs);
  }
  static void decode(Decoder& d, boost::unordered_map<K, V>& v) {
    std::vector<KP> values;
    internal_avro::decode(d, values);
    v.insert(values.begin(), values.end());
  }
};

template <class K>
struct codec_traits<boost::unordered_set<K> > {
  static void encode(Encoder& e, const boost::unordered_set<K>& v) {
    std::vector<K> values(v.begin(), v.end());
    internal_avro::encode(e, values);
  }
  static void decode(Decoder& d, boost::unordered_set<K>& v) {
    std::vector<K> values;
    internal_avro::decode(d, values);
    v.insert(values.begin(), values.end());
  }
};

#if BOOST_VERSION >= 104900
template <class K, class V>
struct codec_traits<boost::container::flat_map<K, V> > {
  typedef std::pair<K, V> KP;
  static void encode(Encoder& e, const boost::container::flat_map<K, V>& v) {
    std::vector<KP> pairs(v.begin(), v.end());
    internal_avro::encode(e, pairs);
  }
  static void decode(Decoder& d, boost::container::flat_map<K, V>& v) {
    std::vector<KP> values;
    internal_avro::decode(d, values);
    v.insert(values.begin(), values.end());
  }
};

template <class K>
struct codec_traits<boost::container::flat_set<K> > {
  static void encode(Encoder& e, const boost::container::flat_set<K>& v) {
    std::vector<K> values(v.begin(), v.end());
    internal_avro::encode(e, values);
  }
  static void decode(Decoder& d, boost::container::flat_set<K>& v) {
    std::vector<K> values;
    internal_avro::decode(d, values);
    v.insert(values.begin(), values.end());
  }
};
#endif

template <class K, class V>
struct codec_traits<std::map<K, V> > {
  typedef std::pair<K, V> KP;
  static void encode(Encoder& e, const std::map<K, V>& v) {
    std::vector<KP> pairs(v.begin(), v.end());
    internal_avro::encode(e, pairs);
  }
  static void decode(Decoder& d, std::map<K, V>& v) {
    std::vector<KP> values;
    internal_avro::decode(d, values);
    v.insert(values.begin(), values.end());
  }
};

template <class K>
struct codec_traits<std::set<K> > {
  static void encode(Encoder& e, const std::set<K>& v) {
    std::vector<K> values(v.begin(), v.end());
    internal_avro::encode(e, values);
  }
  static void decode(Decoder& d, std::set<K>& v) {
    std::vector<K> values;
    internal_avro::decode(d, values);
    v.insert(values.begin(), values.end());
  }
};
}

RMF_DISABLE_WARNINGS

#endif
