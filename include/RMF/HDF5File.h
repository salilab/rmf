/**
 *  \file RMF/HDF5File.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_HDF_5FILE_H
#define RMF_HDF_5FILE_H

#include <RMF/config.h>
#include "HDF5Group.h"
#include "HDF5ConstFile.h"


namespace RMF {
/** \namespace HDF5
    We provide a simple set of classes for accessing core HDF5 functionality from C++. This was needed since
    - The HDF5 C interface is not trivial to use peroperly, particularly in regards to data type conversions and resource management
    - It is very easy to use the C interface incorrectly without knowning it.
    - The \external{http://hdf5.org, standard HDF5 C++ interface} doesn't really simplify use of the HDF5 library and doesn't make use of the features of C++.

    The main classes provide access too:
    - HDF5 files through RMF::HDF5::HDF5File and RMF::HDF5::HDF5ConstFile
    - HDF5 groups through RMF::HDF5::HDF5Group and RMF::HDF5::HDF5ConstGroup
    - HDF5 data sets through RMF::HDF5::HDF5DataSetD and RMF::HDF5::HDF5ConstDataSetD

    The \c Const variants are for read only files.

    They allow one to create a manipulate data sets containing floating point values, integers, strings, variable length arrays of floating point values and a few other types. Attributes on data sets of groups of those types can also be manipulated.

    The top level classes are, in turn, derived from RMF::HDF5::HDF5Object, RMF::HDF5::HDF5ConstAttributes and RMF::HDF5::HDF5MutableAttributes which provide access to general HDF5 per-object and attribute functionality (all objects in HDF5 can have arbitrary small pieces of data attached to them via attributes).

    Not all of the functionality of the HDF5 \c C library is covered in C++. You can get the corresponding native handle from most objects with methods like RMF::HDF5File::get_handle() and use that for other operations if needed. Or submit a patch to the library.
*/

namespace HDF5 {

/** Store a handle to an HDF5 file. See
   \external{http://www.hdfgroup.org/HDF5/doc/UG/UG_frame08TheFile.html,
   the HDF5 manual} for more information.*/
class RMFEXPORT HDF5File: public HDF5Group {
public:
#if !defined(RMF_DOXYGEN) && !defined(SWIG)
  HDF5File(HDF5SharedHandle *h);
  // silliness to make RMF easier to implement
  bool get_is_writable() const {
    unsigned int intent;
    RMF_HDF5_CALL(H5Fget_intent(get_handle(), &intent));
    return intent == H5F_ACC_RDWR;
  }
#endif
  RMF_SHOWABLE(HDF5File, "HDF5File " << get_name());
  HDF5File() {
  }
  std::string get_name() const;
  void flush();
  ~HDF5File();
};

/** Create a new hdf5 file, clearing any existing file with the same
    name if needed. The file cannot already be open.

    \exception RMF::IOException couldn't create file
 */
RMFEXPORT HDF5File create_hdf5_file(std::string name);

/** Open an existing hdf5 file. The file cannot already be open/.

    \exception RMF::IOException couldn't open file
 */
RMFEXPORT HDF5File open_hdf5_file(std::string name);

#ifndef RMF_DOXYGEN
/** Open an existing hdf5 file read only. The file cannot already
    be open.
 */
RMFEXPORT HDF5File
open_hdf5_file_read_only_returning_nonconst(std::string name);
#endif

/** */
typedef std::vector<HDF5Group> HDF5Groups;
/** */
typedef std::vector<HDF5File> HDF5Files;

} /* namespace HDF5 */
} /* namespace RMF */

#endif /* RMF_HDF_5FILE_H */
