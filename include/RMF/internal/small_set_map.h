#ifndef RMF_INTERNAL_SMALL_SET_MAP_H
#define RMF_INTERNAL_SMALL_SET_MAP_H

#include <RMF/config.h>
#include <boost/version.hpp>

#if BOOST_VERSION >= 104900
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#define RMF_SMALL_ORDERED_SET boost::container::flat_set
#define RMF_SMALL_ORDERED_MAP boost::container::flat_map
#define RMF_SMALL_UNORDERED_SET boost::container::flat_set
#define RMF_SMALL_UNORDERED_MAP boost::container::flat_map
#else
#include <set>
#include <map>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#define RMF_SMALL_ORDERED_SET std::set
#define RMF_SMALL_ORDERED_MAP std::map
#define RMF_SMALL_UNORDERED_SET boost::unordered_set
#define RMF_SMALL_UNORDERED_MAP boost::unordered_map
#endif

#endif
