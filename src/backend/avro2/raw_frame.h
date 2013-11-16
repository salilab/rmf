/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef SRC_BACKEND_AVRO2_RAW_FRAME_H_2514367941__H_
#define SRC_BACKEND_AVRO2_RAW_FRAME_H_2514367941__H_


#include "boost/any.hpp"
#include "backend/AvroCpp/api/Specific.hh"
#include "backend/AvroCpp/api/Encoder.hh"
#include "backend/AvroCpp/api/Decoder.hh"

namespace rmf_raw_avro2 {
struct FrameInfo {
    int32_t id;
    std::string name;
    int32_t type;
    std::vector<int32_t > parents;
};

struct Label {
    int32_t id;
    std::string name;
};

struct NodeSet {
    int32_t id;
    std::vector<int32_t > nodes;
};

struct FileInfo {
    std::string description;
    std::string producer;
    std::vector<Label > categories;
    std::vector<Label > node_types;
    std::vector<Label > frame_types;
    std::vector<NodeSet > node_sets;
};

struct _Frame_json_Union__0__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    FrameInfo get_FrameInfo() const;
    void set_FrameInfo(const FrameInfo& v);
    FileInfo get_FileInfo() const;
    void set_FileInfo(const FileInfo& v);
    _Frame_json_Union__0__();
};

struct Node {
    int32_t id;
    std::string name;
    int32_t type;
    std::vector<int32_t > parents;
};

enum Type {
    INT,
    FLOAT,
    STRING,
    INTS,
    FLOATS,
    STRINGS,
    VECTOR3,
    VECTOR4,
    VECTOR3S,
};

struct KeyInfo {
    int32_t id;
    std::string name;
    int32_t category;
    Type type;
};

struct IntValue {
    int32_t id;
    int32_t value;
};

struct IntNodeData {
    int32_t key;
    std::vector<IntValue > values;
};

struct FloatValue {
    int32_t id;
    float value;
};

struct FloatNodeData {
    int32_t key;
    std::vector<FloatValue > values;
};

struct StringValue {
    int32_t id;
    std::string value;
};

struct StringNodeData {
    int32_t key;
    std::vector<StringValue > values;
};

struct IntsValue {
    int32_t key;
    std::vector<int32_t > value;
};

struct IntsNodeData {
    int32_t key;
    std::vector<IntsValue > values;
};

struct FloatsValue {
    int32_t id;
    std::vector<float > value;
};

struct FloatsNodeData {
    int32_t id;
    std::vector<FloatsValue > values;
};

struct StringsValue {
    int32_t id;
    std::vector<std::string > value;
};

struct StringsNodeData {
    int32_t id;
    std::vector<StringsValue > values;
};

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Vector3Value {
    int32_t id;
    Vector3 value;
};

struct Vector3NodeData {
    int32_t key;
    std::vector<Vector3Value > values;
};

struct Vector4 {
    float w;
    float x;
    float y;
    float z;
};

struct Vector4Value {
    int32_t id;
    Vector4 value;
};

struct Vector4NodeData {
    int32_t key;
    std::vector<Vector4Value > values;
};

struct Vector3sValue {
    int32_t id;
    std::vector<Vector3 > value;
};

struct Vector3sNodeData {
    int32_t key;
    std::vector<Vector3sValue > values;
};

struct TypeDatas {
    std::vector<IntNodeData > int_data;
    std::vector<FloatNodeData > float_data;
    std::vector<StringNodeData > string_data;
    std::vector<IntsNodeData > ints_data;
    std::vector<FloatsNodeData > floats_data;
    std::vector<StringsNodeData > strings_data;
    std::vector<Vector3NodeData > vector3_data;
    std::vector<Vector4NodeData > vector4_data;
    std::vector<Vector3sNodeData > vector3s_data;
};

struct Frame {
    typedef _Frame_json_Union__0__ info_t;
    info_t info;
    std::vector<Node > nodes;
    std::vector<KeyInfo > keys;
    TypeDatas data;
};

inline
FrameInfo _Frame_json_Union__0__::get_FrameInfo() const {
    if (idx_ != 0) {
        throw rmf_avro::Exception("Invalid type for union");
    }
    return boost::any_cast<FrameInfo >(value_);
}

inline
void _Frame_json_Union__0__::set_FrameInfo(const FrameInfo& v) {
    idx_ = 0;
    value_ = v;
}

inline
FileInfo _Frame_json_Union__0__::get_FileInfo() const {
    if (idx_ != 1) {
        throw rmf_avro::Exception("Invalid type for union");
    }
    return boost::any_cast<FileInfo >(value_);
}

inline
void _Frame_json_Union__0__::set_FileInfo(const FileInfo& v) {
    idx_ = 1;
    value_ = v;
}

inline _Frame_json_Union__0__::_Frame_json_Union__0__() : idx_(0), value_(FrameInfo()) { }
}
namespace rmf_avro {
template<> struct codec_traits<rmf_raw_avro2::FrameInfo> {
    static void encode(Encoder& e, const rmf_raw_avro2::FrameInfo& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.name);
        rmf_avro::encode(e, v.type);
        rmf_avro::encode(e, v.parents);
    }
    static void decode(Decoder& d, rmf_raw_avro2::FrameInfo& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.name);
        rmf_avro::decode(d, v.type);
        rmf_avro::decode(d, v.parents);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Label> {
    static void encode(Encoder& e, const rmf_raw_avro2::Label& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.name);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Label& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.name);
    }
};

template<> struct codec_traits<rmf_raw_avro2::NodeSet> {
    static void encode(Encoder& e, const rmf_raw_avro2::NodeSet& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.nodes);
    }
    static void decode(Decoder& d, rmf_raw_avro2::NodeSet& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.nodes);
    }
};

template<> struct codec_traits<rmf_raw_avro2::FileInfo> {
    static void encode(Encoder& e, const rmf_raw_avro2::FileInfo& v) {
        rmf_avro::encode(e, v.description);
        rmf_avro::encode(e, v.producer);
        rmf_avro::encode(e, v.categories);
        rmf_avro::encode(e, v.node_types);
        rmf_avro::encode(e, v.frame_types);
        rmf_avro::encode(e, v.node_sets);
    }
    static void decode(Decoder& d, rmf_raw_avro2::FileInfo& v) {
        rmf_avro::decode(d, v.description);
        rmf_avro::decode(d, v.producer);
        rmf_avro::decode(d, v.categories);
        rmf_avro::decode(d, v.node_types);
        rmf_avro::decode(d, v.frame_types);
        rmf_avro::decode(d, v.node_sets);
    }
};

template<> struct codec_traits<rmf_raw_avro2::_Frame_json_Union__0__> {
    static void encode(Encoder& e, rmf_raw_avro2::_Frame_json_Union__0__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            rmf_avro::encode(e, v.get_FrameInfo());
            break;
        case 1:
            rmf_avro::encode(e, v.get_FileInfo());
            break;
        }
    }
    static void decode(Decoder& d, rmf_raw_avro2::_Frame_json_Union__0__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw rmf_avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            {
                rmf_raw_avro2::FrameInfo vv;
                rmf_avro::decode(d, vv);
                v.set_FrameInfo(vv);
            }
            break;
        case 1:
            {
                rmf_raw_avro2::FileInfo vv;
                rmf_avro::decode(d, vv);
                v.set_FileInfo(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<rmf_raw_avro2::Node> {
    static void encode(Encoder& e, const rmf_raw_avro2::Node& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.name);
        rmf_avro::encode(e, v.type);
        rmf_avro::encode(e, v.parents);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Node& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.name);
        rmf_avro::decode(d, v.type);
        rmf_avro::decode(d, v.parents);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Type> {
    static void encode(Encoder& e, rmf_raw_avro2::Type v) {
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Type& v) {
        v = static_cast<rmf_raw_avro2::Type>(d.decodeEnum());
    }
};

template<> struct codec_traits<rmf_raw_avro2::KeyInfo> {
    static void encode(Encoder& e, const rmf_raw_avro2::KeyInfo& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.name);
        rmf_avro::encode(e, v.category);
        rmf_avro::encode(e, v.type);
    }
    static void decode(Decoder& d, rmf_raw_avro2::KeyInfo& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.name);
        rmf_avro::decode(d, v.category);
        rmf_avro::decode(d, v.type);
    }
};

template<> struct codec_traits<rmf_raw_avro2::IntValue> {
    static void encode(Encoder& e, const rmf_raw_avro2::IntValue& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::IntValue& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::IntNodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::IntNodeData& v) {
        rmf_avro::encode(e, v.key);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::IntNodeData& v) {
        rmf_avro::decode(d, v.key);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::FloatValue> {
    static void encode(Encoder& e, const rmf_raw_avro2::FloatValue& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::FloatValue& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::FloatNodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::FloatNodeData& v) {
        rmf_avro::encode(e, v.key);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::FloatNodeData& v) {
        rmf_avro::decode(d, v.key);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::StringValue> {
    static void encode(Encoder& e, const rmf_raw_avro2::StringValue& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::StringValue& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::StringNodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::StringNodeData& v) {
        rmf_avro::encode(e, v.key);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::StringNodeData& v) {
        rmf_avro::decode(d, v.key);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::IntsValue> {
    static void encode(Encoder& e, const rmf_raw_avro2::IntsValue& v) {
        rmf_avro::encode(e, v.key);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::IntsValue& v) {
        rmf_avro::decode(d, v.key);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::IntsNodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::IntsNodeData& v) {
        rmf_avro::encode(e, v.key);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::IntsNodeData& v) {
        rmf_avro::decode(d, v.key);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::FloatsValue> {
    static void encode(Encoder& e, const rmf_raw_avro2::FloatsValue& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::FloatsValue& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::FloatsNodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::FloatsNodeData& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::FloatsNodeData& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::StringsValue> {
    static void encode(Encoder& e, const rmf_raw_avro2::StringsValue& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::StringsValue& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::StringsNodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::StringsNodeData& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::StringsNodeData& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Vector3> {
    static void encode(Encoder& e, const rmf_raw_avro2::Vector3& v) {
        rmf_avro::encode(e, v.x);
        rmf_avro::encode(e, v.y);
        rmf_avro::encode(e, v.z);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Vector3& v) {
        rmf_avro::decode(d, v.x);
        rmf_avro::decode(d, v.y);
        rmf_avro::decode(d, v.z);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Vector3Value> {
    static void encode(Encoder& e, const rmf_raw_avro2::Vector3Value& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Vector3Value& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Vector3NodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::Vector3NodeData& v) {
        rmf_avro::encode(e, v.key);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Vector3NodeData& v) {
        rmf_avro::decode(d, v.key);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Vector4> {
    static void encode(Encoder& e, const rmf_raw_avro2::Vector4& v) {
        rmf_avro::encode(e, v.w);
        rmf_avro::encode(e, v.x);
        rmf_avro::encode(e, v.y);
        rmf_avro::encode(e, v.z);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Vector4& v) {
        rmf_avro::decode(d, v.w);
        rmf_avro::decode(d, v.x);
        rmf_avro::decode(d, v.y);
        rmf_avro::decode(d, v.z);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Vector4Value> {
    static void encode(Encoder& e, const rmf_raw_avro2::Vector4Value& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Vector4Value& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Vector4NodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::Vector4NodeData& v) {
        rmf_avro::encode(e, v.key);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Vector4NodeData& v) {
        rmf_avro::decode(d, v.key);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Vector3sValue> {
    static void encode(Encoder& e, const rmf_raw_avro2::Vector3sValue& v) {
        rmf_avro::encode(e, v.id);
        rmf_avro::encode(e, v.value);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Vector3sValue& v) {
        rmf_avro::decode(d, v.id);
        rmf_avro::decode(d, v.value);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Vector3sNodeData> {
    static void encode(Encoder& e, const rmf_raw_avro2::Vector3sNodeData& v) {
        rmf_avro::encode(e, v.key);
        rmf_avro::encode(e, v.values);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Vector3sNodeData& v) {
        rmf_avro::decode(d, v.key);
        rmf_avro::decode(d, v.values);
    }
};

template<> struct codec_traits<rmf_raw_avro2::TypeDatas> {
    static void encode(Encoder& e, const rmf_raw_avro2::TypeDatas& v) {
        rmf_avro::encode(e, v.int_data);
        rmf_avro::encode(e, v.float_data);
        rmf_avro::encode(e, v.string_data);
        rmf_avro::encode(e, v.ints_data);
        rmf_avro::encode(e, v.floats_data);
        rmf_avro::encode(e, v.strings_data);
        rmf_avro::encode(e, v.vector3_data);
        rmf_avro::encode(e, v.vector4_data);
        rmf_avro::encode(e, v.vector3s_data);
    }
    static void decode(Decoder& d, rmf_raw_avro2::TypeDatas& v) {
        rmf_avro::decode(d, v.int_data);
        rmf_avro::decode(d, v.float_data);
        rmf_avro::decode(d, v.string_data);
        rmf_avro::decode(d, v.ints_data);
        rmf_avro::decode(d, v.floats_data);
        rmf_avro::decode(d, v.strings_data);
        rmf_avro::decode(d, v.vector3_data);
        rmf_avro::decode(d, v.vector4_data);
        rmf_avro::decode(d, v.vector3s_data);
    }
};

template<> struct codec_traits<rmf_raw_avro2::Frame> {
    static void encode(Encoder& e, const rmf_raw_avro2::Frame& v) {
        rmf_avro::encode(e, v.info);
        rmf_avro::encode(e, v.nodes);
        rmf_avro::encode(e, v.keys);
        rmf_avro::encode(e, v.data);
    }
    static void decode(Decoder& d, rmf_raw_avro2::Frame& v) {
        rmf_avro::decode(d, v.info);
        rmf_avro::decode(d, v.nodes);
        rmf_avro::decode(d, v.keys);
        rmf_avro::decode(d, v.data);
    }
};

}
#endif
