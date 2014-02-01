%module "RMF"
%feature("autodoc", 1);

%{
#include <boost/version.hpp>
#include <boost/exception/exception.hpp>

#include <exception>

#include "RMF/internal/swig_helpers.h"
#include "RMF.h"

%}

%include "typemaps.i"

%include "std_vector.i"
%include "std_string.i"
%include "std_pair.i"

%include "RMF.warnings.i"
%include "RMF/config.h"
%include "RMF.types.i"
%include "RMF.exceptions.i"
%include "RMF/compiler_macros.h"
%include "RMF/infrastructure_macros.h"

 // see if we need this
 //%implicitconv;

%pythoncode %{
_value_types=[]
_plural_types=[]
%}





RMF_SWIG_NATIVE_VALUE(double);
RMF_SWIG_NATIVE_VALUE(float);
RMF_SWIG_NATIVE_VALUE(int);
RMF_SWIG_NATIVE_VALUE(std::string);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, double, Floats, FloatsList);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, int, Ints, IntsList);
RMF_SWIG_NATIVE_VALUES_LIST(RMF, std::string, Strings, StringsList);
RMF_SWIG_PAIR(RMF, Index, IndexRange, IndexRanges)
RMF_SWIG_PAIR(RMF, Int, IntRange, IntRanges)
RMF_SWIG_VALUE(RMF, TraverseHelper, TraverseHelpers);
RMF_SWIG_VALUE_BUILTIN(RMF, Float, Floats, double);
RMF_SWIG_VALUE_BUILTIN(RMF, Int, Ints, int);
RMF_SWIG_VALUE_BUILTIN(RMF, String, Strings, std::string);


RMF_SWIG_VALUE_INSTANCE(RMF, Category, ID<CategoryTag>, Categories);
RMF_SWIG_VALUE_INSTANCE(RMF, FrameID, ID<FrameTag>, FrameIDs);
RMF_SWIG_VALUE_INSTANCE(RMF, NodeID, ID<NodeTag>, NodeIDs);
RMF_SWIG_VALUE_TEMPLATE(RMF, ID);
%include "RMF/ID.h"
%template(FrameID) RMF::ID<RMF::FrameTag>;
%template(NodeID) RMF::ID<RMF::NodeTag>;
%template(Category) RMF::ID<RMF::CategoryTag>;

RMF_SWIG_FOREACH_TYPE(RMF_SWIG_DECLARE_TYPE);

%include "RMF.Vector.i"

%{
  // hack, I don't understand what swig is doing
  typedef RMF::Vector<3> Vector3;
  typedef RMF::Vector<4> Vector4;
%}

// old swig doesn't handle expanding the macro properly
%include "RMF/default_types.h"
 /*%template(IntsV) std::vector<RMF::Int>;
%template(FloatsV) std::vector<RMF::Float>;
%template(StringsV) std::vector<RMF::String>;
%template(Vector3sV) std::vector<RMF::Vector3>;
%template(Vector4sV) std::vector<RMF::Vector4>;*/

%include "RMF.traits.i"
%include "RMF.keys.i"

%include "RMF.nullable.i"


RMF_SWIG_VALUE_TEMPLATE(RMF, Enum);
RMF_SWIG_VALUE_INSTANCE(RMF, NodeType, RMF::Enum< RMF::NodeTypeTag> , NodeTypes);
RMF_SWIG_VALUE_INSTANCE(RMF, RepresentationType, RMF::Enum< RMF::RepresentationTypeTag >, RepresentationTypes);
RMF_SWIG_VALUE_INSTANCE(RMF, FrameType, RMF::Enum< RMF::FrameTypeTag >, FrameTypes);
%include "RMF/Enum.h"
namespace RMF {
  %template(NodeType) Enum<NodeTypeTag>;
  %template(FrameType) Enum<FrameTypeTag>;
  %template(RepresentationType) Enum<RepresentationTypeTag>;
}
%include "RMF/enums.h"

RMF_SHADOW_NULLABLE(RMF::NodeConstHandle, get_value)
RMF_SHADOW_NULLABLE(RMF::NodeConstHandle, get_static_value)
RMF_SHADOW_NULLABLE(RMF::NodeConstHandle, get_frame_value)
RMF_SWIG_VALUE(RMF, NodeConstHandle, NodeConstHandles);
%include "RMF/NodeConstHandle.h"

RMF_SWIG_VALUE(RMF, NodeHandle, NodeHandles);
%include "RMF/NodeHandle.h"

RMF_SWIG_VALUE(RMF, BufferConstHandle, BufferConstHandles);
%include "RMF/BufferConstHandle.h"
RMF_SWIG_VALUE(RMF, BufferHandle, BufferHandles);
%include "RMF/BufferHandle.h"


%include "RMF.FileConstHandle.i"

RMF_SWIG_VALUE(RMF, FileHandle, FileHandles);
%include "RMF/FileHandle.h"

RMF_SWIG_VALUE(RMF, Decorator, Decorators);
%include "RMF/Decorator.h"

%include "RMF.decorator.i"

RMF_SWIG_VALUE(RMF, SetCurrentFrame, SetCurrentFrames);
%include "RMF/SetCurrentFrame.h"

RMF_SWIG_VALUE(RMF, RestoreCurrentFrame, RestoreCurrentFrames);
%include "RMF/RestoreCurrentFrame.h"

RMF_SWIG_VALUE(RMF, CoordinateTransformer, CoordinateTransformers);
%include "RMF/CoordinateTransformer.h"

RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_chain_id);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_residue_index);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_residue_type);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_molecule_name);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_rgb_color);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_copy_index);
%include "RMF/TraverseHelper.h"


 // Do not declare types, so order doesn't matter
%include "RMF/constants.h"
%include "RMF/info.h"
%include "RMF/log.h"
%include "RMF/names.h"
%include "RMF/show_hierarchy.h"
%include "RMF/signature.h"
%include "RMF/utility.h"
%include "RMF/validate.h"



%include "RMF.python.i"
