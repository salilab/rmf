/**
 *  \file RMF/Log.h
 *  \brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_LOG_H
#define RMF_LOG_H

#include <RMF/config.h>

#if RMF_USE_LOG4CXX
#  include <log4cxx/logger.h>
#endif

namespace RMF {

#if RMF_USE_LOG4CXX
inline log4cxx::LoggerPtr get_logger() {
  static log4cxx::LoggerPtr ret = log4cxx::Logger::getLogger("RMF");
  return ret;
}
inline log4cxx::LoggerPtr get_avro_logger() {
  static log4cxx::LoggerPtr ret = log4cxx::Logger::getLogger("RMF.avro");
  return ret;
}
inline log4cxx::LoggerPtr get_hdf5_logger() {
  static log4cxx::LoggerPtr ret = log4cxx::Logger::getLogger("RMF.hdf5");
  return ret;
}
#endif


} /* namespace RMF */

#if RMF_USE_LOG4CXX

#  define RMF_TRACE(log, expr) LOG4CXX_TRACE(log, expr)
#  define RMF_DEBUG(log, expr) LOG4CXX_DEBUG(log, expr)
#  define RMF_INFO(log, expr) LOG4CXX_INFO(log, expr)
#  define RMF_WARN(log, expr) LOG4CXX_WARN(log, expr)
#  define RMF_ERROR(log, expr) LOG4CXX_ERROR(log, expr)
#  define RMF_FATAL(log, expr) LOG4CXX_FATAL(log, expr)

#else
#  define RMF_TRACE(log, expr)
#  define RMF_DEBUG(log, expr)
#  define RMF_INFO(log, expr)
#  define RMF_WARN(log, expr)
#  define RMF_ERROR(log, expr)
#  define RMF_FATAL(log, expr)

#endif

#endif /* RMF_LOG_H */
