/**
 *  \file RMF/types.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF__TYPES_H
#define RMF__TYPES_H

#include <RMF/config.h>
#include "NodeID.h"
#include "hdf5_handle.h"
#include "infrastructure_macros.h"
#include "internal/errors.h"
#include "internal/types.h"
#include <hdf5.h>
#include <algorithm>

#include <limits>


namespace RMF {

#ifdef RMF_DOXYGEN
  /** The RMF library uses containers that have the same interfaces as, but
      not necessarily the same type as, std::vector to store lists of
      values. (The reason the types are not necessarily the same is that,
      bounds checked vectors are used when they are available and the
      library is not build with \c NDEBUG).
  */
  template <class T>
  class vector {};
#endif

#ifndef RMF_DOXYGEN
  /** \defgroup hdf5 HDF Support
      These functions provide low level support for reading and writing HDF5
      files. As such, they aren't really part of the RMF library itself, but
      are needed and very useful.
  */

  /** \addtogroup hdf5
      @{
  */
  /** The type used to store integral values.*/
  typedef int Int;
  /** The type used to store lists of integral values.*/
  typedef vector<Int> Ints;
  /** The type used to store lists of floating point values.*/
  typedef double Float;
  /** The type used to store lists of floating point values.*/
  typedef vector<Float> Floats;
  /** The type used to store lists of floating point values.*/
  typedef vector<Floats> FloatsList;
  /** The type used to store lists of index values.*/
  typedef int Index;
  /** The type used to store lists of index values.*/
  typedef vector<Index> Indexes;
 /** The type used to store lists of indexes.*/
  typedef vector<Indexes> IndexesList;
  /** The type used to store lists of string values.*/
  typedef std::string String;
  /** The type used to store lists of string values.*/
  typedef vector<String> Strings;
  /** The type used to store lists of strings values.*/
  typedef vector<Strings> StringsList;
  /** The type used to store lists of NodeID values.*/
  typedef vector<NodeIDs> NodeIDsList;
  /** The type used to store lists of lists of integers values.*/
  typedef vector<Ints> IntsList;
  /** The type used to store char values.*/
  typedef char Char;
  /** The type used to store lists of char values.*/
  typedef std::string Chars;
  /** @} */

  typedef std::pair<Index, Index> IndexRange;
  typedef std::pair<Int, Int> IntRange;

  RMF_SIMPLE_TRAITS(Int, Ints, int, 0, H5T_STD_I64LE, H5T_NATIVE_INT,
                        H5T_NATIVE_INT, std::numeric_limits<int>::max());

  RMF_SIMPLE_TRAITS(Float, Floats, float, 1, H5T_IEEE_F64LE,
                        H5T_NATIVE_DOUBLE,
                        H5T_NATIVE_DOUBLE, std::numeric_limits<double>::max());

  RMF_SIMPLE_TRAITS(Index, Indexes, index, 2, H5T_STD_I64LE, H5T_NATIVE_INT,
                        H5T_NATIVE_INT, -1);

  RMF_TRAITS(String, Strings, string, 3, internal::get_string_type(),
                 internal::get_string_type(), internal::get_string_type(),
                 String(),{
                   char *c;
                   if (!v.empty()) {
                     c= const_cast<char*>(v.c_str());
                   } else {
                     static char empty='\0';
                     c=&empty;
                   }
                   RMF_HDF5_CALL(H5Dwrite(d, get_hdf5_memory_type(), is, s,
                                          H5P_DEFAULT, &c));
                 }
                 ,{
                   char *c=NULL;
                   RMF_HDF5_HANDLE( mt,internal::create_string_type(),
                                    H5Tclose);
                   RMF_HDF5_CALL(H5Dread(d,  mt, is, sp, H5P_DEFAULT, &c));
                   if (c) {
                     ret=std::string(c);
                   }
                   free(c);
                 }
                 ,{
                   RMF_UNUSED(d); RMF_UNUSED(is); RMF_UNUSED(s);
                   RMF_NOT_IMPLEMENTED;
                 }
                 ,{
                   RMF_UNUSED(d); RMF_UNUSED(is);
                   RMF_UNUSED(sp); RMF_UNUSED(sz);
                   RMF_NOT_IMPLEMENTED;
                 }
                 ,{
                   RMF_UNUSED(a); RMF_UNUSED(v);
                   RMF_NOT_IMPLEMENTED;
                 }
                 ,{
                   RMF_UNUSED(a); RMF_UNUSED(sz);
                   RMF_NOT_IMPLEMENTED;
             }, false);

  RMF_TRAITS(NodeID, NodeIDs, node_id, 4, IndexTraits::get_hdf5_disk_type(),
                 IndexTraits::get_hdf5_memory_type(),
                 IndexTraits::get_hdf5_fill_type(), NodeID(),
                 IndexTraits::write_value_dataset(d, is, s, v.get_index())
                 ,{
                   int i= IndexTraits::read_value_dataset(d, is, sp);
                   if (i>=0) ret= NodeID(i);
                 }
                 ,{
                   Ints vi(v.size());
                   for (unsigned int i=0; i< v.size(); ++i) {
                     vi[i]= v[i].get_index();
                   }
                   IntTraits::write_values_dataset(d, is, s, vi);
                 }
                 ,{
                     Ints reti= IndexTraits::read_values_dataset(d, is, sp, sz);
                     for (unsigned int i=0; i< ret.size(); ++i) {
                       ret[i]= NodeID(reti[i]);
                     }
                 }
                 ,{
                     Ints is(v.size());
                     for (unsigned int i=0; i< v.size(); ++i) {
                       is[i]=v[i].get_index();
                     }
                     IndexTraits::write_values_attribute(a, is);
                 }
                 ,{
                   Ints is= IndexTraits::read_values_attribute(a, sz);
                   for (unsigned int i=0; i< ret.size(); ++i) {
                     ret[i]=NodeID(is[i]);
                   }
             }, true);

  RMF_TRAITS_ONE(Char, Chars, char, 6, H5T_STD_I8LE,
                 H5T_NATIVE_CHAR,H5T_NATIVE_CHAR, '\0',
                 {
                   RMF_UNUSED(d); RMF_UNUSED(is); RMF_UNUSED(s);
                   RMF_UNUSED(v);
                   RMF_NOT_IMPLEMENTED;
                 },{
                   RMF_UNUSED(d); RMF_UNUSED(is); RMF_UNUSED(sp);
                   RMF_NOT_IMPLEMENTED;
                   ret='\0';
                 }
                 ,{
                   RMF_UNUSED(d); RMF_UNUSED(is); RMF_UNUSED(s);
                   RMF_NOT_IMPLEMENTED;
                 }
                 ,{
                   RMF_UNUSED(d); RMF_UNUSED(is);
                   RMF_UNUSED(sp); RMF_UNUSED(sz);
                   RMF_NOT_IMPLEMENTED;
                 }
                 ,{
                   RMF_HDF5_CALL(H5Awrite(a, H5T_NATIVE_CHAR, v.c_str()));
                 }
                 ,{
                   vector<char> v(sz);
                   RMF_HDF5_CALL(H5Aread(a, H5T_NATIVE_CHAR, &v[0]));
                   ret=std::string(&v[0], v.size());
                 }, false);
#endif

} /* namespace RMF */

#endif /* RMF__TYPES_H */
