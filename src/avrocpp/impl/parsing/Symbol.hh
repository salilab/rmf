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

#ifndef avro_parsing_Symbol_hh__
#define avro_parsing_Symbol_hh__

#include <vector>
#include <map>
#include <stack>
#include <sstream>

#include <boost/any.hpp>
#include <memory>
#include <boost/tuple/tuple.hpp>

#include "Node.hh"
#include "Decoder.hh"
#include "Exception.hh"

namespace internal_avro {
namespace parsing {

class Symbol;

typedef std::vector<Symbol> Production;
typedef boost::tuple<size_t, bool, Production, Production> RepeaterInfo;
typedef boost::tuple<Production, Production> RootInfo;

class Symbol {
 public:
  enum Kind {
    sTerminalLow,  // extra has nothing
    sNull,
    sBool,
    sInt,
    sLong,
    sFloat,
    sDouble,
    sString,
    sBytes,
    sArrayStart,
    sArrayEnd,
    sMapStart,
    sMapEnd,
    sFixed,
    sEnum,
    sUnion,
    sTerminalHigh,
    sSizeCheck,    // Extra has size
    sNameList,     // Extra has a vector<string>
    sRoot,         // Root for a schema, extra is Symbol
    sRepeater,     // Array or Map, extra is symbol
    sAlternative,  // One of many (union), extra is Union
    sPlaceholder,  // To be fixed up later.
    sIndirect,     // extra is shared_ptr<Production>
    sSymbolic,     // extra is weal_ptr<Production>
    sEnumAdjust,
    sUnionAdjust,
    sSkipStart,
    sResolve,
    sImplicitActionLow,
    sRecordStart,
    sRecordEnd,
    sField,  // extra is string
    sRecord,
    sSizeList,
    sWriterUnion,
    sImplicitActionHigh,
    sError
  };

 private:
  Kind kind_;
  boost::any extra_;

  explicit Symbol(Kind k) : kind_(k) {}
  template <typename T>
  Symbol(Kind k, T t)
      : kind_(k), extra_(t) {}

 public:
  Kind kind() const { return kind_; }

  template <typename T>
  T extra() const {
    return boost::any_cast<T>(extra_);
  }

  template <typename T>
  T* extrap() {
    return boost::any_cast<T>(&extra_);
  }

  template <typename T>
  const T* extrap() const {
    return boost::any_cast<T>(&extra_);
  }

  template <typename T>
  void extra(const T& t) {
    extra_ = t;
  }

  bool isTerminal() const {
    return kind_ > sTerminalLow && kind_ < sTerminalHigh;
  }

  bool isImplicitAction() const {
    return kind_ > sImplicitActionLow && kind_ < sImplicitActionHigh;
  }

  static const char* stringValues[];
  static const char* toString(Kind k) { return stringValues[k]; }

  static Symbol rootSymbol(const Production& s) {
    return Symbol(Symbol::sRoot, RootInfo(s, Production()));
  }

  static Symbol rootSymbol(const Production& main, const Production& backup) {
    return Symbol(Symbol::sRoot, RootInfo(main, backup));
  }

  static Symbol nullSymbol() { return Symbol(sNull); }

  static Symbol boolSymbol() { return Symbol(sBool); }

  static Symbol intSymbol() { return Symbol(sInt); }

  static Symbol longSymbol() { return Symbol(sLong); }

  static Symbol floatSymbol() { return Symbol(sFloat); }

  static Symbol doubleSymbol() { return Symbol(sDouble); }

  static Symbol stringSymbol() { return Symbol(sString); }

  static Symbol bytesSymbol() { return Symbol(sBytes); }

  static Symbol sizeCheckSymbol(size_t s) { return Symbol(sSizeCheck, s); }

  static Symbol fixedSymbol() { return Symbol(sFixed); }

  static Symbol enumSymbol() { return Symbol(sEnum); }

  static Symbol arrayStartSymbol() { return Symbol(sArrayStart); }

  static Symbol arrayEndSymbol() { return Symbol(sArrayEnd); }

  static Symbol mapStartSymbol() { return Symbol(sMapStart); }

  static Symbol mapEndSymbol() { return Symbol(sMapEnd); }

  static Symbol repeater(const Production& p, bool isArray) {
    size_t s = 0;
    return Symbol(sRepeater, boost::make_tuple(s, isArray, p, p));
  }

  static Symbol repeater(const Production& read, const Production& skip,
                         bool isArray) {
    size_t s = 0;
    return Symbol(sRepeater, boost::make_tuple(s, isArray, read, skip));
  }

  static Symbol alternative(const std::vector<Production>& branches) {
    return Symbol(Symbol::sAlternative, branches);
  }

  static Symbol unionSymbol() { return Symbol(sUnion); }

  static Symbol recordStartSymbol() { return Symbol(sRecordStart); }

  static Symbol recordEndSymbol() { return Symbol(sRecordEnd); }

  static Symbol fieldSymbol(const std::string& name) {
    return Symbol(sField, name);
  }

  static Symbol writerUnionAction() { return Symbol(sWriterUnion); }

  static Symbol nameListSymbol(const std::vector<std::string>& v) {
    return Symbol(sNameList, v);
  }

  template <typename T>
  static Symbol placeholder(const T& n) {
    return Symbol(sPlaceholder, n);
  }

  static Symbol indirect(const std::shared_ptr<Production>& p) {
    return Symbol(sIndirect, p);
  }

  static Symbol symbolic(const std::weak_ptr<Production>& p) {
    return Symbol(sSymbolic, p);
  }

  static Symbol enumAdjustSymbol(const NodePtr& writer, const NodePtr& reader);

  static Symbol unionAdjustSymbol(size_t branch, const Production& p) {
    return Symbol(sUnionAdjust, std::make_pair(branch, p));
  }

  static Symbol sizeListAction(std::vector<size_t> order) {
    return Symbol(sSizeList, order);
  }

  static Symbol recordAction() { return Symbol(sRecord); }

  static Symbol error(const NodePtr& writer, const NodePtr& reader);

  static Symbol resolveSymbol(Kind w, Kind r) {
    return Symbol(sResolve, std::make_pair(w, r));
  }

  static Symbol skipStart() { return Symbol(sSkipStart); }
};

template <typename T>
void fixup(Production& p,
           const std::map<T, std::shared_ptr<Production> >& m) {
  for (Production::iterator it = p.begin(); it != p.end(); ++it) {
    fixup(*it, m);
  }
}

template <typename T>
void fixup(Symbol& s, const std::map<T, std::shared_ptr<Production> >& m) {
  switch (s.kind()) {
    case Symbol::sIndirect:
      fixup(*s.extra<std::shared_ptr<Production> >(), m);
      break;
    case Symbol::sAlternative: {
      std::vector<Production>* vv = s.extrap<std::vector<Production> >();
      for (std::vector<Production>::iterator it = vv->begin(); it != vv->end();
           ++it) {
        fixup(*it, m);
      }
    } break;
    case Symbol::sRepeater: {
      RepeaterInfo& ri = *s.extrap<RepeaterInfo>();
      fixup(boost::tuples::get<2>(ri), m);
      fixup(boost::tuples::get<3>(ri), m);
    } break;
    case Symbol::sPlaceholder:
      s = Symbol::symbolic(
          std::weak_ptr<Production>(m.find(s.extra<T>())->second));
      break;
    case Symbol::sUnionAdjust:
      fixup(s.extrap<std::pair<size_t, Production> >()->second, m);
      break;
    default:
      break;
  }
}

template <typename Handler>
class SimpleParser {
  Decoder* decoder_;
  Handler& handler_;
  std::stack<Symbol> parsingStack;

  static void throwMismatch(Symbol::Kind expected, Symbol::Kind actual) {
    std::ostringstream oss;
    oss << "Invalid operation. Expected: " << Symbol::toString(expected)
        << " got " << Symbol::toString(actual);
    throw Exception(oss.str());
  }

  static void assertMatch(Symbol::Kind expected, Symbol::Kind actual) {
    if (expected != actual) {
      throwMismatch(expected, actual);
    }
  }

  void append(const Production& ss) {
    for (Production::const_iterator it = ss.begin(); it != ss.end(); ++it) {
      parsingStack.push(*it);
    }
  }

  size_t popSize() {
    const Symbol& s = parsingStack.top();
    assertMatch(Symbol::sSizeCheck, s.kind());
    size_t result = s.extra<size_t>();
    parsingStack.pop();
    return result;
  }

  static void assertLessThan(size_t n, size_t s) {
    if (n >= s) {
      std::ostringstream oss;
      oss << "Size max value. Upper bound: " << s << " found " << n;
      throw Exception(oss.str());
    }
  }

 public:
  Symbol::Kind advance(Symbol::Kind k) {
    for (;;) {
      Symbol& s = parsingStack.top();
      if (s.kind() == k) {
        parsingStack.pop();
        return k;
      } else if (s.isTerminal()) {
        throwMismatch(k, s.kind());
      } else {
        switch (s.kind()) {
          case Symbol::sRoot:
            append(boost::tuples::get<0>(*s.extrap<RootInfo>()));
            continue;
          case Symbol::sIndirect: {
            std::shared_ptr<Production> pp =
                s.extra<std::shared_ptr<Production> >();
            parsingStack.pop();
            append(*pp);
          }
            continue;
          case Symbol::sSymbolic: {
            std::shared_ptr<Production> pp(
                s.extra<std::weak_ptr<Production> >());
            parsingStack.pop();
            append(*pp);
          }
            continue;
          case Symbol::sRepeater: {
            RepeaterInfo* p = s.extrap<RepeaterInfo>();
            --boost::tuples::get<0>(*p);
            append(boost::tuples::get<2>(*p));
          }
            continue;
          case Symbol::sError:
            throw Exception(s.extra<std::string>());
          case Symbol::sResolve: {
            const std::pair<Symbol::Kind, Symbol::Kind>* p =
                s.extrap<std::pair<Symbol::Kind, Symbol::Kind> >();
            assertMatch(p->second, k);
            Symbol::Kind result = p->first;
            parsingStack.pop();
            return result;
          }
          case Symbol::sSkipStart:
            parsingStack.pop();
            skip(*decoder_);
            break;
          default:
            if (s.isImplicitAction()) {
              Symbol ss = s;
              parsingStack.pop();
              size_t n = handler_.handle(ss);
              if (ss.kind() == Symbol::sWriterUnion) {
                selectBranch(n);
              }
            } else {
              std::ostringstream oss;
              oss << "Encountered " << Symbol::toString(s.kind())
                  << " while looking for " << Symbol::toString(k);
              throw Exception(oss.str());
            }
        }
      }
    }
  }

  void skip(Decoder& d) {
    const size_t sz = parsingStack.size();
    if (sz == 0) {
      throw Exception("Nothing to skip!");
    }
    while (parsingStack.size() >= sz) {
      Symbol& t = parsingStack.top();
      switch (t.kind()) {
        case Symbol::sNull:
          d.decodeNull();
          break;
        case Symbol::sBool:
          d.decodeBool();
          break;
        case Symbol::sInt:
          d.decodeInt();
          break;
        case Symbol::sLong:
          d.decodeLong();
          break;
        case Symbol::sFloat:
          d.decodeFloat();
          break;
        case Symbol::sDouble:
          d.decodeDouble();
          break;
        case Symbol::sString:
          d.skipString();
          break;
        case Symbol::sBytes:
          d.skipBytes();
          break;
        case Symbol::sArrayStart: {
          parsingStack.pop();
          size_t n = d.skipArray();
          assertMatch(Symbol::sRepeater, parsingStack.top().kind());
          if (n == 0) {
            break;
          }
          Symbol& t = parsingStack.top();
          RepeaterInfo* p = t.extrap<RepeaterInfo>();
          boost::tuples::get<0>(*p) = n;
          continue;
        }
        case Symbol::sArrayEnd:
          break;
        case Symbol::sMapStart: {
          parsingStack.pop();
          size_t n = d.skipMap();
          if (n == 0) {
            break;
          }
          assertMatch(Symbol::sRepeater, parsingStack.top().kind());
          Symbol& t = parsingStack.top();
          RepeaterInfo* p = t.extrap<RepeaterInfo>();
          boost::tuples::get<0>(*p) = n;
          continue;
        }
        case Symbol::sMapEnd:
          break;
        case Symbol::sFixed: {
          parsingStack.pop();
          Symbol& t = parsingStack.top();
          d.decodeFixed(t.extra<size_t>());
        } break;
        case Symbol::sEnum:
          parsingStack.pop();
          d.decodeEnum();
          break;
        case Symbol::sUnion: {
          parsingStack.pop();
          size_t n = d.decodeUnionIndex();
          selectBranch(n);
          continue;
        }
        case Symbol::sRepeater: {
          RepeaterInfo* p = t.extrap<RepeaterInfo>();
          if (boost::tuples::get<0>(*p) == 0) {
            boost::tuples::get<0>(*p) =
                boost::tuples::get<1>(*p) ? d.arrayNext() : d.mapNext();
          }
          if (boost::tuples::get<0>(*p) != 0) {
            --boost::tuples::get<0>(*p);
            append(boost::tuples::get<3>(*p));
            continue;
          }
        } break;
        case Symbol::sIndirect: {
          std::shared_ptr<Production> pp =
              t.extra<std::shared_ptr<Production> >();
          parsingStack.pop();
          append(*pp);
        }
          continue;
        case Symbol::sSymbolic: {
          std::shared_ptr<Production> pp(
              t.extra<std::weak_ptr<Production> >());
          parsingStack.pop();
          append(*pp);
        }
          continue;
        default: {
          std::ostringstream oss;
          oss << "Don't know how to skip " << Symbol::toString(t.kind());
          throw Exception(oss.str());
        }
      }
      parsingStack.pop();
    }
  }

  void assertSize(size_t n) {
    size_t s = popSize();
    if (s != n) {
      std::ostringstream oss;
      oss << "Incorrect size. Expected: " << s << " found " << n;
      throw Exception(oss.str());
    }
  }

  void assertLessThanSize(size_t n) { assertLessThan(n, popSize()); }

  size_t enumAdjust(size_t n) {
    const Symbol& s = parsingStack.top();
    assertMatch(Symbol::sEnumAdjust, s.kind());
    const std::pair<std::vector<int>, std::vector<std::string> >* v =
        s.extrap<std::pair<std::vector<int>, std::vector<std::string> > >();
    assertLessThan(n, v->first.size());

    int result = v->first[n];
    if (result < 0) {
      std::ostringstream oss;
      oss << "Cannot resolve symbol: " << v->second[-result - 1] << std::endl;
      throw Exception(oss.str());
    }
    parsingStack.pop();
    return result;
  }

  size_t unionAdjust() {
    const Symbol& s = parsingStack.top();
    assertMatch(Symbol::sUnionAdjust, s.kind());
    std::pair<size_t, Production> p = s.extra<std::pair<size_t, Production> >();
    parsingStack.pop();
    append(p.second);
    return p.first;
  }

  std::string nameForIndex(size_t e) {
    const Symbol& s = parsingStack.top();
    assertMatch(Symbol::sNameList, s.kind());
    const std::vector<std::string> names = s.extra<std::vector<std::string> >();
    if (e >= names.size()) {
      throw Exception("Not that many names");
    }
    std::string result = names[e];
    parsingStack.pop();
    return result;
  }

  size_t indexForName(const std::string& name) {
    const Symbol& s = parsingStack.top();
    assertMatch(Symbol::sNameList, s.kind());
    const std::vector<std::string> names = s.extra<std::vector<std::string> >();
    std::vector<std::string>::const_iterator it =
        std::find(names.begin(), names.end(), name);
    if (it == names.end()) {
      throw Exception("No such enum symbol");
    }
    size_t result = it - names.begin();
    parsingStack.pop();
    return result;
  }

  void setRepeatCount(size_t n) {
    Symbol& s = parsingStack.top();
    assertMatch(Symbol::sRepeater, s.kind());
    size_t& nn = boost::tuples::get<0>(*s.extrap<RepeaterInfo>());
    if (nn != 0) {
      throw Exception("Wrong number of items");
    }
    nn = n;
  }

  void popRepeater() {
    processImplicitActions();
    const Symbol& s = parsingStack.top();
    assertMatch(Symbol::sRepeater, s.kind());
    if (boost::tuples::get<0>(*s.extrap<RepeaterInfo>()) != 0) {
      throw Exception("Incorrect number of items");
    }
    parsingStack.pop();
  }

  void selectBranch(size_t n) {
    const Symbol& s = parsingStack.top();
    assertMatch(Symbol::sAlternative, s.kind());
    std::vector<Production> v = s.extra<std::vector<Production> >();
    if (n >= v.size()) {
      throw Exception("Not that many branches");
    }
    parsingStack.pop();
    append(v[n]);
  }

  const std::vector<size_t>& sizeList() {
    const Symbol& s = parsingStack.top();
    assertMatch(Symbol::sSizeList, s.kind());
    return *s.extrap<std::vector<size_t> >();
  }

  Symbol::Kind top() const { return parsingStack.top().kind(); }

  void pop() { parsingStack.pop(); }

  void processImplicitActions() {
    for (;;) {
      Symbol& s = parsingStack.top();
      if (s.isImplicitAction()) {
        handler_.handle(s);
        parsingStack.pop();
      } else {
        break;
      }
    }
  }

  SimpleParser(const Symbol& s, Decoder* d, Handler& h)
      : decoder_(d), handler_(h) {
    parsingStack.push(s);
  }

  void reset() {
    while (parsingStack.size() > 1) {
      parsingStack.pop();
    }
  }
};

}  // namespace parsing
}  // namespace internal_avro

#endif
