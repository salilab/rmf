#!/usr/bin/env python

import os.path


def replace(msg, to_replace, const):
    for k in to_replace:
        msg = msg.replace(k[0], k[1])
    if const:
        msg = msg.replace("NONCONST", "")
        msg = msg.replace("CONST", "Const")
    else:
        msg = msg.replace("NONCONST", "Const")
        msg = msg.replace("CONST", "")
    return msg


class Base:

    def __init__(self, name, data_type, return_type):
        self.names = [("NAME", name.replace(" ", "_")),
                      ("DATA", data_type)]
        self.names.extend([("GET_BOTH", "get_value"),
                           ("SET_BOTH", "set_value"),
                           ("GET_STATIC", "get_static_value"),
                           ("SET_STATIC", "set_static_value"),
                           ("GET_FRAME", "get_frame_value"),
                           ("SET_FRAME", "set_frame_value")])
        if return_type.endswith("s"):
            self.names.append(("TYPES", return_type + "List"))
        elif return_type.endswith("x"):
            self.names.append(("TYPES", return_type + "es"))
        else:
            self.names.append(("TYPES", return_type + "s"))
        self.names.append(("TYPE", return_type))
        self.get_methods = ""
        self.set_methods = ""
        self.bulk_get_methods = ""
        self.bulk_set_methods = ""
        self.helpers = ""
        self.check = ""

    def get_data_members(self, const):
        return replace("DATA NAME_;", self.names, const)

    def get_get_set_methods(self, const):
        ret = replace(self.get_methods, self.names, const)
        if not const:
            ret += replace(self.set_methods, self.names, const)
        return ret

    def get_bulk_methods(self, const):
        ret = replace(self.bulk_get_methods, self.names, const)
        if not const:
            ret += replace(self.bulk_set_methods, self.names, const)
        return ret

    def get_helpers(self, const):
        return replace(self.helpers, self.names, const)

    def get_data_arguments(self, const):
        return replace("DATA NAME", self.names, const)

    def get_data_pass(self, const):
        return replace("NAME_", self.names, const)

    def get_data_saves(self, const):
        return replace("NAME_(NAME)", self.names, const)

    def get_data_initialize(self, const):
        return (
            replace("NAME_(" + self.data_initialize + ")", self.names, const)
        )

    def get_check(self, const):
        return replace(self.check, self.names, const)


class Attribute(Base):

    def __init__(
        self,
        name,
        attribute_type,
            function_name=None):
        if not function_name:
            function_name = name.replace(" ", "_")
        Base.__init__(self, name, attribute_type +
                      "Key", attribute_type)
        self.get_methods = """
  TYPE get_%s() const {
    try {
      return P::GET_BOTH(NAME_);
    } RMF_DECORATOR_CATCH( );
  }
  TYPE get_frame_%s() const {
    try {
      return P::GET_FRAME(NAME_);
    } RMF_DECORATOR_CATCH( );
  }
  TYPE get_static_%s() const {
    try {
      return P::GET_STATIC(NAME_);
    } RMF_DECORATOR_CATCH( );
  }
""" % (function_name, function_name, function_name)
        self.set_methods = """
  void set_%s(TYPE v) {
    try {
      P::SET_BOTH(NAME_, v);
    } RMF_DECORATOR_CATCH( );
  }
  void set_frame_%s(TYPE v) {
    try {
      P::SET_FRAME(NAME_, v);
    } RMF_DECORATOR_CATCH( );
  }
  void set_static_%s(TYPE v) {
    try {
      P::SET_STATIC(NAME_, v);
    } RMF_DECORATOR_CATCH( );
  }
""" % (function_name, function_name, function_name)
        self.check = "!nh.GET(NAME_).get_is_null()"
        self.data_initialize = "fh.get_key<TYPETraits>(cat_, \"%s\")" % name


class NodeAttribute(Attribute):

    def __init__(self, name):
        Attribute.__init__(self, name, "Int", True)
        self.get_methods = """
  NodeCONSTHandle get_NAME() const {
    try {
      int id = get_node().GET_BOTH(NAME_);
      return get_node().get_file().get_node(NodeID(id));
    } RMF_DECORATOR_CATCH( );
  }
"""
        self.set_methods = """
  void set_NAME(NodeConstHandle v) {
    try {
      get_node().SET_BOTH(NAME_, v.get_index().get_index());
    } RMF_DECORATOR_CATCH( );
  }
"""


class PathAttribute(Attribute):

    def __init__(self, name):
        Attribute.__init__(self, name, "String")
        self.get_methods = """
  String get_NAME() const {
    try {
      String relpath = get_node().GET_BOTH(NAME_);
      String filename = get_node().get_file().get_path();
      return internal::get_absolute_path(filename, relpath);
    } RMF_DECORATOR_CATCH( );
  }
"""
        self.set_methods = """
  void set_NAME(String path) {
   try {
     String filename = get_node().get_file().get_path();
     String relpath = internal::get_relative_path(filename, path);
     get_node().SET_BOTH(NAME_, relpath);
   } RMF_DECORATOR_CATCH( );
  }
"""


class AttributePair(Base):

    def __init__(
            self,
            name,
        data_type,
        return_type,
        begin,
            end):
        Base.__init__(self, name, "boost::array<%sKey, 2>" %
                      data_type, return_type)
        self.helpers = """  DATA get_NAME_keys(FileCONSTHandle fh) {
     DATA ret;
     ret[0] = fh.get_key<%sTraits>(cat_, "%s");
     ret[1] = fh.get_key<%sTraits>(cat_, "%s");
     return ret;
    }
""" % (data_type, begin, data_type, end)
        self.check = "!nh.GET(NAME_[0]).get_is_null() && !nh.GET(NAME_[1]).get_is_null()"
        self.data_initialize = "get_NAME_keys(fh)"


class SingletonRangeAttribute(AttributePair):

    def __init__(self, name, data_type, begin, end):
        AttributePair.__init__(
            self, name, data_type, data_type, begin, end)
        self.get_methods = """
  TYPE get_NAME() const {
    try {
      return get_node().GET_BOTH(NAME_[0]);
    } RMF_DECORATOR_CATCH( );
  }
  TYPE get_frame_NAME() const {
    try {
      return get_node().GET_FRAME(NAME_[0]);
    } RMF_DECORATOR_CATCH( );
  }
  TYPE get_static_NAME() const {
    try {
      return get_node().GET_STATIC(NAME_[0]);
    } RMF_DECORATOR_CATCH( );
  }
"""
        self.set_methods = """
  void set_NAME(TYPE v) {
    try {
      get_node().SET_BOTH(NAME_[0], v);
      get_node().SET_BOTH(NAME_[1], v);
    } RMF_DECORATOR_CATCH( );
  }
  void set_frame_NAME(TYPE v) {
    try {
      get_node().SET_FRAME(NAME_[0], v);
      get_node().SET_FRAME(NAME_[1], v);
    } RMF_DECORATOR_CATCH( );
  }
  void set_static_NAME(TYPE v) {
    try {
      get_node().SET_STATIC(NAME_[0], v);
      get_node().SET_STATIC(NAME_[1], v);
    } RMF_DECORATOR_CATCH( );
  }
"""
        self.check = "!nh.GET(NAME_[0]).get_is_null() && !nh.GET(NAME_[1]).get_is_null() && nh.GET_BOTH(NAME_[0]) == nh.GET_BOTH(NAME_[1])"


class RangeAttribute(AttributePair):

    def __init__(self, name, data_type, begin, end):
        AttributePair.__init__(
            self, name, data_type, data_type + "Range", begin, end)
        self.get_methods = """  /** DOC */
  TYPE get_NAME() const {
    try {
      return std::make_pair(get_node().GET_BOTH(NAME_[0]), get_node().GET_BOTH(NAME_[1]));
    } RMF_DECORATOR_CATCH( );
  }
  TYPE get_static_NAME() const {
    try {
      return std::make_pair(get_node().GET_STATIC(NAME_[0]), get_node().GET_STATIC(NAME_[1]));
    } RMF_DECORATOR_CATCH( );
  }
  TYPE get_frame_NAME() const {
    try {
      return std::make_pair(get_node().GET_FRAME(NAME_[0]), get_node().GET_FRAME(NAME_[1]));
    } RMF_DECORATOR_CATCH( );
  }
"""
        self.set_methods = """
  void set_NAME(TYPE v) {
    try {
      get_node().SET_BOTH(NAME_[0], v.first);
      get_node().SET_BOTH(NAME_[1], v.second);
    } RMF_DECORATOR_CATCH( );
  }
  void set_frame_NAME(TYPE v) {
    try {
      get_node().SET_FRAME(NAME_[0], v.first);
      get_node().SET_FRAME(NAME_[1], v.second);
    } RMF_DECORATOR_CATCH( );
  }
    void set_static_NAME(TYPE v) {
    try {
      get_node().SET_STATIC(NAME_[0], v.first);
      get_node().SET_STATIC(NAME_[1], v.second);
    } RMF_DECORATOR_CATCH( );
  }
"""
        self.check = "!nh.GET(NAME_[0]).get_is_null() && !nh.GET(NAME_[1]).get_is_null() && nh.GET_BOTH(NAME_[0]) < nh.GET_BOTH(NAME_[1])"

        self.check = "!nh.GET(NAME_[0]).get_is_null() && !nh.GET(NAME_[1]).get_is_null() && nh.GET_BOTH(NAME_[0]) < nh.GET_BOTH(NAME_[1])"


decorator = """
  /** DESCRIPTION

       See also NAMENONCONST and NAMECONSTFactory.
    */
  class NAMECONST: public Decorator<NodeCONSTHandle> {
    friend class NAMECONSTFactory;
    std::string get_name() const {return \"NAME\";}
  private:
    typedef Decorator<NodeCONSTHandle> P;
DATA_MEMBERS
    NAMECONST(NodeCONSTHandle nh,
              DATA_ARGUMENTS):
       DATA_SAVES {
    }
  public:
METHODS
    static std::string get_decorator_type_name() {
         return "NAMECONST";
    }
  };

    typedef std::vector<NAMECONST> NAMECONSTs;
"""


factory = """
  /** Create decorators of type NAME.

       See also NAMECONST and NAMENONCONSTFactory.
    */
  class NAMECONSTFactory: public Factory<FileCONSTHandle> {
    typedef Factory<FileCONSTHandle> P;
    Category cat_;
DATA_MEMBERS
HELPERS
  public:
    NAMECONSTFactory(FileCONSTHandle fh):
    P(),
    cat_(fh.get_category("CATEGORY")),
    DATA_INITIALIZE {
    }
    /** Get a NAMECONST for nh.*/
    NAMECONST get(NodeCONSTHandle nh) const {
      CREATE_CHECKS
      return NAMECONST(nh, DATA_PASS);
    }
    /** Check whether nh has all the attributes required to be a
        NAMECONST.*/
    bool get_is(NodeCONSTHandle nh) const {
      return FRAME_CHECKS;
    }
    bool get_is_static(NodeCONSTHandle nh) const {
      return STATIC_CHECKS;
    }
    BULK_METHODS
  };

  typedef std::vector<NAMECONSTFactory> NAMECONSTFactories;
"""


class Decorator:

    def __init__(self, allowed_types, category, name,
                 attributes,
                 init_function="",
                 check_function=""):
        self.name = name
        self.category = category
        self.allowed_types = allowed_types
        self.init_function = init_function
        self.attributes = attributes
        self.check_function = check_function

    def _get_data_members(self, const):
        ret = []
        for a in self.attributes:
            ret.append(a.get_data_members(const))
        return "\n".join(ret)

    def _get_methods(self, const):
        ret = []
        for a in self.attributes:
            ret.append(a.get_get_set_methods(const))
        return "\n".join(ret)

    def _get_bulk_methods(self, const):
        ret = []
        for a in self.attributes:
            ret.append(a.get_bulk_methods(const))
        return "\n".join(ret)

    def _get_helpers(self, const):
        ret = []
        for a in self.attributes:
            ret.append(a.get_helpers(const))
        return "\n".join(ret)

    def _get_data_arguments(self, const):
        ret = []
        for a in self.attributes:
            ret.append(a.get_data_arguments(const))
        return ",\n".join(ret)

    def _get_data_pass(self, const):
        ret = []
        for a in self.attributes:
            ret.append(a.get_data_pass(const))
        return ",\n".join(ret)

    def _get_data_saves(self, const):
        ret = []
        for a in self.attributes:
            ret.append(a.get_data_saves(const))
        return ",\n".join(["P(nh)"] + ret)

    def _get_type_check(self):
        cret = []
        for t in self.allowed_types:
            cret.append("nh.get_type() == RMF::%s" % t)
        return "(" + "||".join(cret) + ")"

    def _get_checks(self, const):
        ret = [self._get_type_check()]
        for a in self.attributes:
            ret.append(a.get_check(const))
        ret.append(self.check_function)
        return "\n    && ".join(x for x in ret if x != "")

    def _get_construct(self, const):
        ret = []
        # make handle missing later
        ret.append("Category cat = fh.get_category(\""
                   + self.category + "\");")
        ret.append("RMF_UNUSED(cat);")
        for a in self.attributes:
            ret.append(a.get_construct(const))
        return "\n".join(ret)

    def _get_data_initialize(self, const):
        ret = []
        for a in self.attributes:
            ret.append(a.get_data_initialize(const))
        return ", ".join(ret)

    def _get_list(self, const):
        ret = [("HELPERS", self._get_helpers(const)),
               ("DATA_MEMBERS", self._get_data_members(const)),
               ("BULK_METHODS", self._get_bulk_methods(const)),
               ("METHODS", self._get_methods(const)),
               ("DATA_ARGUMENTS", self._get_data_arguments(const)),
               ("DATA_SAVES", self._get_data_saves(const)),
               ("DATA_PASS", self._get_data_pass(const)),
               ("DATA_INITIALIZE", self._get_data_initialize(const))]
        if const:
            ret.append(("CREATE_CHECKS", """RMF_USAGE_CHECK(%s, std::string("Bad node type. Got \\\"")
                                      + boost::lexical_cast<std::string>(nh.get_type())
                                      + "\\\" in decorator type  %s");""" % (self._get_type_check(),
                                                                             self.name)))
        else:
            ret.append(("CREATE_CHECKS", """RMF_USAGE_CHECK(%s, std::string("Bad node type. Got \\\"")
                                      + boost::lexical_cast<std::string>(nh.get_type())
                                      + "\\\" in decorator type  %s");
                                      %s
                                      """ % (self._get_type_check(), self.name,
                                             self.init_function)))

        ret.append(
            ("FRAME_CHECKS", self._get_checks(const)
             .replace("GET", "get_value")))
        ret.append(
            ("STATIC_CHECKS", self._get_checks(const)
             .replace("GET", "get_static_value")))
        return ret

    def get(self):
        ret = ""
        common = [("NAME", self.name),
                  ("CATEGORY", self.category)]
        ret += replace(decorator, common + self._get_list(True), True)
        ret += replace(decorator, common + self._get_list(False), False)
        ret += replace(factory, common + self._get_list(True), True)
        ret += replace(factory, common + self._get_list(False), False)
        return ret


def make_header(name, infos, deps):
    path = os.path.join("include", "RMF", "decorator")
    if not os.path.exists(path):
        os.makedirs(path)

    fl = open(os.path.join("include", "RMF", "decorator", name + ".h"), "w")
    print >> fl, """/**
 *  \\file RMF/decorator/%(name)s.h
 *  \\brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_%(NAME)s_DECORATORS_H
#define RMF_%(NAME)s_DECORATORS_H

#include <RMF/config.h>
#include <RMF/infrastructure_macros.h>
#include <RMF/NodeHandle.h>
#include <RMF/FileHandle.h>
#include <RMF/Decorator.h>
#include <RMF/Factory.h>
#include <RMF/constants.h>
#include <RMF/Vector.h>
#include <RMF/internal/utility.h>
#include <RMF/internal/paths.h>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>""" % {"name": name, "NAME": name.upper()}
    for d in deps:
        print >> fl, """#include "%s.h" """ % d
    print >> fl, """
RMF_ENABLE_WARNINGS
namespace RMF {
namespace decorator {
""" % {"name": name, "NAME": name.upper()}
    for i in infos:
        print >> fl, i.get()
    print >> fl, """
} /* namespace decorator */
} /* namespace RMF */
RMF_DISABLE_WARNINGS

#endif /* RMF_%(NAME)s_DECORATORS_H */""" % {"name": name, "NAME": name.upper()}
