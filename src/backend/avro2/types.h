
#ifndef RMF_AVRO2_TYPES_H
#define RMF_AVRO2_TYPES_H

#include <RMF/config.h>
#include <RMF/ID.h>
#include <RMF/types.h>
#include <RMF/internal/SharedDataHierarchy.h>
#include <RMF/internal/SharedDataData.h>
#include <RMF/internal/small_set_map.h>

#include "avrocpp/api/Specific.hh"
#include "avrocpp/api/Encoder.hh"
#include "avrocpp/api/Decoder.hh"

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace avro2 {

template <class Traits>
struct KeyMaps {
  RMF_SMALL_UNORDERED_MAP<ID<Traits>, Category> category;
  RMF_SMALL_UNORDERED_MAP<ID<Traits>, std::string> name;
};

struct KeyData {
  KeyMaps<FloatTraits> float_keys;
  KeyMaps<IntTraits> int_keys;
  KeyMaps<StringTraits> string_keys;
  KeyMaps<FloatsTraits> floats_keys;
  KeyMaps<IntsTraits> ints_keys;
  KeyMaps<StringsTraits> strings_keys;
  KeyMaps<Vector3Traits> vector3_keys;
  KeyMaps<Vector4Traits> vector4_keys;
  KeyMaps<Vector3sTraits> vector3s_keys;
};

typedef internal::TypeData<FloatTraits> FloatData;
typedef internal::TypeData<StringTraits> StringData;
typedef internal::TypeData<IntTraits> IntData;
typedef internal::TypeData<FloatsTraits> FloatsData;
typedef internal::TypeData<StringsTraits> StringsData;
typedef internal::TypeData<IntsTraits> IntsData;
typedef internal::TypeData<Vector3Traits> Vector3Data;
typedef internal::TypeData<Vector4Traits> Vector4Data;
typedef internal::TypeData<Vector3sTraits> Vector3sData;

struct HierarchyNode {
  NodeID id;
  std::string name;
  NodeType type;
  NodeIDs parents;
};

struct DataTypes {
  FloatData float_data;
  StringData string_data;
  IntData int_data;
  StringsData strings_data;
  FloatsData floats_data;
  IntsData ints_data;
  Vector3Data vector3_data;
  Vector4Data vector4_data;
  Vector3sData vector3s_data;
};

enum KeyType {
  INT,
  FLOAT,
  STRING,
  INTS,
  FLOATS,
  STRINGS,
  VECTOR3,
  VECTOR4,
  VECTOR3S
};

struct KeyInfo {
  std::string name;
  Category category;
  int32_t id;
  KeyType type;
};

struct StringAccumulator : public std::string {
  void set(std::string v) { std::string::operator=(v); }
};

struct FileData {
  // only used during parsing
  FrameID cur_id;
  StringAccumulator description;
  StringAccumulator producer;
  std::vector<std::pair<Category, std::string> > categories;
  RMF_SMALL_UNORDERED_MAP<int, std::string> extra_node_types;
  RMF_SMALL_UNORDERED_MAP<int, std::string> extra_frame_types;
  RMF_SMALL_UNORDERED_MAP<int32_t, RMF_SMALL_UNORDERED_SET<RMF::NodeID> >
      node_sets;

  FrameID max_id;
  std::vector<internal::HierarchyNode<NodeID, NodeType> > nodes;
  std::vector<FrameIDs> frame_children;
  KeyData keys;
  DataTypes data;
  RMF_LARGE_UNORDERED_MAP<FrameID, int32_t> frame_block_offsets;
};

struct FileDataChanges {
  std::string description;
  std::string producer;
  std::vector<std::pair<Category, std::string> > categories;
  std::vector<std::pair<NodeType, std::string> > node_types;
  std::vector<std::pair<FrameType, std::string> > frame_types;
  std::vector<std::pair<int32_t, RMF::NodeIDs> > node_sets;

  std::vector<HierarchyNode> nodes;
  std::vector<KeyInfo> keys;
  DataTypes data;
};

struct Frame {
  FrameID id;
  std::string name;
  FrameType type;
  FrameIDs parents;
  std::vector<HierarchyNode> nodes;
  std::vector<KeyInfo> keys;
  DataTypes data;
};
}
}

RMF_DISABLE_WARNINGS
#endif  // RMF_AVRO2_TYPES_H
