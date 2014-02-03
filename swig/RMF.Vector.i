RMF_SWIG_VALUE_INSTANCE(RMF, Vector3, Vector<3>, Vector3s);
RMF_SWIG_VALUE_INSTANCE(RMF, Vector4, Vector<4>, Vector4s);
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

//%implicitconv RMF::Vector;
%include "RMF/Vector.h"
%template(Vector3) RMF::Vector<3>;
%template(Vector4) RMF::Vector<4>;
