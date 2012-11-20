/**
 *  \file RMF/HDF5DataSetIndexD.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_HDF_5DATA_SET_INDEX_D_H
#define RMF_HDF_5DATA_SET_INDEX_D_H

#include <RMF/config.h>
#include "types.h"
#include <boost/functional/hash.hpp>
#include <stdexcept>

namespace RMF {

  /** Store an index into a data set. Typedefs are provides
      for 1,2 and 3 dimension indexes, name like
      HDF5DataSetIndex2D.
   */
  template <int D>
  class HDF5DataSetIndexD
  {
    hsize_t d_[D];
    int compare(const HDF5DataSetIndexD<D> &o) const {
      for (unsigned int i=0; i< D; ++i) {
        if (d_[i] < o.d_[i]) return -1;
        else if (d_[i] > o.d_[i]) return 1;
      }
      return 0;
    }
  public:
    HDF5DataSetIndexD(const Ints &o) {
      RMF_USAGE_CHECK(o.size()==D, "Wrong number of values");
      std::copy(o.begin(), o.end(), d_);
    }
    HDF5DataSetIndexD() {
      std::fill(d_, d_+D, -1);
    }
    HDF5DataSetIndexD(unsigned int i) {
      RMF_USAGE_CHECK(D==1, "Constructor does not match dimension.");
      d_[0]=i;
    }
    HDF5DataSetIndexD(unsigned int i, unsigned int j) {
      RMF_USAGE_CHECK(D==2, "Constructor does not match dimension.");
      d_[0]=i;
      if (D>1) d_[1]=j;
    }
    HDF5DataSetIndexD(unsigned int i, unsigned int j, unsigned int k) {
      RMF_USAGE_CHECK(D==3, "Constructor does not match dimension.");
      d_[0]=i;
      // for clang
      if (D>1) d_[1]=j;
      if (D>2) d_[2]=k;
    }
#ifndef SWIG
    hsize_t& operator[](unsigned int i) {
      RMF_USAGE_CHECK(i < D,
                          internal::get_error_message("Index out of range: ",
                                                      i));
      return d_[i];
    }
    hsize_t operator[](unsigned int i) const {
      RMF_USAGE_CHECK(i < D,
                          internal::get_error_message("Index out of range: ",
                                                      i));
      return d_[i];
    }
    typedef const hsize_t * const_iterator;
    const_iterator begin() const {
      return d_;
    }
    const_iterator end() const {
      return d_+D;
    }
    typedef hsize_t * iterator;
    iterator begin() {
      return d_;
    }
    iterator end() {
      return d_+D;
    }
    hsize_t* get() const {
      return const_cast<hsize_t*>(d_);
    }
#endif
    int __getitem__(unsigned int i) const {
      if (i >= D) {
        RMF_THROW(internal::get_error_message("Bad index ", i),
                      std::runtime_error);
      }
      return operator[](i);
    }

    unsigned int get_dimension() const {return D;}
    RMF_SHOWABLE(HDF5DataSetIndexD, Ints(d_, d_+D));
    RMF_COMPARISONS(HDF5DataSetIndexD);
    RMF_HASHABLE(HDF5DataSetIndexD,
                     size_t ret=0;
                     for (unsigned int i=0; i< D; ++i) {
                       boost::hash_combine(ret, static_cast<size_t>(d_[i]));
                     }
                     return ret;);
  };

#ifndef RMF_DOXYGEN
  typedef HDF5DataSetIndexD<1> HDF5DataSetIndex1D;
  typedef vector<HDF5DataSetIndex1D> HDF5DataSetIndex1Ds;
  typedef HDF5DataSetIndexD<2> HDF5DataSetIndex2D;
  typedef vector<HDF5DataSetIndex2D> HDF5DataSetIndex2Ds;
  typedef HDF5DataSetIndexD<3> HDF5DataSetIndex3D;
  typedef vector<HDF5DataSetIndex3D> HDF5DataSetIndex3Ds;
#endif

} /* namespace RMF */

#endif /* RMF_HDF_5DATA_SET_INDEX_D_H */
