
%{
#define RMF_SWIG_CPP_WARNING(string) WARN_PREPROCESS(string)
  %}

/*%pythonprepend Namespace::Name::~Name %{
  print "bye"
  %}*/

/* SWIG's default behavior for functions that return a reference to an object
   is to create a Python object that points to that object. This doesn't work
   well if the IMP object is not refcounted and the C++ object is deleted before
   it is used in Python (for example, algebra::Transformation3D::get_rotation()
   returns a const reference to its internal algebra::Rotation3D object; if the
   Transformation3D is destroyed before the Rotation3D is used, an invalid
   memory access will ensue).

   This typemap modifies the default behavior to instead copy the returned
   object (as if it were returned by value). It should *probably* only be used
   for simple types that do not use large amounts of memory, and only for
   returns of const references. (In other cases, it may make sense to make the
   type refcounted, or not to expose it to Python.)

   Example usage:

   %apply REFCOPY & { const Rotation3D & };
*/
%typemap(out, noblock=1) REFCOPY & {
  %set_output(SWIG_NewPointerObj(%new_copy(*$1, $*ltype), $descriptor, SWIG_POINTER_OWN | %newpointer_flags));
 }

/* size_t is an unsigned type, but Python only has signed integer types.
   Thus a large size_t can overflow a regular Python 'int'. SWIG is
   conservative and so converts such large values to Python 'long's instead.
   This causes hashing to fail on Python < 2.5 though, since hash values must
   be ints rather than longs. But for a hash value we don't actually care
   whether it's signed or not. So we override the default here and force the
   hash value into a signed type, so it will always fit into a Python 'int'. */
%typemap(out) std::size_t __hash__ {
  $result = PyInt_FromLong(static_cast<long>($1));
}

/* Add additional CONTAINER methods for scripting languages */
%define RMF_SWIG_CONTAINER(Namespace, ContainedNamespace, type, Ucname, lcname)
  %extend Namespace::type {
  ContainedNamespace::Ucname##s get_##lcname##s() const {
    ContainedNamespace::Ucname##s ret(self->lcname##s_begin(), self->lcname##s_end());
    return ret;
  }
 }
%enddef






/*%{
  BOOST_STATIC_ASSERT(Convert<IMP::Particle>::converter ==2);
  BOOST_STATIC_ASSERT(Convert<IMP::internal::_TrivialDecorator>::converter ==3);
  %}*/

%define RMF_SWIG_SHOWSTUFF(Name)
  std::string __str__() const {
  std::ostringstream out;
  self->show(out);
    return out.str();
  }
  std::string __repr__() const {
    std::ostringstream out;
    self->show(out);
    return out.str();
  }
%enddef

%define RMF_SWIG_SHOWABLE(Namespace, Name)
     %extend Namespace::Name {
  RMF_SWIG_SHOWSTUFF(Name);
 }
%enddef




%define RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, Name, PluralName, CONSTREF)
  %typemap(in) Namespace::PluralName CONSTREF {
  try {
    // hack to get around swig's value wrapper being randomly used
    assign($1, ConvertSequence<Namespace::PluralName, Convert< Name > >::get_cpp_object($input, $descriptor(Name*)));
  } catch (const RMF::Exception &e) {
    //PyErr_SetString(PyExc_ValueError,"Wrong type in sequence");
    PyErr_SetString(PyExc_TypeError, e.what());
    return NULL;
  }
 }
%typemap(freearg) Namespace::PluralName CONSTREF {
  delete_if_pointer($1);
 }
%typecheck(SWIG_TYPECHECK_POINTER) Namespace::PluralName CONSTREF {
  $1= ConvertSequence<Namespace::PluralName, Convert< Name > >::get_is_cpp_object($input, $descriptor(Name*));
 }
%typemap(out) Namespace::PluralName CONSTREF {
  $result = ConvertSequence<Namespace::PluralName, Convert< Name > >::create_python_object(ValueOrObject<Namespace::PluralName >::get($1), $descriptor(Name*), SWIG_POINTER_OWN);
 }
%typemap(directorout) Namespace::PluralName CONSTREF {
  // hack to get around swig's evil value wrapper being randomly used
  assign($result, ConvertSequence<Namespace::PluralName, Convert< Name > >::get_cpp_object($input, $descriptor(Name*)));
 }
%typemap(directorin) Namespace::PluralName CONSTREF {
  $input = ConvertSequence<Namespace::PluralName, Convert< Name > >::create_python_object($1_name, $descriptor(Name*), SWIG_POINTER_OWN);
 }
%typemap(in) Namespace::PluralName* {
  collections_like_##PluralName##_must_be_passed_by_value_or_const_ref;
 }
/*%typemap(out) Namespace::PluralName* {
  collections_like_##PluralName##_must_be_returned_by_value_or_const_ref;
  }*/
%typemap(in) Namespace::PluralName& {
  collections_like_##PluralName##_must_be_passed_by_value_or_const_ref;
 }
%typemap(out) Namespace::PluralName& {
  collections_like_##PluralName##_must_be_returned_by_value_or_const_ref;
 }
%enddef





%define RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(Name, IntermediateName, PluralName, CONSTREF)
  %typemap(in) PluralName CONSTREF {
  try {
    // hack to get around swig's value wrapper being randomly used
    assign($1, ConvertSequence<PluralName, ConvertSequence< IntermediateName, Convert< Name > > >::get_cpp_object($input, $descriptor(Name*)));
  } catch (const RMF::Exception &e) {
    //PyErr_SetString(PyExc_ValueError,"Wrong type in sequence");
    PyErr_SetString(PyExc_TypeError, e.what());
    return NULL;
  }
 }
%typemap(freearg) PluralName CONSTREF {
  delete_if_pointer($1);
 }
%typecheck(SWIG_TYPECHECK_POINTER) PluralName CONSTREF {
  $1= ConvertSequence<PluralName, ConvertSequence< IntermediateName, Convert< Name > > >::get_is_cpp_object($input, $descriptor(Name*));
 }
%typemap(out) PluralName CONSTREF {
  $result = ConvertSequence<PluralName, ConvertSequence< IntermediateName, Convert< Name > > >::create_python_object(ValueOrObject<PluralName >::get($1), $descriptor(Name*), SWIG_POINTER_OWN);
 }
%typemap(directorout) PluralName CONSTREF {
  // hack to get around swig's evil value wrapper being randomly used
  assign($result, ConvertSequence<PluralName, ConvertSequence< IntermediateName, Convert< Name > > >::get_cpp_object($input, $descriptor(Name*)));
 }
%typemap(directorin) PluralName CONSTREF {
  $input = ConvertSequence<PluralName, ConvertSequence< IntermediateName, Convert< Name > > >::create_python_object($1_name, $descriptor(Name*), SWIG_POINTER_OWN);
 }
%typemap(in) PluralName* {
  collections_like_##PluralName##_must_be_passed_by_value_or_const_ref;
 }
/*%typemap(out) PluralName* {
  collections_like_##PluralName##_must_be_returned_by_value_or_const_ref;
  }*/
%typemap(in) PluralName& {
  collections_like_##PluralName##_must_be_passed_by_value_or_const_ref;
 }
%typemap(out) PluralName& {
  collections_like_##PluralName##_must_be_returned_by_value_or_const_ref;
 }
%enddef









%define RMF_SWIG_VALUE_CHECKS(Namespace, Name, Type)
%typemap(in) Namespace::Name const& = Type const &;
%typecheck(SWIG_TYPECHECK_POINTER) Namespace::Name const& = Type const &;
%typemap(in) Namespace::Name & {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
}

%typemap(in) Namespace::Name *self = Type *;
%typecheck(SWIG_TYPECHECK_POINTER) Namespace::Name *self = Type *;
%typemap(in) Namespace::Name * {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
}
%typemap(directorin) Namespace::Name * {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
  }



//%typemap(out) Namespace::Name const& = SWIGTYPE const &;
/*%typemap(out) Namespace::Name & {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_non_const_ref;
  }*/

%typemap(out) Namespace::Name *self = Type *;
%typemap(out) Namespace::Name *Namespace::Name = Type *;
/*%typemap(out) Namespace::Name * {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_pointer;
  }*/


%typemap(directorout) Namespace::Name & {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_non_const_ref;
}
%typemap(directorout) Namespace::Name * {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_pointer;
}
// for newer swig
%typemap(out) Namespace::Name *Namespace::Name::Name = Type *;

%enddef



%define RMF_SWIG_VALUE_TUPLE(Namespace, Name, PluralName)
RMF_SWIG_VALUE_CHECKS(Namespace, Name, SWIGTYPE);
RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(IMP::ParticleIndex, Namespace::Name, Namespace::PluralName, const&);
RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(IMP::ParticleIndex, Namespace::Name, Namespace::PluralName,);
RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, IMP::ParticleIndex, Name, const&);
RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, IMP::ParticleIndex, Name,);
%pythoncode %{
PluralName=list
_plural_types.append(#PluralName)
_value_types.append(#Name)
%}
%feature("valuewrapper") PluralName;
%enddef
 //SWIGTYPE_p_##SwigNamespace##Name






%define RMF_SWIG_VALUE_IMPL(Namespace, Name, TemplateName, UniqueName, PluralName)
RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, Namespace::Name, PluralName, const&);
RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, Namespace::Name, PluralName,);

%typemap(out) Namespace::Name const& {
  $result=SWIG_NewPointerObj(new Namespace::Name(*$1), $descriptor(Namespace::Name*), SWIG_POINTER_OWN | %newpointer_flags);
 }
%typemap(directorout) Namespace::Name const& {
  // hack to get around swig's evil value wrapper being randomly used
  assign($result, SWIG_NewPointerObj(new Namespace::Name(*$input), $descriptor(Namespace::Name*), SWIG_POINTER_OWN | %newpointer_flags));
 }
%typemap(directorin) Namespace::Name const& {
  $input = SWIG_NewPointerObj(new Namespace::Name($1_name), $descriptor(Namespace::Name*), SWIG_POINTER_OWN | %newpointer_flags);
 }
%pythoncode %{
PluralName=list
_plural_types.append(#PluralName)
_value_types.append(#Name)
%}
%feature("valuewrapper") PluralName;
%enddef


%define RMF_SWIG_VALUE_INSTANCE(Namespace, Name, TemplateName, PluralName)
RMF_SWIG_VALUE_CHECKS(Namespace, Name, SWIGTYPE);
RMF_SWIG_VALUE_IMPL(Namespace, Name, TemplateName, PluralName, PluralName);
%enddef


%define RMF_SWIG_VALUE_BUILTIN(Namespace, Name, PluralName, Type)
%typemap(in) Namespace::Name const& = Type const &;
%typecheck(SWIG_TYPECHECK_POINTER) Namespace::Name const& = Type const &;
%typemap(in) Namespace::Name & {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
}
%typemap(in) Namespace::Name * {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
}
%typemap(directorin) Namespace::Name * {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
  }
//%typemap(out) Namespace::Name const& = SWIGTYPE const &;
%typemap(out) Namespace::Name & {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_non_const_ref;
}

/*%typemap(out) Namespace::Name * {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_pointer;
  }*/
%typemap(directorout) Namespace::Name & {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_non_const_ref;
}
/*%typemap(directorout) Namespace::Name * {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_pointer;
  }*/
RMF_SWIG_VALUE_IMPL(Namespace, Name, TemplateName, PluralName, PluralName);
%enddef

%define RMF_SWIG_VALUE(Namespace, Name, PluralName)
RMF_SWIG_VALUE_INSTANCE(Namespace, Name, Name, PluralName)
RMF_SWIG_SHOWABLE(Namespace, Name);
%enddef

 // a value that has implicit constructors
%define RMF_SWIG_VALUE_IMPLICIT(Namespace, Name, PluralName)
RMF_SWIG_VALUE_IMPL(Namespace, Name, Name, PluralName, PluralName);
RMF_SWIG_SHOWABLE(Namespace, Name);
%enddef

%define RMF_SWIG_VALUE_TEMPLATE(Namespace, Name)
RMF_SWIG_SHOWABLE(Namespace, Name);
%enddef




%define RMF_SWIG_NATIVE_VALUE(Name)

%typemap(in) Name & {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
}
%typemap(in) Name * {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
}
%typemap(directorin) Name * {
  values_like_##Name##_must_be_passed_by_value_or_const_ref_not_non_const_ref;
  }
//%typemap(out) Namespace::Name const& = SWIGTYPE const &;
/*%typemap(out) Name & {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_non_const_ref;
  }*/
/*%typemap(out) Name * {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_pointer;
  }*/
%typemap(directorout) Name & {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_non_const_ref;
}
/*%typemap(directorout) Name * {
  values_like_##Name##_must_be_returned_by_value_or_const_ref_not_pointer;
  }*/
%typemap(in) std::vector< Name > const& {
  try {
    // hack to get around swig's value wrapper being randomly used
    assign($1, ConvertSequence<std::vector< Name >, Convert< Name > >::get_cpp_object($input, $descriptor(Name*)));
  } catch (const RMF::Exception &e) {
    //PyErr_SetString(PyExc_ValueError,"Wrong type in sequence");
    PyErr_SetString(PyExc_TypeError, e.what());
    return NULL;
  }
 }
%typemap(freearg) std::vector< Name > const& {
  delete_if_pointer($1);
 }
%typecheck(SWIG_TYPECHECK_POINTER) std::vector< Name > const& {
  $1= ConvertSequence<std::vector< Name >, Convert< Name > >::get_is_cpp_object($input, $descriptor(Name*));
 }
%typemap(out) std::vector< Name > const& {
  $result = ConvertSequence<std::vector< Name >, Convert< Name > >::create_python_object(ValueOrObject<std::vector< Name > >::get($1), $descriptor(Name*), SWIG_POINTER_OWN);
 }
%typemap(directorout) std::vector< Name > const& {
  // hack to get around swig's evil value wrapper being randomly used
  assign($result, ConvertSequence<std::vector< Name >, Convert< Name > >::get_cpp_object($input, $descriptor(Name*)));
 }
%typemap(directorin) std::vector< Name > const& {
  $input = ConvertSequence<std::vector< Name >, Convert< Name > >::create_python_object($1_name, $descriptor(Name*), SWIG_POINTER_OWN);
 }

%typemap(in) std::vector< Name > {
  try {
    // hack to get around swig's value wrapper being randomly used
    assign($1, ConvertSequence<std::vector< Name >, Convert< Name > >::get_cpp_object($input, $descriptor(Name*)));
  } catch (const RMF::Exception &e) {
    //PyErr_SetString(PyExc_ValueError,"Wrong type in sequence");
    PyErr_SetString(PyExc_TypeError, e.what());
    return NULL;
  }
 }
%typemap(freearg) std::vector< Name > {
  delete_if_pointer($1);
 }
%typecheck(SWIG_TYPECHECK_POINTER) std::vector< Name > {
  $1= ConvertSequence<std::vector< Name >, Convert< Name > >::get_is_cpp_object($input, $descriptor(Name*));
 }
%typemap(out) std::vector< Name > {
  $result = ConvertSequence<std::vector< Name >, Convert< Name > >::create_python_object(ValueOrObject<std::vector< Name > >::get($1), $descriptor(Name*), SWIG_POINTER_OWN);
 }
%typemap(directorout) std::vector< Name > {
  // hack to get around swig's evil value wrapper being randomly used
  assign($result, ConvertSequence<std::vector< Name >, Convert< Name > >::get_cpp_object($input, $descriptor(Name*)));
 }
%typemap(directorin) std::vector< Name > {
  $input = ConvertSequence<std::vector< Name >, Convert< Name > >::create_python_object($1_name, $descriptor(Name*), SWIG_POINTER_OWN);
 }

%enddef





%define RMF_SWIG_RAII_INSTANCE(Namespace, Name, NiceName)
  %typemap(in) Namespace::Name* {
  BOOST_STATIC_ASSERT($argnum==1); // RAII object Namespace::Name cannot be passed as an argument
try {
  $1=ConvertRAII<Namespace::Name >::get_cpp_object($input, $descriptor(Namespace::Name*));
 } catch (const RMF::Exception &e) {
  //PyErr_SetString(PyExc_ValueError,"Wrong type in sequence");
  PyErr_SetString(PyExc_TypeError, e.what());
  return NULL;
  }
   }
%typemap(in) Namespace::Name {
 }
%typecheck(SWIG_TYPECHECK_POINTER) Namespace::Name * {
  $1= ConvertRAII<Namespace::Name >::get_is_cpp_object($input, $descriptor(Namespace::Name*));
 }
%typemap(out) Namespace::Name {
 }
%pythoncode %{
  _raii_types.append(#Name)
%}
%enddef


%define RMF_SWIG_RAII(Namespace, Name)
RMF_SWIG_RAII_INSTANCE(Namespace, Name, Name)
RMF_SWIG_SHOWABLE(Namespace, Name);
%enddef

%define RMF_SWIG_RAII_TEMPLATE(Namespace, Name)
RMF_SWIG_SHOWABLE(Namespace, Name);
%enddef



%define RMF_SWIG_PAIR(Namespace, Name, PairName, PluralName)
RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, Namespace::Name, PairName, const&);
RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, Namespace::Name, PairName,);
RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(Namespace::Name, Namespace::PairName, Namespace::PluralName, const&);
RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(Namespace::Name, Namespace::PairName, Namespace::PluralName,);
%feature("valuewrapper") PluralName;
%enddef

%define RMF_SWIG_NATIVE_PAIR(Namespace, Name, PairName, PluralName)
RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, Name, PairName, const&);
RMF_SWIG_SEQUENCE_TYPEMAP(Namespace, Name, PairName,);
RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(Name, Namespace::PairName, Namespace::PluralName, const&);
RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(Name, Namespace::PairName, Namespace::PluralName,);
%feature("valuewrapper") PluralName;
%enddef

%define RMF_SWIG_NATIVE_VALUES_LIST(Namespace, Name, PluralName, PluralListName)
RMF_SWIG_VALUE_CHECKS(Namespace, PluralListName, SWIGTYPE);
RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(Name, Namespace::PluralName, Namespace::PluralListName, const&);
RMF_SWIG_NESTED_SEQUENCE_TYPEMAP(Name, Namespace::PluralName, Namespace::PluralListName,);
%enddef



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
RMF_SWIG_VALUE_INSTANCE(RMF, Ucname##Key, Ucname##Key, Ucname##Keys);
%enddef

%define RMF_SWIG_DEFINE_TYPE(lcname, Ucname)
%template(_##Ucname##Traits) RMF::Traits<RMF::Ucname>;
%template(Ucname##Key) RMF::ID<RMF::Ucname##Traits>;
%enddef


%define RMF_SHADOW_NULLABLE(Class, function)
%feature("shadow") Class::function const %{
    def function(self, *args):
        return _handle_nullable( $action(self, *args))
%}
%enddef
