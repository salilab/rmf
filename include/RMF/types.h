/**
 *  \file RMF/types.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_TYPES_H
#define RMF_TYPES_H

#include "RMF/config.h"
#include "ID.h"
#include "Vector.h"
#include "infrastructure_macros.h"
#include "internal/errors.h"
#include "RMF/HDF5/types.h"

#include <algorithm>
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cmath>
#include <cstdlib>
#include <limits>

RMF_ENABLE_WARNINGS

namespace RMF {

/** The type used to store integral values.*/
typedef int Int;
/** The type used to store lists of integral values.*/
typedef std::vector<Int> Ints;
/** The type used to store lists of floating point values.*/
typedef float Float;
/** The type used to store lists of floating point values.*/
typedef std::vector<Float> Floats;
/** The type used to store lists of floating point values.*/
typedef std::vector<Floats> FloatsList;
/** The type used to store lists of index values.*/
typedef int Index;
/** The type used to store lists of index values.*/
typedef std::vector<Index> Indexes;
/** The type used to store lists of indexes.*/
typedef std::vector<Indexes> IndexesList;
/** The type used to store lists of string values.*/
typedef std::string String;
/** The type used to store lists of string values.*/
typedef std::vector<String> Strings;
/** The type used to store lists of strings values.*/
typedef std::vector<Strings> StringsList;
/** The type used to store lists of NodeID values.*/
typedef std::vector<NodeIDs> NodeIDsList;
/** The type used to store lists of lists of integers values.*/
typedef std::vector<Ints> IntsList;
/** The type used to store char values.*/
typedef char Char;
/** The type used to store lists of char values.*/
typedef std::string Chars;

typedef std::pair<int, int> IntRange;

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_TYPES_H */
