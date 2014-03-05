%extend RMF::BufferConstHandle {
  %pythoncode %{
     def __getstate__(self):
       return self.get_string()

     def __setstate__(self, st):
       self.__init__(st)
       return self
     %}
  }

%include "RMF/BufferConstHandle.h"
%include "RMF/BufferHandle.h"
