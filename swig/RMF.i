%module "RMF"
%feature("autodoc", 1);


%include "typemaps.i"

%include "std_vector.i"
%include "std_string.i"
%include "std_pair.i"

%include "RMF.warnings.i"

%{
#include <boost/version.hpp>
#include <boost/exception/exception.hpp>

#include <exception>

#include "RMF/internal/swig_helpers.h"
#include "RMF.h"

%}

%include "RMF/config.h"

%pythoncode %{
_value_types=[]
_plural_types=[]
%}



%include "RMF.types.i"
%include "RMF.exceptions.i"
%include "RMF/compiler_macros.h"
%include "RMF/infrastructure_macros.h"

%pythoncode %{
_types_list = []
_traits_list = []
def get_data_types():
   return _types_list
def get_data_traits():
   return _traits_list

def _handle_nullable(n):
   if n.get_is_null(): return None
   else: return n.get()
%}


/* Apply the passed macro to each type used in RMF */
%define RMF_SWIG_FOREACH_TYPE(macroname)
  macroname(int, Int);
  macroname(ints, Ints);
  macroname(float, Float);
  macroname(floats, Floats);
  macroname(string, String);
  macroname(strings, Strings);
  macroname(vector3, Vector3);
  macroname(vector4, Vector4);
  macroname(vector3s, Vector3s);
%enddef

/* Declare the needed things for each type */
%define RMF_SWIG_DECLARE_TYPE(lcname, Ucname)
%rename(_##Ucname##Traits) RMF::Ucname##Traits;
RMF_SWIG_VALUE_INSTANCE(RMF, Ucname##Key, Ucname##Key, Ucname##Keys);
%enddef

%define RMF_SWIG_TYPE_LIST(lcname, UCname)
%pythoncode %{
lcname##_traits = _##UCname##Traits()
_types_list.append(#lcname)
_traits_list.append(lcname##_traits)
%}
%enddef

%define RMF_SWIG_DEFINE_TYPE(lcname, Ucname)
%template(Ucname##Key) RMF::ID<RMF::Ucname##Traits>;
%enddef

%define RMF_SWIG_WRAP_NULLABLE(lcname, Ucname)
%template(_##Ucname##Nullable) RMF::Nullable<RMF::Ucname##Traits>;
%enddef

%define RMF_SHADOW_NULLABLE(Class, function)
%feature("shadow") Class::function const %{
    def function(self, *args):
        return _handle_nullable( $action(self, *args))
%}
%enddef

RMF_SHADOW_NULLABLE(RMF::NodeConstHandle, get_value)
RMF_SHADOW_NULLABLE(RMF::NodeConstHandle, get_static_value)
RMF_SHADOW_NULLABLE(RMF::NodeConstHandle, get_frame_value)


RMF_SWIG_GRAPH(RMF, NodeTree, NodeTree, RMF::NodeHandle);
RMF_SWIG_NATIVE_VALUE(double);
RMF_SWIG_NATIVE_VALUE(float);
RMF_SWIG_NATIVE_VALUE(int);
RMF_SWIG_NATIVE_VALUE(std::string);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, double, Floats, FloatsList);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, int, Ints, IntsList);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, std::string, Strings, StringsList);
RMF_SWIG_PAIR(RMF, Index, IndexRange, IndexRanges)
RMF_SWIG_PAIR(RMF, Int, IntRange, IntRanges)
RMF_SWIG_VALUE(RMF, BufferConstHandle, BufferConstHandles);
RMF_SWIG_VALUE(RMF, BufferHandle, BufferHandles);
RMF_SWIG_VALUE(RMF, FileConstHandle, FileConstHandles);
RMF_SWIG_VALUE(RMF, FileHandle, FileHandles);
RMF_SWIG_VALUE(RMF, NodeConstHandle, NodeConstHandles);
RMF_SWIG_VALUE(RMF, NodeHandle, NodeHandles);
RMF_SWIG_VALUE(RMF, SetCurrentFrame, SetCurrentFrames);
RMF_SWIG_VALUE(RMF, TraverseHelper, TraverseHelpers);
RMF_SWIG_VALUE_BUILTIN(RMF, Float, Floats, double);
RMF_SWIG_VALUE_BUILTIN(RMF, Int, Ints, int);
RMF_SWIG_VALUE_BUILTIN(RMF, String, Strings, std::string);
RMF_SWIG_VALUE_INSTANCE(RMF, Category, ID<CategoryTag>, Categories);
RMF_SWIG_VALUE_INSTANCE(RMF, FrameID, ID<FrameTag>, FrameIDs);
RMF_SWIG_VALUE_INSTANCE(RMF, FrameType, RMF::Enum< RMF::FrameTypeTag >, FrameTypes);
RMF_SWIG_VALUE_INSTANCE(RMF, NodeID, ID<NodeTag>, NodeIDs);
RMF_SWIG_VALUE_INSTANCE(RMF, NodeType, RMF::Enum< RMF::NodeTypeTag> , NodeTypes);
RMF_SWIG_VALUE_INSTANCE(RMF, RepresentationType, RMF::Enum< RMF::RepresentationTypeTag >, RepresentationTypes);
RMF_SWIG_VALUE_INSTANCE(RMF, Vector3, Vector<3>, Vector3s);
RMF_SWIG_VALUE_INSTANCE(RMF, Vector4, Vector<4>, Vector4s);
RMF_SWIG_VALUE_TEMPLATE(RMF, Enum);
RMF_SWIG_VALUE_TEMPLATE(RMF, ID);


RMF_SWIG_FOREACH_TYPE(RMF_SWIG_DECLARE_TYPE);

%implicitconv;

%include "RMF/ID.h"
%template(FrameID) RMF::ID<RMF::FrameTag>;
%template(NodeID) RMF::ID<RMF::NodeTag>;
%template(Category) RMF::ID<RMF::CategoryTag>;
RMF_SWIG_FOREACH_TYPE(RMF_SWIG_DEFINE_TYPE);

namespace RMF {
%rename(_get_keys) FileConstHandle::get_keys;
}

%extend RMF::FileConstHandle {
   %pythoncode %{
    def get_frames(self):
        class MyRange:
           def __init__(self, mx):
               self.max = mx
           def __getitem__(self, i):
               if i >= self.max:
                    raise IndexError()
               else:
                    return FrameID(i)
           def __len__(self):
               return self.max
        return MyRange(self.get_number_of_frames())
    def get_node_ids(self):
        class MyRange:
           def __init__(self, mx):
               self.max = mx
           def __getitem__(self, i):
               if i >= self.max:
                    raise IndexError()
               else:
                    return NodeID(i)
           def __len__(self):
               return self.max
        return MyRange(self.get_number_of_nodes())
    def get_keys(self, kc):
        ret=[]
        for t in _traits_list:
           ret.extend(self._get_keys(kc, t))
        return ret
  %}
}

%include "RMF/BufferConstHandle.h"
%include "RMF/BufferHandle.h"

%include "RMF/constants.h"

%extend RMF::Vector {
 // hack for testing
  bool __eq__(const Vector<D>& o) const {
    for (unsigned int i = 0; i< D; ++i) {
      if (self->operator[](i) != o[i]) return false;
    }
    return true;
  }
  bool __ne__(const Vector<D>&o) const {
    for (unsigned int i = 0; i< D; ++i) {
      if (self->operator[](i) != o[i]) return true;
    }
    return false;
  }
%pythoncode %{
    def __str__(self):
       return str([x for x in self])
    def __repr__(self):
         return self.__str__()
  %}
 }

%include "RMF/Vector.h"

%template(Vector3) RMF::Vector<3>;
%template(Vector4) RMF::Vector<4>;

// old swig doesn't handle expanding the macro properly
%include "RMF/default_types.h"
%include "RMF/traits.h"
%include "RMF/keys.h"

RMF_SWIG_FOREACH_TYPE(RMF_SWIG_TYPE_LIST)

%include "RMF/Nullable.h"
RMF_SWIG_FOREACH_TYPE(RMF_SWIG_WRAP_NULLABLE);

%include "RMF/names.h"
%include "RMF/Enum.h"
namespace RMF {
  %template(NodeType) Enum<NodeTypeTag>;
  %template(FrameType) Enum<FrameTypeTag>;
  %template(RepresentationType) Enum<RepresentationTypeTag>;
}
%include "RMF/enums.h"
%include "RMF/NodeConstHandle.h"
%include "RMF/NodeHandle.h"
%include "RMF/FileConstHandle.h"
%include "RMF/FileHandle.h"
%include "RMF/validate.h"
%include "RMF/Decorator.h"
%include "RMF.decorator.i"

%include "RMF/utility.h"
%include "RMF/signature.h"
%include "RMF/info.h"
%include "RMF/SetCurrentFrame.h"
%include "RMF/RestoreCurrentFrame.h"
%include "RMF/CoordinateTransformer.h"
%include "RMF/log.h"
%include "RMF/show_hierarchy.h"

RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_chain_id);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_residue_index);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_residue_type);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_molecule_name);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_rgb_color);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_copy_index);

%include "RMF/TraverseHelper.h"

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

def _os_path_split_asunder(path):
    import os.path
    parts = []
    while True:
        newpath, tail = os.path.split(path)
        if newpath == path:
            assert not tail
            if path: parts.append(path)
            break
        parts.append(tail)
        path = newpath
    parts.reverse()
    return parts


def get_example_path(name):
   import sys
   import os.path
   dir= _os_path_split_asunder(sys.argv[0])[:-1]
   dir.append(name)
   return os.path.join(*dir)

suffixes=["rmf", "rmfz", "rmf-avro", "rmf3", "rmf-hdf5", "_rmf_test_buffer"]

import RMF_HDF5
HDF5=RMF_HDF5

%}

%{
RMF_POP_WARNINGS
%}

%pythoncode %{
def _assert_signatures_equal(sa, sb):
  if sa == sb:
     return
  else:
     import difflib
     for l in difflib.unified_diff(sa.split("\n"), sb.split("\n")):
        stl = str(l)
        print stl
     raise RuntimeError("not equal")

%}
