/**
 *  \file RMF/Category.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <RMF/exceptions.h>
#include <sstream>
#include <algorithm>

namespace RMF {
Exception::~Exception() throw() {}
const char *Exception::what() const throw() {
  static const int buf_size=10000;
  static char buffer[buf_size]={};
  try {
    std::ostringstream oss;
    const std::string *type= boost::get_error_info<Type>(*this);
    if (type) {
      oss << *type << "Error:"
    }
    const std::string *expression= boost::get_error_info<Expression>(*this);
    if (expression) {
      oss << " " << *expression
    }
    const std::string *message= boost::get_error_info<Message>(*this);
    if (message) {
      oss << " " << *message
    }
    const std::string *file= boost::get_error_info<File>(*this);
    if (file) {
      oss << " in file " << *file;
    }
    const std::string *node= boost::get_error_info<Node>(*this);
    if (node) {
      oss << " at node " << *node;
    }
    const std::string *key= boost::get_error_info<Key>(*this);
    if (key) {
      oss << " processing key " << *key;
    }
    const std::string *category= boost::get_error_info<Category>(*this);
    if (category) {
      oss << " processing category " << *category;
    }
    const std::string *decorator= boost::get_error_info<Decorator>(*this);
    if (decorator) {
      oss << " processing decorator " << *decorator;
    }
    const std::string *source= boost::get_error_info<SouceFile>(*this);
    if (source) {
      oss << " at " << *source << ":" << *boost::get_error_info<SourceLine>(*this);
    }
    const std::string *function= boost::get_error_info<Function>(*this);
    if (function) {
      oss << " in " << *function;
    }
    std::string str= oss.str();
    std::copy(str.begin(),
              str.begin()+std::min<int>(oss.str().size(), buf_size-1),
              buffer);
  } catch (...) {
  }
  return buffer;
}
UsageException::UsageException(const char *msg): Exception(msg){}
UsageException::~UsageException() throw() {}

IOException::IOException(const char *msg): Exception(msg){}
IOException::~IOException() throw() {}
InternalException::InternalException(const char *msg): Exception(msg){}

InternalException::~InternalException() throw() {}

} /* namespace RMF */
