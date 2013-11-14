#ifndef RMF_INTERNAL_SMALL_SET_MAP_H
#define RMF_INTERNAL_SMALL_SET_MAP_H

#include <RMF/config.h>
#include <boost/version.hpp>

#if BOOST_VERSION >= 104900
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#define RMF_SMALL_ORDERED_SET(k) boost::container::flat_set<k>
#define RMF_SMALL_ORDERED_MAP(k,v) boost::container::flat_map<k, v>
#define RMF_SMALL_UNORDERED_SET(k) boost::container::flat_set<k>
#define RMF_SMALL_UNORDERED_MAP(k,v) boost::container::flat_map<k, v>
#else
#include <set>
#include <map>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#define RMF_SMALL_ORDERED_SET(k) std::set<k>
#define RMF_SMALL_ORDERED_MAP(k, v) std::map<k,v>
#define RMF_SMALL_UNORDERED_SET(k) boost::unordered_set<k>
#define RMF_SMALL_UNORDERED_MAP(k, v) boost::unordered_map<k,v>
#endif

#endif
