%module "RMF"
%feature("autodoc", 1);
// turn off the warning as it mostly triggers on methods (and lots of them)
%warnfilter(321);

%{
#include <RMF/compiler_macros.h>

RMF_PUSH_WARNINGS
RMF_GCC_PRAGMA(diagnostic ignored "-Wunused-but-set-variable")
RMF_GCC_PRAGMA(diagnostic ignored "-Wunused-value")
RMF_GCC_PRAGMA(diagnostic ignored "-Wmissing-declarations")
RMF_CLANG_PRAGMA(diagnostic ignored "-Wunused-parameter")
RMF_CLANG_PRAGMA(diagnostic ignored "-Wused-but-marked-unused")
RMF_CLANG_PRAGMA(diagnostic ignored "-Wunused-function")

/* SWIG generates long class names with wrappers that use certain Boost classes,
   longer than the 255 character name length for MSVC. This shouldn't affect
   the code, but does result in a lot of warning output, so disable this warning
   for clarity. */
RMF_VC_PRAGMA( warning( disable: 4503 ) )

#include <boost/version.hpp>
#include <boost/exception/exception.hpp>

#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>
#include <exception>

#include "RMF/internal/swig_helpers.h"
#include "RMF.h"

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
%include "RMF/compiler_macros.h"
%include "RMF/infrastructure_macros.h"

%pythoncode %{
_types_list=[]
def get_data_types():
   return _types_list
%}


/* Apply the passed macro to each type used in RMF */
%define RMF_SWIG_FOREACH_TYPE(macroname)
  macroname(int, Int, Ints, int);
  macroname(ints, Ints, IntsList, RMF::Ints);
  macroname(float, Float, Floats, double);
  macroname(floats, Floats, FloatsList, RMF::Floats);
  macroname(string, String, Strings, std::string);
  macroname(strings, Strings, StringsList, RMF::Strings);
  macroname(vector3, Vector3, Vector3s, RMF::Vector3);
  macroname(vector4, Vector4, Vector4s, RMF::Vector4);
  macroname(vector3s, Vector3s, Vector3sList, RMF::Vector3s);
%enddef

/* Declare the needed things for each type */
%define RMF_SWIG_DECLARE_TYPE(lcname, Ucname, Ucnames, Type)
namespace RMF {
%rename(_##Ucname##Traits) Ucname##Traits;
}

RMF_SWIG_VALUE_INSTANCE(RMF, Ucname##Key, Ucname##Key, Ucname##Keys);
%pythoncode %{
_types_list.append(#lcname)
%}
%enddef

%define RMF_SWIG_DEFINE_TYPE(lcname, Ucname, Ucnames, Type)
%template(Ucname##Key) RMF::ID<RMF::Ucname##Traits>;
%enddef

%define RMF_SWIG_WRAP_NULLABLE(lcname, Ucname, Ucnames, Type)
%template(_##Ucname##Nullable) RMF::Nullable<RMF::Ucname##Traits>;
%enddef

%feature("shadow") RMF::NodeConstHandle::get_value const %{
    def get_value(self, *args):
        v = $action(self, *args)
        if v.get_is_null(): return None
        else: return v.get()
%}


RMF_SWIG_VALUE_INSTANCE(RMF, NodeID, ID<NodeTag>, NodeIDs);
RMF_SWIG_VALUE_INSTANCE(RMF, Category, ID<CategoryTag>, Categories);
RMF_SWIG_VALUE_INSTANCE(RMF, FrameID, ID<FrameTag>, FrameIDs);
RMF_SWIG_VALUE_INSTANCE(RMF, Vector3, Vector<3>, Vector3s);
RMF_SWIG_VALUE_INSTANCE(RMF, Vector4, Vector<4>, Vector4s);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, double, Floats, FloatsList);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, int, Ints, IntsList);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, std::string, Strings, StringsList);
RMF_SWIG_VALUE_BUILTIN(RMF, Float, Floats, double);
RMF_SWIG_VALUE_BUILTIN(RMF, Int, Ints, int);
RMF_SWIG_VALUE_BUILTIN(RMF, String, Strings, std::string);
RMF_SWIG_NATIVE_VALUE(float);
RMF_SWIG_NATIVE_VALUE(double);
RMF_SWIG_NATIVE_VALUE(int);
RMF_SWIG_NATIVE_VALUE(std::string);


RMF_SWIG_GRAPH(RMF, NodeTree, NodeTree, RMF::NodeHandle);
RMF_SWIG_VALUE(RMF, NodeConstHandle, NodeConstHandles);
RMF_SWIG_VALUE(RMF, FileConstHandle, FileConstHandles);
RMF_SWIG_VALUE(RMF, NodeHandle, NodeHandles);
RMF_SWIG_VALUE(RMF, FileHandle, FileHandles);
RMF_SWIG_VALUE(RMF, SetCurrentFrame, SetCurrentFrames);
RMF_SWIG_VALUE_TEMPLATE(RMF, ID);
RMF_SWIG_PAIR(RMF, Index, IndexRange, IndexRanges)
RMF_SWIG_PAIR(RMF, Int, IntRange, IntRanges)


RMF_SWIG_VALUE(RMF, ReferenceFrame, ReferenceFrames);


RMF_SWIG_FOREACH_TYPE(RMF_SWIG_DECLARE_TYPE);

%implicitconv;

%include "RMF/ID.h"
%template(FrameID) RMF::ID<RMF::FrameTag>;
%template(NodeID) RMF::ID<RMF::NodeTag>;
%template(Category) RMF::ID<RMF::CategoryTag>;
RMF_SWIG_FOREACH_TYPE(RMF_SWIG_DEFINE_TYPE);

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
        for t in _types_list:
           fn= getattr(self, "get_"+t+"_keys")
           ret.extend(fn(kc))
        return ret
  %}
 }



%include "RMF/constants.h"
%include "RMF/Vector.h"
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
 }
%template(Vector3) RMF::Vector<3>;
%template(Vector4) RMF::Vector<4>;

%include "RMF/types.h"

%include "RMF/Nullable.h"
RMF_SWIG_FOREACH_TYPE(RMF_SWIG_WRAP_NULLABLE);

%include "RMF/names.h"
%include "RMF/enums.h"
%include "RMF/NodeConstHandle.h"
%include "RMF/NodeHandle.h"
%include "RMF/FileConstHandle.h"
%include "RMF/FileHandle.h"
%include "RMF/Validator.h"
%include "RMF/Decorator.h"
namespace RMF {
  %template(_Decorator) Decorator<NodeHandle>;
  %template(_ConstDecorator) Decorator<NodeConstHandle>;
}

%include "RMF/Factory.h"
namespace RMF {
  %template(_Factory) Factory<FileHandle>;
  %template(_ConstFactory) Factory<FileConstHandle>;
}


RMF_DECORATOR(RMF, Particle);
RMF_DECORATOR(RMF, IntermediateParticle);
RMF_DECORATOR(RMF, StaticParticle);
RMF_DECORATOR(RMF, StaticIntermediateParticle);
RMF_DECORATOR(RMF, External);
RMF_DECORATOR(RMF, JournalArticle);
RMF_DECORATOR(RMF, Colored);
RMF_DECORATOR(RMF, Ball);
RMF_DECORATOR(RMF, Cylinder);
RMF_DECORATOR(RMF, Segment);
RMF_DECORATOR(RMF, StaticColored);
RMF_DECORATOR(RMF, StaticBall);
RMF_DECORATOR(RMF, StaticCylinder);
RMF_DECORATOR(RMF, StaticSegment);
RMF_DECORATOR(RMF, Score);
RMF_DECORATOR(RMF, RigidParticle);
RMF_DECORATOR(RMF, ReferenceFrame);
RMF_DECORATOR(RMF, StaticReferenceFrame);
RMF_DECORATOR(RMF, Residue);
RMF_DECORATOR(RMF, Atom);
RMF_DECORATOR(RMF, Alias);
RMF_DECORATOR(RMF, Bond);
RMF_DECORATOR(RMF, Chain);
RMF_DECORATOR(RMF, Domain);
RMF_DECORATOR(RMF, Copy);
RMF_DECORATOR(RMF, Diffuser);
RMF_DECORATOR(RMF, Typed);
RMF_DECORATOR(RMF, Force);
RMF_DECORATOR(RMF, Torque);

%include "RMF/physics_decorators.h"
%include "RMF/sequence_decorators.h"
%include "RMF/shape_decorators.h"
%include "RMF/external_decorators.h"
%include "RMF/feature_decorators.h"
%include "RMF/alias_decorators.h"
%include "RMF/publication_decorators.h"
%include "RMF/decorator_utility.h"
%include "RMF/utility.h"
%include "RMF/info.h"
%include "RMF/SetCurrentFrame.h"
%include "RMF/CoordinateTransformer.h"
%include "RMF/log.h"

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

suffixes=["rmf3", "rmf", "rmf2", "rmfa", "rmft", "rmf3"]

read_write_suffixes=["rmf", "rmfa", "rmft"]

import RMF_HDF5
HDF5=RMF_HDF5

%}

%{
RMF_POP_WARNINGS
%}
