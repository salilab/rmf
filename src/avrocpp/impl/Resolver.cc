
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

#include "boost/scoped_array.hpp"
#include "Resolver.hh"
#include "Layout.hh"
#include "NodeImpl.hh"
#include "ValidSchema.hh"
#include "Reader.hh"
#include "Boost.hh"
#include "AvroTraits.hh"

namespace internal_avro {

class ResolverFactory;
typedef std::shared_ptr<Resolver> ResolverPtr;
typedef boost::ptr_vector<Resolver> ResolverPtrVector;

// #define DEBUG_VERBOSE

#ifdef DEBUG_VERBOSE
#define DEBUG_OUT(str) std::cout << str << '\n'
#else
class NoOp {};
template <typename T>
NoOp &operator<<(NoOp &noOp, const T &) {
  return noOp;
}
NoOp noop;
#define DEBUG_OUT(str) noop << str
#endif

template <typename T>
class PrimitiveSkipper : public Resolver {
 public:
  PrimitiveSkipper() : Resolver() {}

  virtual void parse(Reader &reader, uint8_t *address) const {
    T val;
    reader.readValue(val);
    DEBUG_OUT("Skipping " << val);
  }
};

template <typename T>
class PrimitiveParser : public Resolver {
 public:
  PrimitiveParser(const PrimitiveLayout &offset)
      : Resolver(), offset_(offset.offset()) {}

  virtual void parse(Reader &reader, uint8_t *address) const {
    T *location = reinterpret_cast<T *>(address + offset_);
    reader.readValue(*location);
    DEBUG_OUT("Reading " << *location);
  }

 private:
  size_t offset_;
};

template <typename WT, typename RT>
class PrimitivePromoter : public Resolver {
 public:
  PrimitivePromoter(const PrimitiveLayout &offset)
      : Resolver(), offset_(offset.offset()) {}

  virtual void parse(Reader &reader, uint8_t *address) const {
    parseIt<WT>(reader, address);
  }

 private:
  void parseIt(Reader &reader, uint8_t *address,
               const boost::true_type &) const {
    WT val;
    reader.readValue(val);
    RT *location = reinterpret_cast<RT *>(address + offset_);
    *location = static_cast<RT>(val);
    DEBUG_OUT("Promoting " << val);
  }

  void parseIt(Reader &reader, uint8_t *address,
               const boost::false_type &) const {}

  template <typename T>
  void parseIt(Reader &reader, uint8_t *address) const {
    parseIt(reader, address, is_promotable<T>());
  }

  size_t offset_;
};

template <>
class PrimitiveSkipper<std::vector<uint8_t> > : public Resolver {
 public:
  PrimitiveSkipper() : Resolver() {}

  virtual void parse(Reader &reader, uint8_t *address) const {
    std::vector<uint8_t> val;
    reader.readBytes(val);
    DEBUG_OUT("Skipping bytes");
  }
};

template <>
class PrimitiveParser<std::vector<uint8_t> > : public Resolver {
 public:
  PrimitiveParser(const PrimitiveLayout &offset)
      : Resolver(), offset_(offset.offset()) {}

  virtual void parse(Reader &reader, uint8_t *address) const {
    std::vector<uint8_t> *location =
        reinterpret_cast<std::vector<uint8_t> *>(address + offset_);
    reader.readBytes(*location);
    DEBUG_OUT("Reading bytes");
  }

 private:
  size_t offset_;
};

class RecordSkipper : public Resolver {
 public:
  RecordSkipper(ResolverFactory &factory, const NodePtr &writer);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Skipping record");

    reader.readRecord();
    size_t steps = resolvers_.size();
    for (size_t i = 0; i < steps; ++i) {
      resolvers_[i].parse(reader, address);
    }
  }

 protected:
  ResolverPtrVector resolvers_;
};

class RecordParser : public Resolver {
 public:
  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Reading record");

    reader.readRecord();
    size_t steps = resolvers_.size();
    for (size_t i = 0; i < steps; ++i) {
      resolvers_[i].parse(reader, address);
    }
  }

  RecordParser(ResolverFactory &factory, const NodePtr &writer,
               const NodePtr &reader, const CompoundLayout &offsets);

 protected:
  ResolverPtrVector resolvers_;
};

class MapSkipper : public Resolver {
 public:
  MapSkipper(ResolverFactory &factory, const NodePtr &writer);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Skipping map");

    std::string key;
    int64_t size = 0;
    do {
      size = reader.readMapBlockSize();
      for (int64_t i = 0; i < size; ++i) {
        reader.readValue(key);
        resolver_->parse(reader, address);
      }
    } while (size != 0);
  }

 protected:
  ResolverPtr resolver_;
};

class MapParser : public Resolver {
 public:
  typedef uint8_t *(*GenericMapSetter)(uint8_t *map, const std::string &key);

  MapParser(ResolverFactory &factory, const NodePtr &writer,
            const NodePtr &reader, const CompoundLayout &offsets);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Reading map");

    uint8_t *mapAddress = address + offset_;

    std::string key;
    GenericMapSetter *setter =
        reinterpret_cast<GenericMapSetter *>(address + setFuncOffset_);

    int64_t size = 0;
    do {
      size = reader.readMapBlockSize();
      for (int64_t i = 0; i < size; ++i) {
        reader.readValue(key);

        // create a new map entry and get the address
        uint8_t *location = (*setter)(mapAddress, key);
        resolver_->parse(reader, location);
      }
    } while (size != 0);
  }

 protected:
  ResolverPtr resolver_;
  size_t offset_;
  size_t setFuncOffset_;
};

class ArraySkipper : public Resolver {
 public:
  ArraySkipper(ResolverFactory &factory, const NodePtr &writer);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Skipping array");

    int64_t size = 0;
    do {
      size = reader.readArrayBlockSize();
      for (int64_t i = 0; i < size; ++i) {
        resolver_->parse(reader, address);
      }
    } while (size != 0);
  }

 protected:
  ResolverPtr resolver_;
};

typedef uint8_t *(*GenericArraySetter)(uint8_t *array);

class ArrayParser : public Resolver {
 public:
  ArrayParser(ResolverFactory &factory, const NodePtr &writer,
              const NodePtr &reader, const CompoundLayout &offsets);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Reading array");

    uint8_t *arrayAddress = address + offset_;

    GenericArraySetter *setter =
        reinterpret_cast<GenericArraySetter *>(address + setFuncOffset_);

    int64_t size = 0;
    do {
      size = reader.readArrayBlockSize();
      for (int64_t i = 0; i < size; ++i) {
        // create a new map entry and get the address
        uint8_t *location = (*setter)(arrayAddress);
        resolver_->parse(reader, location);
      }
    } while (size != 0);
  }

 protected:
  ArrayParser() : Resolver() {}

  ResolverPtr resolver_;
  size_t offset_;
  size_t setFuncOffset_;
};

class EnumSkipper : public Resolver {
 public:
  EnumSkipper(ResolverFactory &factory, const NodePtr &writer) : Resolver() {}

  virtual void parse(Reader &reader, uint8_t *address) const {
    int64_t val = reader.readEnum();
    DEBUG_OUT("Skipping enum" << val);
  }
};

class EnumParser : public Resolver {
 public:
  enum EnumRepresentation {
    VAL
  };

  EnumParser(ResolverFactory &factory, const NodePtr &writer,
             const NodePtr &reader, const CompoundLayout &offsets)
      : Resolver(),
        offset_(offsets.at(0).offset()),
        readerSize_(reader->names()) {
    const size_t writerSize = writer->names();

    mapping_.reserve(writerSize);

    for (size_t i = 0; i < writerSize; ++i) {
      const std::string &name = writer->nameAt(i);
      size_t readerIndex = readerSize_;
      reader->nameIndex(name, readerIndex);
      mapping_.push_back(readerIndex);
    }
  }

  virtual void parse(Reader &reader, uint8_t *address) const {
    size_t val = static_cast<size_t>(reader.readEnum());
    assert(static_cast<size_t>(val) < mapping_.size());

    if (mapping_[val] < readerSize_) {
      EnumRepresentation *location =
          reinterpret_cast<EnumRepresentation *>(address + offset_);
      *location = static_cast<EnumRepresentation>(mapping_[val]);
      DEBUG_OUT("Setting enum" << *location);
    }
  }

 protected:
  size_t offset_;
  size_t readerSize_;
  std::vector<size_t> mapping_;
};

class UnionSkipper : public Resolver {
 public:
  UnionSkipper(ResolverFactory &factory, const NodePtr &writer);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Skipping union");
    size_t choice = static_cast<size_t>(reader.readUnion());
    resolvers_[choice].parse(reader, address);
  }

 protected:
  ResolverPtrVector resolvers_;
};

class UnionParser : public Resolver {
 public:
  typedef uint8_t *(*GenericUnionSetter)(uint8_t *, int64_t);

  UnionParser(ResolverFactory &factory, const NodePtr &writer,
              const NodePtr &reader, const CompoundLayout &offsets);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Reading union");
    size_t writerChoice = static_cast<size_t>(reader.readUnion());
    int64_t *readerChoice =
        reinterpret_cast<int64_t *>(address + choiceOffset_);

    *readerChoice = choiceMapping_[writerChoice];
    GenericUnionSetter *setter =
        reinterpret_cast<GenericUnionSetter *>(address + setFuncOffset_);
    uint8_t *value = reinterpret_cast<uint8_t *>(address + offset_);
    uint8_t *location = (*setter)(value, *readerChoice);

    resolvers_[writerChoice].parse(reader, location);
  }

 protected:
  ResolverPtrVector resolvers_;
  std::vector<int64_t> choiceMapping_;
  size_t offset_;
  size_t choiceOffset_;
  size_t setFuncOffset_;
};

class UnionToNonUnionParser : public Resolver {
 public:
  typedef uint8_t *(*GenericUnionSetter)(uint8_t *, int64_t);

  UnionToNonUnionParser(ResolverFactory &factory, const NodePtr &writer,
                        const NodePtr &reader, const Layout &offsets);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Reading union to non-union");
    size_t choice = static_cast<size_t>(reader.readUnion());
    resolvers_[choice].parse(reader, address);
  }

 protected:
  ResolverPtrVector resolvers_;
};

class NonUnionToUnionParser : public Resolver {
 public:
  typedef uint8_t *(*GenericUnionSetter)(uint8_t *, int64_t);

  NonUnionToUnionParser(ResolverFactory &factory, const NodePtr &writer,
                        const NodePtr &reader, const CompoundLayout &offsets);

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Reading non-union to union");

    int64_t *choice = reinterpret_cast<int64_t *>(address + choiceOffset_);
    *choice = choice_;
    GenericUnionSetter *setter =
        reinterpret_cast<GenericUnionSetter *>(address + setFuncOffset_);
    uint8_t *value = reinterpret_cast<uint8_t *>(address + offset_);
    uint8_t *location = (*setter)(value, choice_);

    resolver_->parse(reader, location);
  }

 protected:
  ResolverPtr resolver_;
  size_t choice_;
  size_t offset_;
  size_t choiceOffset_;
  size_t setFuncOffset_;
};

class FixedSkipper : public Resolver {
 public:
  FixedSkipper(ResolverFactory &factory, const NodePtr &writer) : Resolver() {
    size_ = writer->fixedSize();
  }

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Skipping fixed");
    boost::scoped_array<uint8_t> val(new uint8_t[size_]);
    reader.readFixed(&val[0], size_);
  }

 protected:
  int size_;
};

class FixedParser : public Resolver {
 public:
  FixedParser(ResolverFactory &factory, const NodePtr &writer,
              const NodePtr &reader, const CompoundLayout &offsets)
      : Resolver() {
    size_ = writer->fixedSize();
    offset_ = offsets.at(0).offset();
  }

  virtual void parse(Reader &reader, uint8_t *address) const {
    DEBUG_OUT("Reading fixed");
    uint8_t *location = reinterpret_cast<uint8_t *>(address + offset_);
    reader.readFixed(location, size_);
  }

 protected:
  int size_;
  size_t offset_;
};

class ResolverFactory : private boost::noncopyable {

  template <typename T>
  Resolver *constructPrimitiveSkipper(const NodePtr &writer) {
    return new PrimitiveSkipper<T>();
  }

  template <typename T>
  Resolver *constructPrimitive(const NodePtr &writer, const NodePtr &reader,
                               const Layout &offset) {
    Resolver *instruction = 0;

    SchemaResolution match = writer->resolve(*reader);

    if (match == RESOLVE_NO_MATCH) {
      instruction = new PrimitiveSkipper<T>();
    } else if (reader->type() == AVRO_UNION) {
      const CompoundLayout &compoundLayout =
          static_cast<const CompoundLayout &>(offset);
      instruction =
          new NonUnionToUnionParser(*this, writer, reader, compoundLayout);
    } else if (match == RESOLVE_MATCH) {
      const PrimitiveLayout &primitiveLayout =
          static_cast<const PrimitiveLayout &>(offset);
      instruction = new PrimitiveParser<T>(primitiveLayout);
    } else if (match == RESOLVE_PROMOTABLE_TO_LONG) {
      const PrimitiveLayout &primitiveLayout =
          static_cast<const PrimitiveLayout &>(offset);
      instruction = new PrimitivePromoter<T, int64_t>(primitiveLayout);
    } else if (match == RESOLVE_PROMOTABLE_TO_FLOAT) {
      const PrimitiveLayout &primitiveLayout =
          static_cast<const PrimitiveLayout &>(offset);
      instruction = new PrimitivePromoter<T, float>(primitiveLayout);
    } else if (match == RESOLVE_PROMOTABLE_TO_DOUBLE) {
      const PrimitiveLayout &primitiveLayout =
          static_cast<const PrimitiveLayout &>(offset);
      instruction = new PrimitivePromoter<T, double>(primitiveLayout);
    } else {
      assert(0);
    }
    return instruction;
  }

  template <typename Skipper>
  Resolver *constructCompoundSkipper(const NodePtr &writer) {
    return new Skipper(*this, writer);
  }

  template <typename Parser, typename Skipper>
  Resolver *constructCompound(const NodePtr &writer, const NodePtr &reader,
                              const Layout &offset) {
    Resolver *instruction;

    SchemaResolution match = RESOLVE_NO_MATCH;

    match = writer->resolve(*reader);

    if (match == RESOLVE_NO_MATCH) {
      instruction = new Skipper(*this, writer);
    } else if (writer->type() != AVRO_UNION && reader->type() == AVRO_UNION) {
      const CompoundLayout &compoundLayout =
          dynamic_cast<const CompoundLayout &>(offset);
      instruction =
          new NonUnionToUnionParser(*this, writer, reader, compoundLayout);
    } else if (writer->type() == AVRO_UNION && reader->type() != AVRO_UNION) {
      instruction = new UnionToNonUnionParser(*this, writer, reader, offset);
    } else {
      const CompoundLayout &compoundLayout =
          dynamic_cast<const CompoundLayout &>(offset);
      instruction = new Parser(*this, writer, reader, compoundLayout);
    }

    return instruction;
  }

 public:
  Resolver *construct(const NodePtr &writer, const NodePtr &reader,
                      const Layout &offset) {

    typedef Resolver *(ResolverFactory::*BuilderFunc)(
        const NodePtr & writer, const NodePtr & reader, const Layout & offset);

    NodePtr currentWriter =
        (writer->type() == AVRO_SYMBOLIC) ? resolveSymbol(writer) : writer;

    NodePtr currentReader =
        (reader->type() == AVRO_SYMBOLIC) ? resolveSymbol(reader) : reader;

    static const BuilderFunc funcs[] = {
        &ResolverFactory::constructPrimitive<std::string>,
        &ResolverFactory::constructPrimitive<std::vector<uint8_t> >,
        &ResolverFactory::constructPrimitive<int32_t>,
        &ResolverFactory::constructPrimitive<int64_t>,
        &ResolverFactory::constructPrimitive<float>,
        &ResolverFactory::constructPrimitive<double>,
        &ResolverFactory::constructPrimitive<bool>,
        &ResolverFactory::constructPrimitive<Null>,
        &ResolverFactory::constructCompound<RecordParser, RecordSkipper>,
        &ResolverFactory::constructCompound<EnumParser, EnumSkipper>,
        &ResolverFactory::constructCompound<ArrayParser, ArraySkipper>,
        &ResolverFactory::constructCompound<MapParser, MapSkipper>,
        &ResolverFactory::constructCompound<UnionParser, UnionSkipper>,
        &ResolverFactory::constructCompound<FixedParser, FixedSkipper>};

    static_assert((sizeof(funcs) / sizeof(BuilderFunc)) ==
                  (AVRO_NUM_TYPES), "function table size mismatch");

    BuilderFunc func = funcs[currentWriter->type()];
    assert(func);

    return ((this)->*(func))(currentWriter, currentReader, offset);
  }

  Resolver *skipper(const NodePtr &writer) {

    typedef Resolver *(ResolverFactory::*BuilderFunc)(const NodePtr & writer);

    NodePtr currentWriter =
        (writer->type() == AVRO_SYMBOLIC) ? writer->leafAt(0) : writer;

    static const BuilderFunc funcs[] = {
        &ResolverFactory::constructPrimitiveSkipper<std::string>,
        &ResolverFactory::constructPrimitiveSkipper<std::vector<uint8_t> >,
        &ResolverFactory::constructPrimitiveSkipper<int32_t>,
        &ResolverFactory::constructPrimitiveSkipper<int64_t>,
        &ResolverFactory::constructPrimitiveSkipper<float>,
        &ResolverFactory::constructPrimitiveSkipper<double>,
        &ResolverFactory::constructPrimitiveSkipper<bool>,
        &ResolverFactory::constructPrimitiveSkipper<Null>,
        &ResolverFactory::constructCompoundSkipper<RecordSkipper>,
        &ResolverFactory::constructCompoundSkipper<EnumSkipper>,
        &ResolverFactory::constructCompoundSkipper<ArraySkipper>,
        &ResolverFactory::constructCompoundSkipper<MapSkipper>,
        &ResolverFactory::constructCompoundSkipper<UnionSkipper>,
        &ResolverFactory::constructCompoundSkipper<FixedSkipper>};

    static_assert((sizeof(funcs) / sizeof(BuilderFunc)) ==
                  (AVRO_NUM_TYPES), "function table size mismatch");

    BuilderFunc func = funcs[currentWriter->type()];
    assert(func);

    return ((this)->*(func))(currentWriter);
  }
};

RecordSkipper::RecordSkipper(ResolverFactory &factory, const NodePtr &writer)
    : Resolver() {
  size_t leaves = writer->leaves();
  resolvers_.reserve(leaves);
  for (size_t i = 0; i < leaves; ++i) {
    const NodePtr &w = writer->leafAt(i);
    resolvers_.push_back(factory.skipper(w));
  }
}

RecordParser::RecordParser(ResolverFactory &factory, const NodePtr &writer,
                           const NodePtr &reader, const CompoundLayout &offsets)
    : Resolver() {
  size_t leaves = writer->leaves();
  resolvers_.reserve(leaves);
  for (size_t i = 0; i < leaves; ++i) {

    const NodePtr &w = writer->leafAt(i);

    const std::string &name = writer->nameAt(i);

    size_t readerIndex = 0;
    bool found = reader->nameIndex(name, readerIndex);

    if (found) {
      const NodePtr &r = reader->leafAt(readerIndex);
      resolvers_.push_back(factory.construct(w, r, offsets.at(readerIndex)));
    } else {
      resolvers_.push_back(factory.skipper(w));
    }
  }
}

MapSkipper::MapSkipper(ResolverFactory &factory, const NodePtr &writer)
    : Resolver(), resolver_(factory.skipper(writer->leafAt(1))) {}

MapParser::MapParser(ResolverFactory &factory, const NodePtr &writer,
                     const NodePtr &reader, const CompoundLayout &offsets)
    : Resolver(),
      resolver_(factory.construct(writer->leafAt(1), reader->leafAt(1),
                                  offsets.at(1))),
      offset_(offsets.offset()),
      setFuncOffset_(offsets.at(0).offset()) {}

ArraySkipper::ArraySkipper(ResolverFactory &factory, const NodePtr &writer)
    : Resolver(), resolver_(factory.skipper(writer->leafAt(0))) {}

ArrayParser::ArrayParser(ResolverFactory &factory, const NodePtr &writer,
                         const NodePtr &reader, const CompoundLayout &offsets)
    : Resolver(),
      resolver_(factory.construct(writer->leafAt(0), reader->leafAt(0),
                                  offsets.at(1))),
      offset_(offsets.offset()),
      setFuncOffset_(offsets.at(0).offset()) {}

UnionSkipper::UnionSkipper(ResolverFactory &factory, const NodePtr &writer)
    : Resolver() {
  size_t leaves = writer->leaves();
  resolvers_.reserve(leaves);
  for (size_t i = 0; i < leaves; ++i) {
    const NodePtr &w = writer->leafAt(i);
    resolvers_.push_back(factory.skipper(w));
  }
}

namespace {

// asumes the writer is NOT a union, and the reader IS a union

SchemaResolution checkUnionMatch(const NodePtr &writer, const NodePtr &reader,
                                 size_t &index) {
  SchemaResolution bestMatch = RESOLVE_NO_MATCH;

  index = 0;
  size_t leaves = reader->leaves();

  for (size_t i = 0; i < leaves; ++i) {

    const NodePtr &leaf = reader->leafAt(i);
    SchemaResolution newMatch = writer->resolve(*leaf);

    if (newMatch == RESOLVE_MATCH) {
      bestMatch = newMatch;
      index = i;
      break;
    }
    if (bestMatch == RESOLVE_NO_MATCH) {
      bestMatch = newMatch;
      index = i;
    }
  }

  return bestMatch;
}
};

UnionParser::UnionParser(ResolverFactory &factory, const NodePtr &writer,
                         const NodePtr &reader, const CompoundLayout &offsets)
    : Resolver(),
      offset_(offsets.offset()),
      choiceOffset_(offsets.at(0).offset()),
      setFuncOffset_(offsets.at(1).offset()) {

  size_t leaves = writer->leaves();
  resolvers_.reserve(leaves);
  choiceMapping_.reserve(leaves);
  for (size_t i = 0; i < leaves; ++i) {

    // for each writer, we need a schema match for the reader
    const NodePtr &w = writer->leafAt(i);
    size_t index = 0;

    SchemaResolution match = checkUnionMatch(w, reader, index);

    if (match == RESOLVE_NO_MATCH) {
      resolvers_.push_back(factory.skipper(w));
      // push back a non-sensical number
      choiceMapping_.push_back(reader->leaves());
    } else {
      const NodePtr &r = reader->leafAt(index);
      resolvers_.push_back(factory.construct(w, r, offsets.at(index + 2)));
      choiceMapping_.push_back(index);
    }
  }
}

NonUnionToUnionParser::NonUnionToUnionParser(ResolverFactory &factory,
                                             const NodePtr &writer,
                                             const NodePtr &reader,
                                             const CompoundLayout &offsets)
    : Resolver(),
      offset_(offsets.offset()),
      choiceOffset_(offsets.at(0).offset()),
      setFuncOffset_(offsets.at(1).offset()) {
#ifndef NDEBUG
  SchemaResolution bestMatch =
#endif
      checkUnionMatch(writer, reader, choice_);
  assert(bestMatch != RESOLVE_NO_MATCH);
  resolver_.reset(factory.construct(writer, reader->leafAt(choice_),
                                    offsets.at(choice_ + 2)));
}

UnionToNonUnionParser::UnionToNonUnionParser(ResolverFactory &factory,
                                             const NodePtr &writer,
                                             const NodePtr &reader,
                                             const Layout &offsets)
    : Resolver() {
  size_t leaves = writer->leaves();
  resolvers_.reserve(leaves);
  for (size_t i = 0; i < leaves; ++i) {
    const NodePtr &w = writer->leafAt(i);
    resolvers_.push_back(factory.construct(w, reader, offsets));
  }
}

Resolver *constructResolver(const ValidSchema &writerSchema,
                            const ValidSchema &readerSchema,
                            const Layout &readerLayout) {
  ResolverFactory factory;
  return factory.construct(writerSchema.root(), readerSchema.root(),
                           readerLayout);
}

}  // namespace internal_avro
