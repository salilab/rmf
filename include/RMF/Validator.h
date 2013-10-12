/**
 *  \file RMF/Validator.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_VALIDATOR_H
#define RMF_VALIDATOR_H

#include <RMF/config.h>
#include "FileHandle.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

RMF_ENABLE_WARNINGS namespace RMF {
  /** Validators check invariants of the RMF hierarchy. Use the
      RMF_VALIDATOR() macro to register them. They should write
      information about any errors encountered to the provided
      stream.
   */
  class RMFEXPORT Validator {
    std::string name_;
    FileConstHandle rh_;

   public:
    /** This method is called with the root handle when
        validation is required.
     */
    virtual void write_errors(std::ostream& out) const = 0;
    virtual ~Validator();

   protected:
    FileConstHandle get_file() const { return rh_; }
    Validator(FileConstHandle rh, std::string name);
  };

  class RMFEXPORT NodeValidator : public Validator {
   public:
    /** This method traverses the node and calls
        validate_node() on each.
     */
    void write_errors(std::ostream& out) const;

   protected:
    NodeValidator(FileConstHandle rh, std::string name);
    virtual void write_errors_node(NodeConstHandle node,
                                   const NodeConstHandles& path,
                                   std::ostream& out) const = 0;
  };

#if !defined(RMF_DOXYGEN) && !defined(SWIG)
  struct Creator {
    std::string name_;
    Creator(std::string name) : name_(name) {}
    virtual Validator* create(FileConstHandle rh) = 0;
    virtual ~Creator() {}
  };
  template <class V>
  struct CreatorImpl : public Creator {
    CreatorImpl(std::string name) : Creator(name) {}
    Validator* create(FileConstHandle rh) { return new V(rh, name_); }
  };

  typedef std::vector<boost::shared_ptr<Creator> > Creators;
  RMFEXPORT Creators& get_validators();

  template <class V>
  struct Registrar {
    Registrar(std::string name) {
      get_validators().push_back(boost::make_shared<CreatorImpl<V> >(name));
    }
  };
#endif

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_VALIDATOR_H */
