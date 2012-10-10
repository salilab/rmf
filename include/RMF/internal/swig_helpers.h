/**
 *  \file internal/swig_helpers.h
 *  \brief Functions for use in swig wrappers
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPLIBRMF_INTERNAL_SWIG_HELPERS_H
#define IMPLIBRMF_INTERNAL_SWIG_HELPERS_H

// Python.h must be included first
#include <RMF/config.h>
#include "../infrastructure_macros.h"
#include <boost/array.hpp>
#include <vector>
#include <stdexcept>

#ifndef SWIG
  template <bool REFED>
  struct PyPointer: boost::noncopyable {
    PyObject* ptr_;
    PyPointer(PyObject* ptr): ptr_(ptr){
      IMP_RMF_INTERNAL_CHECK(ptr, "nullptr pointer passed");
      if (!REFED) {
        Py_INCREF(ptr_);
      } else {
        IMP_RMF_INTERNAL_CHECK(ptr_->ob_refcnt >=1, "No refcount");
      }
    }
    operator PyObject*() const{
      return ptr_;
    }
    PyObject* operator->() const {
      return ptr_;
    }
    PyObject *release() {
      IMP_RMF_INTERNAL_CHECK(ptr_->ob_refcnt >=1, "No refcount");
      PyObject *ret=ptr_;
      ptr_=NULL;
      return ret;
    }
    ~PyPointer() {
      if (ptr_) {
        Py_DECREF(ptr_);
      }
    }
  };
  typedef PyPointer<true> PyReceivePointer;
  typedef PyPointer<false> PyOwnerPointer;

#define IMP_RMF_PYTHON_CALL(call) {int rc=call;                 \
    if (rc != 0) {                                              \
      IMP_RMF_INTERNAL_CHECK(0, RMF::internal::get_error_message\
                             ("Python call failed: ", #call,    \
                              " with ", rc));                   \
    }                                                           \
  }

  using boost::enable_if;
  using boost::mpl::and_;
  using boost::mpl::not_;
  using boost::is_convertible;
  using boost::is_base_of;
  using boost::is_pointer;
  //using namespace boost;
  //using namespace boost::mpl;

  template <class V>
  void assign(V* &a, const V &b) {
    a= new V(b);
  }
  template <class V>
  void assign(V &a, const V &b) {
    a= b;
  }
  template <class V>
  void assign(SwigValueWrapper<V> &a, const V &b) {
    a= b;
  }
  template <class T>
  void delete_if_pointer(T&t) {
    t=T();
  }
  template <class T>
  void delete_if_pointer(T*&t) {
    if (t) {
      *t=T();
      delete t;
    }
  }
  template <class T>
  void delete_if_pointer(SwigValueWrapper<T> &v) {
  }


  /*
    Handle assignment into a container. Swig always provides the
    values by reference so we need to determine if the container wants
    them by value or reference and dereference if necessary.
  */
  template <class Container, class Value>
  struct Assign {
    static void assign(Container &c, unsigned int i, Value *v) {
      c[i]=*v;
    }
    static void assign(Container &c, unsigned int i, const Value &v) {
      c[i]=v;
    }

  };


  /*
    Return a reference to a value-type and a pointer to an
    object-type. Also, return what the storage type is (since we get a
    pointer for objects, but just want the object type).
  */

  template <class T, class Enabled=void>
  struct ValueOrObject {
    static const T& get(const T&t) {return t;}
    static const T& get(const T*t) {return *t;}
    typedef T type;
    typedef T store_type;
  };

  template <class T>
  struct ConvertAllBase {
    BOOST_STATIC_ASSERT(!is_pointer<T>::value);
    template <class SwigData>
    static bool get_is_cpp_object(PyObject *o, SwigData st,
                                  SwigData particle_st, SwigData decorator_st) {
      void *vp;
      int res= SWIG_ConvertPtr(o, &vp,st, 0 );
      return SWIG_IsOK(res) &&  vp;
    }
  };


  template <class T>
  struct ConvertValueBase: public ConvertAllBase<T> {
    BOOST_STATIC_ASSERT(!is_pointer<T>::value);
    template <class SwigData>
    static const T& get_cpp_object(PyObject *o,
                                   SwigData st, SwigData, SwigData) {
      void *vp;
      int res=SWIG_ConvertPtr(o, &vp,st, 0 );
      if (!SWIG_IsOK(res)) {
        IMP_RMF_THROW( "Wrong type.", std::runtime_error);
      }
      if (!vp) {
        IMP_RMF_THROW( "Wrong type.", std::runtime_error);
      }
      return *reinterpret_cast<T*>(vp);
    }
    template <class SwigData>
    static PyObject* create_python_object(T t, SwigData st, int OWN) {
      PyReceivePointer o(SWIG_NewPointerObj(new T(t), st, OWN));
      return o.release();
    }
  };



  /*
    Provide support for converting from python objects to the appropriate
    C++ object.
    - get_cpp_object takes a python object and returns a pointer to the
    needed C++ object
    - get_is_cpp_object checks if the python object can be converted to
    the needed C++ object
    - create_python_object takes the C++ object and creates a python object

    Decorators are a special case in order to provide the implicit conversion
    from Decorator to Particle* (so their swig type info is passed around
    even when it is not needed).
  */
  template <class T, class Enabled=void>
  struct Convert: public ConvertValueBase<T > {
    static const int converter=0;
  };



  /*
    Sequences are anything which gets converted to/from a python sequence.
    These all result in more than one layer of python objects being created
    instead of a single one as in the above cases.
  */
  template <class T, class VT, class ConvertVT>
  struct ConvertSequenceHelper {
    typedef typename ValueOrObject< VT >::type V;
    BOOST_STATIC_ASSERT(!is_pointer<T>::value);
    template <class SwigData>
    static bool get_is_cpp_object(PyObject *in, SwigData st,
                                  SwigData particle_st, SwigData decorator_st) {
      if (!in || !PySequence_Check(in)) {
        return false;
      }
      for (unsigned int i=0; i< PySequence_Length(in); ++i) {
        PyReceivePointer o(PySequence_GetItem(in,i));
        if(! ConvertVT::get_is_cpp_object(o, st, particle_st,
                                           decorator_st)) {
          return false;
        }
      }
      return true;
    }
    template <class SwigData, class C>
    static void fill(PyObject *in, SwigData st,
                     SwigData particle_st, SwigData decorator_st, C&t) {
      if (!in || !PySequence_Check(in)) {
        PyErr_SetString(PyExc_ValueError,"Expected a sequence");
      }
      unsigned int l= PySequence_Size(in);
      IMP_RMF_INTERNAL_CHECK(in->ob_refcnt >0, "Freed sequence object found");
      for (unsigned int i=0; i< l; ++i) {
        PyReceivePointer o(PySequence_GetItem(in,i));
        typename ValueOrObject<V>::store_type vs
          =ConvertVT::get_cpp_object(o,st,
                                      particle_st, decorator_st);
        Assign<C, VT>::assign(t, i, vs);
      }
    }
  };

  template <class T, class ConvertValue, class Enabled=void>
  struct ConvertSequence {

  };

  template <class T, class ConvertT>
  struct ConvertSequence<T, ConvertT, typename enable_if< is_base_of<
       boost::array<typename T::value_type, T::static_size>, T> >::type > {
    static const int converter=5;
    typedef ConvertSequenceHelper<T, typename T::value_type, ConvertT> Helper;
    typedef typename ValueOrObject< typename T::value_type >::type VT;
    template <class SwigData>
    static T get_cpp_object(PyObject *o, SwigData st,
                            SwigData particle_st, SwigData decorator_st) {
      if (!get_is_cpp_object(o, st, particle_st, decorator_st)) {
        IMP_RMF_THROW("Argument not of correct type", std::runtime_error);
      }
      T ret;
      Helper::fill(o, st, particle_st, decorator_st, ret);
      return ret;
    }
    template <class SwigData>
    static bool get_is_cpp_object(PyObject *in, SwigData st,
                                  SwigData particle_st, SwigData decorator_st) {
      if (!Helper::get_is_cpp_object(in, st,
                                     particle_st, decorator_st)) return false;
      else return PySequence_Size(in) ==T::static_size;
    }
    template <class SwigData>
    static PyObject* create_python_object(const T& t, SwigData st, int OWN) {
      PyReceivePointer ret(PyTuple_New(T::static_size));
      for (unsigned int i=0; i< T::static_size; ++i) {
        PyReceivePointer o(Convert<VT>::create_python_object(t[i], st, OWN));
        IMP_RMF_PYTHON_CALL(PyTuple_SetItem(ret,i,o.release()));
      }
      return ret.release();
    }
  };


  // use an array as an intermediate since pair is not a sequence
  template <class T, class ConvertT>
  struct ConvertSequence<std::pair<T,T>, ConvertT > {
    static const int converter=6;
    typedef boost::array<T,2> Intermediate;
    typedef ConvertSequenceHelper<Intermediate, T, ConvertT > Helper;
    typedef typename ValueOrObject< T >::type VT;
    template <class SwigData>
    static std::pair<T,T> get_cpp_object(PyObject *o,
                                         SwigData st,
                                         SwigData particle_st,
                                         SwigData decorator_st) {
      if (!get_is_cpp_object(o, st, particle_st, decorator_st)) {
        IMP_RMF_THROW("Argument not of correct type", std::runtime_error);
      }
      Intermediate im;
      Helper::fill(o, st, particle_st, decorator_st, im);
      std::pair<T,T> ret;
      ret.first=im[0];
      ret.second=im[1];
      return ret;
    }
    template <class SwigData>
    static bool get_is_cpp_object(PyObject *in, SwigData st,
                           SwigData particle_st, SwigData decorator_st) {
      if (!Helper::get_is_cpp_object(in, st, particle_st,
                                     decorator_st)) return false;
      else return PySequence_Size(in) == 2;
    }
    template <class SwigData>
    static PyObject* create_python_object(const std::pair<T,T>& t,
                                          SwigData st, int OWN) {
      PyReceivePointer ret(PyTuple_New(2));
      PyReceivePointer of(Convert<VT>::create_python_object(t.first, st, OWN));
      IMP_RMF_PYTHON_CALL(PyTuple_SetItem(ret,0,of.release()));
      PyReceivePointer os(Convert<VT>::create_python_object(t.second, st, OWN));
      IMP_RMF_PYTHON_CALL(PyTuple_SetItem(ret,1,os.release()));
      return ret.release();
    }
  };


  template <class T, class ConvertT>
  struct ConvertVectorBase {
    typedef ConvertSequenceHelper<T, typename T::value_type, ConvertT> Helper;
    typedef typename ValueOrObject< typename T::value_type >::type VT;
    template <class SwigData>
    static T get_cpp_object(PyObject *o, SwigData st,
                            SwigData particle_st, SwigData decorator_st) {
      if (!get_is_cpp_object(o, st, particle_st, decorator_st)) {
        IMP_RMF_THROW("Argument not of correct type", std::runtime_error);
      }
      T ret(PySequence_Size(o));
      Helper::fill(o, st, particle_st, decorator_st, ret);
      return ret;
    }
    template <class SwigData>
    static bool get_is_cpp_object(PyObject *in, SwigData st,
                                  SwigData particle_st, SwigData decorator_st) {
      return Helper::get_is_cpp_object(in, st, particle_st, decorator_st);
    }
    template <class SwigData>
    static PyObject* create_python_object(const T& t, SwigData st, int OWN) {
      PyReceivePointer ret(PyList_New(t.size()));
      for (unsigned int i=0; i< t.size(); ++i) {
        PyReceivePointer o(ConvertT::create_python_object(t[i], st, OWN));
        // this does not increment the ref count
        IMP_RMF_PYTHON_CALL(PyList_SetItem(ret, i, o.release()));
      }
      return ret.release();
    }
  };



  template <class T, class ConvertT>
  struct ConvertSequence< RMF::vector<T>, ConvertT > :
    public ConvertVectorBase<RMF::vector<T>, ConvertT> {
    static const int converter=7;
  };

  template <>
  struct Convert<std::string> {
    static const int converter=10;
    template <class SwigData>
    static std::string get_cpp_object(PyObject *o, SwigData st,
                                      SwigData particle_st,
                                      SwigData decorator_st) {
      if (!o || !PyString_Check(o)) {
        IMP_RMF_THROW("Not all objects in list have correct type.",
                  std::runtime_error);
      } else {
        return std::string(PyString_AsString(o));
      }
    }
    template <class SwigData>
    static bool get_is_cpp_object(PyObject *o, SwigData st,
                                  SwigData particle_st, SwigData decorator_st) {
      return PyString_Check(o);
    }
    template <class SwigData>
    static PyObject* create_python_object(std::string f, SwigData st, int OWN) {
      return PyString_FromString(f.c_str());
    }
  };

  struct ConvertFloatBase {
    template <class SwigData>
    static double get_cpp_object(PyObject *o, SwigData st,
                                 SwigData particle_st, SwigData decorator_st) {
      if (!o || !PyNumber_Check(o)) {
        IMP_RMF_THROW("Not all objects in list have correct type.",
                  std::runtime_error);
      } else {
        return PyFloat_AsDouble(o);
      }
    }
    template <class SwigData>
    static bool get_is_cpp_object(PyObject *o, SwigData st,
                                  SwigData particle_st, SwigData decorator_st) {
      return PyNumber_Check(o);
    }
    template <class SwigData>
    static PyObject* create_python_object(float f, SwigData st, int OWN) {
      // these may or may not have a refcount
      return PyFloat_FromDouble(f);
    }
  };





  template <>
  struct Convert<float>: public ConvertFloatBase {
    static const int converter=11;
  };
  template <>
  struct Convert<double>: public ConvertFloatBase {
    static const int converter=12;
  };

  template <>
  struct Convert<int> {
    static const int converter=13;
    template <class SwigData>
    static int get_cpp_object(PyObject *o, SwigData st,
                              SwigData particle_st, SwigData decorator_st) {
      if (!PyInt_Check(o)) {
        IMP_RMF_THROW("Not all objects in list have correct number type.",
                  std::runtime_error);
      } else {
        return PyInt_AsLong(o);
      }
    }
    template <class SwigData>
    static bool get_is_cpp_object(PyObject *o, SwigData st,
                                  SwigData particle_st, SwigData decorator_st) {
      return PyInt_Check(o);
    }
    template <class SwigData>
    static PyObject* create_python_object(int f, SwigData st, int OWN) {
      // These may or may not have a ref count
      return PyInt_FromLong(f);
    }
  };



#endif


#endif  /* IMPLIBRMF_INTERNAL_SWIG_HELPERS_H */