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
#include <sstream>

#include "Compiler.hh"
#include "Types.hh"
#include "Schema.hh"
#include "ValidSchema.hh"
#include "Stream.hh"

#include "json/JsonDom.hh"

extern void yyparse(void *ctx);

using std::map;
using std::vector;

namespace rmf_avro {

typedef map<Name, NodePtr> SymbolTable;

using json::Entity;

// #define DEBUG_VERBOSE

static NodePtr makePrimitive(const std::string& t)
{
    if (t == "null") {
        return NodePtr(new NodePrimitive(AVRO_NULL));
    } else if (t == "boolean") {
        return NodePtr(new NodePrimitive(AVRO_BOOL));
    } else if (t == "int") {
        return NodePtr(new NodePrimitive(AVRO_INT));
    } else if (t == "long") {
        return NodePtr(new NodePrimitive(AVRO_LONG));
    } else if (t == "float") {
        return NodePtr(new NodePrimitive(AVRO_FLOAT));
    } else if (t == "double") {
        return NodePtr(new NodePrimitive(AVRO_DOUBLE));
    } else if (t == "string") {
        return NodePtr(new NodePrimitive(AVRO_STRING));
    } else if (t == "bytes") {
        return NodePtr(new NodePrimitive(AVRO_BYTES));
    } else {
        return NodePtr();
    }
}

static NodePtr makeNode(const json::Entity& e, SymbolTable& st, const std::string& ns);

template <typename T>
concepts::SingleAttribute<T> asSingleAttribute(const T& t)
{
    concepts::SingleAttribute<T> n;
    n.add(t);
    return n;
}

static bool isFullName(const std::string& s)
{
    return s.find('.') != std::string::npos;
}

static Name getName(const std::string& name, const std::string& ns)
{
    return (isFullName(name)) ? Name(name) : Name(name, ns);
}

static NodePtr makeNode(const std::string& t, SymbolTable& st, const std::string& ns)
{
    NodePtr result = makePrimitive(t);
    if (result) {
        return result;
    }
    Name n = getName(t, ns);

    map<Name, NodePtr>::const_iterator it = st.find(n);
    if (it != st.end()) {
        return NodePtr(new NodeSymbolic(asSingleAttribute(n), it->second));
    }
    throw Exception(boost::format("Unknown type: %1%") % n.fullname());
}

const map<std::string, Entity>::const_iterator findField(const Entity& e,
    const map<std::string, Entity>& m, const std::string& fieldName)
{
    map<std::string, Entity>::const_iterator it = m.find(fieldName);
    if (it == m.end()) {
        throw Exception(boost::format("Missing Json field \"%1%\": %2%") %
            fieldName % e.toString());
    } else {
        return it;
    }
}

template<typename T>
const T& getField(const Entity& e, const map<std::string, Entity>& m,
    const std::string& fieldName)
{
    map<std::string, Entity>::const_iterator it = findField(e, m, fieldName);
    if (it->second.type() != json::type_traits<T>::type()) {
        throw Exception(boost::format(
            "Json field \"%1%\" is not a %2%: %3%") %
                fieldName % json::type_traits<T>::name() %
                it->second.toString());
    } else {
        return it->second.value<T>();
    }
}

struct Field {
    const std::string& name;
    const NodePtr value;
    Field(const std::string& n, const NodePtr& v) : name(n), value(v) { }
};

static Field makeField(const Entity& e, SymbolTable& st, const std::string& ns)
{
    const map<std::string, Entity>& m = e.value<map<std::string, Entity> >();
    const std::string& n = getField<std::string>(e, m, "name");
    map<std::string, Entity>::const_iterator it = findField(e, m, "type");
    return Field(n, makeNode(it->second, st, ns));
}

static NodePtr makeRecordNode(const Entity& e,
    const Name& name, const map<std::string, Entity>& m, SymbolTable& st, const std::string& ns)
{
    const vector<Entity>& v = getField<vector<Entity> >(e, m, "fields");
    concepts::MultiAttribute<std::string> fieldNames;
    concepts::MultiAttribute<NodePtr> fieldValues;

    for (vector<Entity>::const_iterator it = v.begin(); it != v.end(); ++it) {
        Field f = makeField(*it, st, ns);
        fieldNames.add(f.name);
        fieldValues.add(f.value);
    }
    return NodePtr(new NodeRecord(asSingleAttribute(name),
        fieldValues, fieldNames));
}

static NodePtr makeEnumNode(const Entity& e,
    const Name& name, const map<std::string, Entity>& m)
{
    const vector<Entity>& v = getField<vector<Entity> >(e, m, "symbols");
    concepts::MultiAttribute<std::string> symbols;
    for (vector<Entity>::const_iterator it = v.begin(); it != v.end(); ++it) {
        if (it->type() != json::etString) {
            throw Exception(boost::format("Enum symbol not a string: %1%") %
                it->toString());
        }
        symbols.add(it->value<std::string>());
    }
    return NodePtr(new NodeEnum(asSingleAttribute(name), symbols));
}

static NodePtr makeFixedNode(const Entity& e,
    const Name& name, const map<std::string, Entity>& m)
{
    int v = static_cast<int>(getField<int64_t>(e, m, "size"));
    if (v <= 0) {
        throw Exception(boost::format("Size for fixed is not positive: ") %
            e.toString());
    }
    return NodePtr(new NodeFixed(asSingleAttribute(name),
        asSingleAttribute(v)));
}

static NodePtr makeArrayNode(const Entity& e, const map<std::string, Entity>& m,
    SymbolTable& st, const std::string& ns)
{
    map<std::string, Entity>::const_iterator it = findField(e, m, "items");
    return NodePtr(new NodeArray(asSingleAttribute(
        makeNode(it->second, st, ns))));
}

static NodePtr makeMapNode(const Entity& e, const map<std::string, Entity>& m,
    SymbolTable& st, const std::string& ns)
{
    map<std::string, Entity>::const_iterator it = findField(e, m, "values");

    return NodePtr(new NodeMap(asSingleAttribute(
        makeNode(it->second, st, ns))));
}

static Name getName(const Entity& e, const map<std::string, Entity>& m, const std::string& ns)
{
    const std::string& name = getField<std::string>(e, m, "name");

    if (isFullName(name)) {
        return Name(name);
    } else {
        map<std::string, Entity>::const_iterator it = m.find("namespace");
        if (it != m.end()) {
            if (it->second.type() != json::type_traits<std::string>::type()) {
                throw Exception(boost::format(
                    "Json field \"%1%\" is not a %2%: %3%") %
                        "namespace" % json::type_traits<std::string>::name() %
                        it->second.toString());
            }
            Name result = Name(name, it->second.value<std::string>());
            return result;
        }
        return Name(name, ns);
    }
}

static NodePtr makeNode(const Entity& e, const map<std::string, Entity>& m,
    SymbolTable& st, const std::string& ns)
{
    const std::string& type = getField<std::string>(e, m, "type");
    if (NodePtr result = makePrimitive(type)) {
        if (m.size() > 1) {
            throw Exception(boost::format(
                "Unknown additional Json fields: %1%")
                % e.toString());
        } else {
            return result;
        }
    } else if (type == "record" || type == "error" ||
        type == "enum" || type == "fixed") {
        Name nm = getName(e, m, ns);
        NodePtr result;
        if (type == "record" || type == "error") {
            result = NodePtr(new NodeRecord());
            st[nm] = result;
            NodePtr r = makeRecordNode(e, nm, m, st, nm.ns());
            (boost::dynamic_pointer_cast<NodeRecord>(r))->swap(
                *boost::dynamic_pointer_cast<NodeRecord>(result));
        } else {
            result = (type == "enum") ? makeEnumNode(e, nm, m) :
                makeFixedNode(e, nm, m);
            st[nm] = result;
        }
        return result;
    } else if (type == "array") {
        return makeArrayNode(e, m, st, ns);
    } else if (type == "map") {
        return makeMapNode(e, m, st, ns);
    }
    throw Exception(boost::format("Unknown type definition: %1%")
        % e.toString());
}

static NodePtr makeNode(const Entity& e, const vector<Entity>& m,
    SymbolTable& st, const std::string& ns)
{
    concepts::MultiAttribute<NodePtr> mm;
    for (vector<Entity>::const_iterator it = m.begin(); it != m.end(); ++it) {
        mm.add(makeNode(*it, st, ns));
    }
    return NodePtr(new NodeUnion(mm));
}

static NodePtr makeNode(const json::Entity& e, SymbolTable& st, const std::string& ns)
{
    switch (e.type()) {
    case json::etString:
        return makeNode(e.value<std::string>(), st, ns);
    case json::etObject:
        return makeNode(e, e.value<map<std::string, Entity> >(), st, ns);
    case json::etArray:
        return makeNode(e, e.value<vector<Entity> >(), st, ns);
    default:
        throw Exception(boost::format("Invalid Avro type: %1%") % e.toString());
    }
}

ValidSchema compileJsonSchemaFromStream(InputStream& is)
{
    json::Entity e = json::loadEntity(is);
    SymbolTable st;
    NodePtr n = makeNode(e, st, "");
    return ValidSchema(n);
}

ValidSchema compileJsonSchemaFromMemory(const uint8_t* input, size_t len)
{
    return compileJsonSchemaFromStream(*memoryInputStream(input, len));
}

ValidSchema compileJsonSchemaFromString(const char* input)
{
    return compileJsonSchemaFromMemory(reinterpret_cast<const uint8_t*>(input),
        ::strlen(input));
}

static ValidSchema compile(std::istream& is)
{
    std::auto_ptr<InputStream> in = istreamInputStream(is);
    return compileJsonSchemaFromStream(*in);
}

void compileJsonSchema(std::istream &is, ValidSchema &schema)
{
    if (!is.good()) {
        throw Exception("Input stream is not good");
    }

    schema = compile(is);
}

bool compileJsonSchema(std::istream &is, ValidSchema &schema, std::string &error)
{
    try {
        compileJsonSchema(is, schema);
        return true;
    } catch (const Exception &e) {
        error = e.what();
        return false;
    }

}

} // namespace rmf_avro
