%module "RMF_HDF5"
%feature("autodoc", 1);
// turn off the warning as it mostly triggers on methods (and lots of them)
%warnfilter(321);

%{

#pragma GCC diagnostic ignored "-Wunused-value"

/* SWIG generates long class names with wrappers that use certain Boost classes,
   longer than the 255 character name length for MSVC. This shouldn't affect
   the code, but does result in a lot of warning output, so disable this warning
   for clarity. */
#ifdef _MSC_VER
#pragma warning( disable: 4503 )
#endif

#include <boost/version.hpp>
#include <boost/exception/all.hpp>

#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>
#include <exception>

#include "RMF/internal/swig_helpers.h"
#include "RMF/HDF5.h"

%}
%include "std_vector.i"
%include "std_string.i"
%include "std_pair.i"

%include "RMF/config.h"

%pythoncode %{
_value_types=[]
_object_types=[]
_raii_types=[]
_plural_types=[]
%}



%include "typemaps.i"

%include "RMF.types.i"
%include "RMF.exceptions.i"
%include "RMF/infrastructure_macros.h"

%pythoncode %{
_types_list=[]
def get_data_types():
   return _types_list
%}

/* Apply the passed macro to each type used in RMF */
%define IMP_RMF_SWIG_FOREACH_HDF5_TYPE(macroname)
  macroname(int, Int, Ints, int);
  macroname(ints, Ints, IntsList, RMF::Ints);
  macroname(float, Float, Floats, double);
  macroname(floats, Floats, FloatsList, RMF::Floats);
  macroname(index, Index, Indexes, int);
  macroname(indexes, Indexes, IndexesList, RMF::Indexes);
  macroname(string, String, Strings, std::string);
  macroname(strings, Strings, StringsList, RMF::Strings);
%enddef


/* Declare the needed things for each type */
%define IMP_RMF_SWIG_DECLARE_HDF5_TYPE(lcname, Ucname, Ucnames, Type)
namespace RMF {
  namespace HDF5 {
    %rename(_HDF5##Ucname##Traits) Ucname##Traits;
  }
}
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##DataSet1D, HDF5##Ucname##DataSet1D, HDF5##Ucname##DataSet1Ds);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##DataSet2D, HDF5##Ucname##DataSet2D, HDF5##Ucname##DataSet2Ds);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##DataSet3D, HDF5##Ucname##DataSet3D, HDF5##Ucname##DataSet3Ds);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##DataSet1DAttributes, HDF5##Ucname##DataSet1DAttributes, HDF5##Ucname##DataSet1DAttributesList);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##DataSet2DAttributes, HDF5##Ucname##DataSet2DAttributes, HDF5##Ucname##DataSet2DAttributesList);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##DataSet3DAttributes, HDF5##Ucname##DataSet3DAttributes, HDF5##Ucname##DataSet3DAttributesList);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##ConstDataSet1D, HDF5##Ucname##ConstDataSet1D, HDF5##Ucname##ConstDataSet1Ds);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##ConstDataSet2D, HDF5##Ucname##ConstDataSet2D, HDF5##Ucname##ConstDataSet2Ds);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5##Ucname##ConstDataSet3D, HDF5##Ucname##ConstDataSet3D, HDF5##Ucname##ConstDataSet3Ds);
%enddef


%define IMP_RMF_SWIG_DEFINE_INTERMEDIATE_HDF5_TYPE(lcname, Ucname, Ucnames, Type)
%template(HDF5##Ucname##ConstDataSet1D) RMF::HDF5::HDF5ConstDataSetD<RMF::HDF5::Ucname##Traits, 1>;
%template(HDF5##Ucname##ConstDataSet2D) RMF::HDF5::HDF5ConstDataSetD<RMF::HDF5::Ucname##Traits, 2>;
%template(HDF5##Ucname##ConstDataSet3D) RMF::HDF5::HDF5ConstDataSetD<RMF::HDF5::Ucname##Traits, 3>;
%template(HDF5##Ucname##DataSetAttributes1D) RMF::HDF5::HDF5MutableAttributes< RMF::HDF5::HDF5ConstDataSetD<RMF::HDF5::Ucname##Traits, 1> >;
%template(HDF5##Ucname##DataSetAttributes2D) RMF::HDF5::HDF5MutableAttributes< RMF::HDF5::HDF5ConstDataSetD<RMF::HDF5::Ucname##Traits, 2> >;
%template(HDF5##Ucname##DataSetAttributes3D) RMF::HDF5::HDF5MutableAttributes< RMF::HDF5::HDF5ConstDataSetD<RMF::HDF5::Ucname##Traits, 3> >;
%enddef

%define IMP_RMF_SWIG_DEFINE_HDF5_TYPE(lcname, Ucname, Ucnames, Type)
%template(HDF5##Ucname##DataSet1D) RMF::HDF5::HDF5DataSetD<RMF::HDF5::Ucname##Traits, 1>;
%template(HDF5##Ucname##DataSet2D) RMF::HDF5::HDF5DataSetD<RMF::HDF5::Ucname##Traits, 2>;
%template(HDF5##Ucname##DataSet3D) RMF::HDF5::HDF5DataSetD<RMF::HDF5::Ucname##Traits, 3>;
%enddef

IMP_RMF_SWIG_VALUE(RMF::HDF5, HDF5Object, HDF5Objects);
IMP_RMF_SWIG_VALUE_TEMPLATE(RMF::HDF5, HDF5ConstAttributes);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5ConstGroupAttributes,HDF5ConstGroupAttributes, HDF5ConstGroupAttributesList);
IMP_RMF_SWIG_VALUE(RMF::HDF5, HDF5ConstGroup, HDF5ConstGroups);
IMP_RMF_SWIG_VALUE(RMF::HDF5, HDF5ConstFile, HDF5ConstFiles);
IMP_RMF_SWIG_VALUE_TEMPLATE(RMF::HDF5, HDF5MutableAttributes);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5GroupAttributes,HDF5GroupAttributes, HDF5GroupAttributesList);
IMP_RMF_SWIG_VALUE(RMF::HDF5, HDF5Group, HDF5Groups);
IMP_RMF_SWIG_VALUE(RMF::HDF5, HDF5File, HDF5Files);
IMP_RMF_SWIG_VALUE_TEMPLATE(RMF::HDF5, HDF5DataSetD);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5DataSetIndex1D, HDF5DataSetIndex1D, HDF5DataSetIndex1Ds);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5DataSetIndex2D, HDF5DataSetIndex2D, HDF5DataSetIndex2Ds);
IMP_RMF_SWIG_VALUE_INSTANCE(RMF::HDF5, HDF5DataSetIndex3D, HDF5DataSetIndex3D, HDF5DataSetIndex3Ds);

IMP_RMF_SWIG_FOREACH_HDF5_TYPE(IMP_RMF_SWIG_DECLARE_HDF5_TYPE);

%implicitconv;

// char is special cased since it is just used for attributes
namespace RMF {
  namespace HDF5 {
%rename(_CharTraits) CharTraits;
  }
}

%include "RMF/HDF5/hdf5_types.h"
%include "RMF/HDF5/hdf5_handle.h"
%include "RMF/HDF5/HDF5Object.h"
%include "RMF/HDF5/HDF5ConstAttributes.h"
%template(_HDF5ConstAttributesObject) RMF::HDF5::HDF5ConstAttributes<RMF::HDF5::HDF5Object>;
%include "RMF/HDF5/HDF5MutableAttributes.h"
%include "RMF/HDF5/HDF5DataSetIndexD.h"
%template(HDF5DataSetIndex1D) RMF::HDF5::HDF5DataSetIndexD<1>;
%template(HDF5DataSetIndex2D) RMF::HDF5::HDF5DataSetIndexD<2>;
%template(HDF5DataSetIndex3D) RMF::HDF5::HDF5DataSetIndexD<3>;

%include "RMF/HDF5/HDF5DataSetAccessPropertiesD.h"
%include "RMF/HDF5/HDF5DataSetCreationPropertiesD.h"
%include "RMF/HDF5/HDF5ConstDataSetD.h"
IMP_RMF_SWIG_FOREACH_HDF5_TYPE(IMP_RMF_SWIG_DEFINE_INTERMEDIATE_HDF5_TYPE);
%include "RMF/HDF5/HDF5DataSetD.h"


IMP_RMF_SWIG_FOREACH_HDF5_TYPE(IMP_RMF_SWIG_DEFINE_HDF5_TYPE);

%include "RMF/HDF5/HDF5ConstGroup.h"
%template(_HDF5MutableAttributesGroup) RMF::HDF5::HDF5MutableAttributes<RMF::HDF5::HDF5ConstGroup>;
%include "RMF/HDF5/HDF5ConstFile.h"
%include "RMF/HDF5/HDF5Group.h"
%include "RMF/HDF5/HDF5File.h"

%pythoncode %{
_tmpdir=None

def _get_temporary_file_path(name):
   global _tmpdir
   if not _tmpdir:
       import tempfile
       _tmpdir = tempfile.mkdtemp()
   import os.path
   return os.path.join(_tmpdir, name)

def _get_test_input_file_path(name):
   import sys
   import os.path
   dir= os.path.split(sys.argv[0])[0]
   return os.path.join(dir, "input", name)

def get_example_path(name):
   import sys
   import os.path
   dir= os.path.split(sys.argv[0])[0]
   return os.path.join(dir, name)
%}
