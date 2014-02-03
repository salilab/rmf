%module "RMF"
%feature("autodoc", 1);


%extend std::vector {
  std::string __str__() const {
    std::ostringstream out;
    out << RMF::Showable(*self);
    return out.str();
  }
  std::string __repr__() const {
    std::ostringstream out;
    out << RMF::Showable(*self);
    return out.str();
  }
  /*bool __eq__(const std::vector<T> &o) const {
    if (self->size() != o.size()) return false;
    for (unsigned int i= 0; i < self->size(); ++i) {
      if (self->operator[](i) != o[i]) return false;
    }
    return true;
  }
  bool __ne__(const std::vector<T> &o) const {
    if (self->size() != o.size()) return true;
    for (unsigned int i= 0; i < self->size(); ++i) {
      if (self->operator[](i) != o[i]) return true;
    }
    return false;
    }*/
}

%{
#define RMF_SWIG_WRAPPER
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



RMF_SWIG_PAIR(RMF, Index, IndexRange, IndexRanges)
RMF_SWIG_PAIR(RMF, Int, IntRange, IntRanges)

%include "RMF/ID.h"
%template(FrameID) RMF::ID<RMF::FrameTag>;
%template(NodeID) RMF::ID<RMF::NodeTag>;
%template(Category) RMF::ID<RMF::CategoryTag>;
%template(FrameIDs) std::vector<RMF::ID<RMF::FrameTag> >;
%template(NodeIDs) std::vector<RMF::ID<RMF::NodeTag> >;
%template(Categories) std::vector<RMF::ID<RMF::CategoryTag> >;

%include "RMF.Vector.i"

%{
  // hack, I don't understand what swig is doing
  typedef RMF::Vector<3> Vector3;
  typedef RMF::Vector<4> Vector4;
%}

// old swig doesn't handle expanding the macro properly
%include "RMF/default_types.h"
 // have to be the real type due to https://github.com/swig/swig/issues/73
%template(Ints) std::vector<int>;
%template(Floats) std::vector<float>;
%template(Strings) std::vector<std::string>;
%template(Vector3s) std::vector<RMF::Vector<3> >;
%template(Vector4s) std::vector<RMF::Vector<4> >;
%template(IntsList) std::vector<std::vector<int> >;
%template(FloatsList) std::vector<std::vector<float> >;
%template(StringsList) std::vector<std::vector<std::string> >;
%template(Vector3sList) std::vector<std::vector<RMF::Vector<3> > >;


%include "RMF.traits.i"
%include "RMF.keys.i"

%include "RMF.nullable.i"


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
%include "RMF/NodeConstHandle.h"
RMF_SWIG_VECTOR(RMF, NodeConstHandle)

%include "RMF/NodeHandle.h"
RMF_SWIG_VECTOR(RMF, NodeHandle)

%include "RMF/BufferConstHandle.h"
%include "RMF/BufferHandle.h"


%include "RMF.FileConstHandle.i"

%include "RMF/FileHandle.h"

%include "RMF/Decorator.h"

%include "RMF.decorator.i"

%include "RMF/SetCurrentFrame.h"

%include "RMF/RestoreCurrentFrame.h"

%include "RMF/CoordinateTransformer.h"

RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_chain_id);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_residue_index);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_residue_type);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_molecule_name);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_rgb_color);
RMF_SHADOW_NULLABLE(RMF::TraverseHelper, get_copy_index);
%include "RMF/TraverseHelper.h"
RMF_SWIG_VECTOR(RMF, TraverseHelper)

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
