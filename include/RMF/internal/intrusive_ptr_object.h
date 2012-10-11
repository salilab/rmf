/*
// Copyright (c) 2006 Alexis Wilke and Doug Barbieri
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Find documentation on the home website:
//
// http://boost-extras.sourceforge.net/
// http://boost-extras.sourceforge.net/intrusive_ptr_object/intrusive_ptr_object.html


   Note: this is heavily edited from the freely available version as I
   could not get that one to work reliably and it introduced various
   overhead with dynamic casts and things.  */

#ifndef IMPLIBRMF_INTRUSIVE_PTR_OBJECT_H
#define IMPLIBRMF_INTRUSIVE_PTR_OBJECT_H

#include "../infrastructure_macros.h"
#include "../exceptions.h"
#include <boost/config.hpp>
// avoid thread overhead if not required/available
#if defined(BOOST_HAS_THREADS)
#include <boost/thread/mutex.hpp>
#endif
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/assert.hpp>


// place these in the boost namespace so the intrusive_ptr<> template
// finds the intrusive_ptr_add_ref() and intrusive_ptr_release() functions
// whatever the compiler. (see the boost::intrusive_ptr documentation)
//
// http://www.boost.org/libs/smart_ptr/intrusive_ptr.html
//
namespace boost
{




  // objects are derived last from this class; this way they
  // can be reference counted
  class intrusive_ptr_object
  {
  public:
    intrusive_ptr_object() throw()
    : m_ref_count(0)
    {
    }
    ~intrusive_ptr_object();

    void add_ref()
    {
#if defined(BOOST_HAS_THREADS)
      mutex::scoped_lock lock(m_mutex);
#endif
      ++m_ref_count;
    }
    bool release()
    {
#if defined(BOOST_HAS_THREADS)
      mutex::scoped_lock lock(m_mutex);
#endif
      // since users should not call release() themselves but use
      // the intrusive_ptr<> template instead, this should never happen
      BOOST_ASSERT(m_ref_count != 0);
      --m_ref_count;
      return m_ref_count==0;
    }

  private:
    unsigned int  m_ref_count;
#if defined(BOOST_HAS_THREADS)
    mutex    m_mutex;
#endif
  };


  inline intrusive_ptr_object::~intrusive_ptr_object()
  {
    // a stack object will be deleted without a release() call...
    // if you use objects properly, this should never happen!
    RMF_INTERNAL_CHECK(m_ref_count == 0, "Not 0 ref count");
  }

  // for clang++ move these to the actual types that use them
  /*template <class C>
  inline void intrusive_ptr_add_ref(C *a,
    typename boost::enable_if<boost::is_base_of<intrusive_ptr_object,
                                                C> >::type* =0)
  {
    (a)->add_ref();
  }


  template <class C>
  inline void intrusive_ptr_release(C *a,
   typename boost::enable_if<boost::is_base_of<intrusive_ptr_object,
                                               C> >::type* =0)
  {
    bool del=(a)->release();
    if (del) {
      delete a;
    }
    }*/

} // namespace boost
#include <boost/intrusive_ptr.hpp>

#endif /* IMPLIBRMF_INTRUSIVE_PTR_OBJECT_H */
