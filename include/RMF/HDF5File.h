/**
 *  \file RMF/HDF5File.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPLIBRMF_HDF_5FILE_H
#define IMPLIBRMF_HDF_5FILE_H

#include <RMF/config.h>
#include "HDF5Group.h"
#include "HDF5ConstFile.h"


namespace RMF {
  /** Store a handle to an HDF5 file. See
   \external{http://www.hdfgroup.org/HDF5/doc/UG/UG_frame08TheFile.html,
  the HDF5 manual} for more information.*/
  class RMFEXPORT HDF5File: public HDF5Group {
  public:
#if !defined(IMP_DOXYGEN) && !defined(SWIG)
    HDF5File(HDF5SharedHandle *h);
    // silliness to make RMF easier to implement
    bool get_is_writable() const {
      unsigned int intent;
      IMP_HDF5_CALL(H5Fget_intent(get_handle(), &intent));
      return intent==H5F_ACC_RDWR;
    }
#endif
    IMP_RMF_SHOWABLE(HDF5File, "HDF5File " << get_name());
    HDF5File(){}
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

  /** */
  typedef vector<HDF5Group> HDF5Groups;
  /** */
  typedef vector<HDF5File> HDF5Files;


} /* namespace RMF */

#endif /* IMPLIBRMF_HDF_5FILE_H */