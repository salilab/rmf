/**
 *  \file RMF/HDF5ConstAttributes.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF__HDF_5CONST_ATTRIBUTES_H
#define RMF__HDF_5CONST_ATTRIBUTES_H

#include <RMF/config.h>
#include "types.h"
#include "hdf5_handle.h"
#include "infrastructure_macros.h"
#include <hdf5.h>

namespace RMF {

  /** Wrap an gettings the attributes from an HDF5 Object. Each data set or
      group can have associated data. These data should be relativel small
      (a few k at most). See HDF5MutableAttributes for the methods to change
      the attribute data.
  */
  template <class Base>
  class HDF5ConstAttributes: public Base {
#ifndef SWIG
  protected:
    HDF5ConstAttributes(HDF5SharedHandle *h): Base(h){}
    HDF5ConstAttributes() {}
#else
 private:
    HDF5ConstAttributes();
#endif
  public:
    /** \name Template attribute methods
        When manipulating attriutes from C++ you can use these
        templated methods.
        @{
    */
    template <class TypeTraits>
      typename TypeTraits::Types
      get_attribute(std::string name) const {
      if (!H5Aexists(Base::get_shared_handle()->get_hid(), name.c_str())) {
        return typename TypeTraits::Types();
      } else {
        RMF_HDF5_HANDLE(a, H5Aopen(Base::get_shared_handle()->get_hid(),
                                   name.c_str(), H5P_DEFAULT),
                     &H5Aclose);
        RMF_HDF5_HANDLE(s, H5Aget_space(a), &H5Sclose);
        hsize_t dim, maxdim;
        RMF_HDF5_CALL(H5Sget_simple_extent_dims(s, &dim, &maxdim));
        typename TypeTraits::Types ret
          = TypeTraits::read_values_attribute(a, dim);
        return ret;
      }
    }
    /** @} */
    template <class CT, class CF>
      CT copy_to(const CF &cf) const {
      return CT(cf.begin(), cf.end());
    }
    bool get_has_attribute(std::string nm) const {
      return H5Aexists_by_name(Base::get_handle(), ".",
                               nm.c_str(), H5P_DEFAULT) > 0;
    }
    /** \name Nontemplated attributes
        When using python, call the non-template versions of the
        attribute manipulation methods.
        @{
    */
#define RMF_HDF5_CONST_ATTRIBUTE(lcname, UCName, PassValue, ReturnValue, \
                           PassValues, ReturnValues)                    \
    ReturnValues                                                        \
      get_##lcname##_attribute(std::string nm) const {                  \
      return get_attribute< UCName##Traits>(nm);                        \
    }                                                                   \

    RMF_FOREACH_SIMPLE_TYPE(RMF_HDF5_CONST_ATTRIBUTE);
    RMF_HDF5_CONST_ATTRIBUTE(char, Char, char, char, std::string, std::string);
    /** @} */
  };

} /* namespace RMF */

#endif /* RMF__HDF_5CONST_ATTRIBUTES_H */
