/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SHARED_DATA_H
#define RMF_INTERNAL_SHARED_DATA_H

#include <RMF/config.h>
#include "../Key.h"
#include "../types.h"
#include "../names.h"
#include "../enums.h"
#include "../constants.h"
#include "../ID.h"
#include "../infrastructure_macros.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/cstdint.hpp>
#include <boost/any.hpp>
#include <algorithm>
#include <boost/range/irange.hpp>

#include <boost/shared_ptr.hpp>

RMF_ENABLE_WARNINGS namespace RMF {
  template <class P>
  inline uintptr_t get_uint(const P * p) {
    return reinterpret_cast<uintptr_t>(p);
  }
  template <class P>
  inline uintptr_t get_uint(boost::shared_ptr<P> p) {
    return reinterpret_cast<uintptr_t>(p.get());
  }
  inline uintptr_t get_uint(NodeID id) { return id.get_index(); }

  namespace internal {

#define RMF_SHARED_TYPE(lcname, Ucname, PassValue, ReturnValue, PassValues, \
                        ReturnValues)                                       \
  /** Return a value or the null value.*/                                   \
  virtual Ucname##Traits::ReturnType get_loaded_value(                      \
      NodeID node, Key<Ucname##Traits> k) const = 0;                        \
  /** Return a value or the null value.*/                                   \
  virtual Ucname##Traits::ReturnType get_static_value(                      \
      NodeID node, Key<Ucname##Traits> k) const = 0;                        \
  virtual void set_loaded_value(NodeID node, Key<Ucname##Traits> k,         \
                                Ucname##Traits::Type v) = 0;                \
  virtual void set_static_value(NodeID node, Key<Ucname##Traits> k,         \
                                Ucname##Traits::Type v) = 0;                \
  virtual std::vector<Key<Ucname##Traits> > get_keys(Category category,     \
                                                     Ucname##Traits) = 0;   \
  virtual Category get_category(Key<Ucname##Traits> k) const = 0;           \
  virtual Key<Ucname##Traits> get_key(Category category, std::string name,  \
                                      Ucname##Traits) = 0;                  \
  virtual std::string get_name(Key<Ucname##Traits> k) const = 0

  /**
     Base class for wrapping all the file handles, caches, etc. for
     open RMF file handles, and to manage the associations between
     external objects and nodes in the RMF hierarchy

     Note this class serves as an internal interface to RMS file handling
     with an almost one-to-one mapping between most of its functions and
     exposed functions
   */
  class SharedData {
    std::vector<boost::any> association_;
    std::vector<uintptr_t> back_association_value_;
    boost::unordered_map<uintptr_t, NodeID> back_association_;
    boost::unordered_map<int, boost::any> user_data_;
    int valid_;
    FrameID loaded_frame_;
    bool cur_is_static_;
    std::string path_;

   protected:
    SharedData(std::string path);

   public:
    std::string get_file_path() const { return path_; }
    FrameID get_loaded_frame() const { return loaded_frame_; }
    bool get_current_is_static() const { return cur_is_static_; }
    virtual void set_loaded_frame(FrameID frame) {
      loaded_frame_ = frame;
    }
    void set_current_is_static(bool tf) {
      cur_is_static_ = tf;
    }
    FrameID get_current_frame() const {
      return get_current_is_static() ? RMF::ALL_FRAMES : loaded_frame_;
    }

    RMF_FOREACH_TYPE(RMF_SHARED_TYPE);
    void audit_key_name(std::string name) const;
    void audit_node_name(std::string name) const;
    template <class T>
    void set_user_data(int i, const T& d) {
      user_data_[i] = boost::any(d);
    }
    bool get_has_user_data(int i) const {
      return user_data_.find(i) != user_data_.end();
    }
    template <class T>
    T get_user_data(int i) const {
      RMF_USAGE_CHECK(user_data_.find(i) != user_data_.end(),
                      "No such data found");
      try {
        return boost::any_cast<T>(user_data_.find(i)->second);
      }
      catch (boost::bad_any_cast) {
        RMF_THROW(Message("Type mismatch when recovering user data"),
                  UsageException);
      }
      RMF_NO_RETURN(T);
    }
    template <class T>
    void set_association(NodeID nid, const T& d, bool overwrite) {
      int id = nid.get_index();
      if (association_.size() <= static_cast<unsigned int>(id)) {
        association_.resize(id + 1, boost::any());
        back_association_value_.resize(id + 1);
      }
      RMF_USAGE_CHECK(overwrite || association_[id].empty(),
                      "Associations can only be set once");
      if (overwrite && !association_[id].empty()) {
        uintptr_t v = back_association_value_[id];
        back_association_.erase(v);
      }
      uintptr_t v = get_uint(d);
      back_association_value_[id] = v;
      association_[id] = boost::any(d);
      RMF_USAGE_CHECK(back_association_.find(v) == back_association_.end(),
                      "Collision on association keys.");
      back_association_[v] = nid;
    }
    template <class T>
    bool get_has_associated_node(const T& v) const {
      return back_association_.find(get_uint(v)) != back_association_.end();
    }
    boost::any get_association(NodeID nid) const {
      int id = nid.get_index();
      RMF_USAGE_CHECK(static_cast<unsigned int>(id) < association_.size(),
                      std::string("Unassociated id ") + get_name(nid));
      try {
        return association_[id];
      }
      catch (boost::bad_any_cast) {
        RMF_THROW(Message("Type mismatch when recovering node data"),
                  UsageException);
      }
      RMF_NO_RETURN(boost::any);
    }
    bool get_has_association(NodeID nid) const {
      int id = nid.get_index();
      if (id >= static_cast<int>(association_.size())) return false;
      return !association_[id].empty();
    }
    template <class T>
    NodeID get_associated_node(const T& d) const {
      return back_association_.find(get_uint(d))->second;
    }
    virtual void flush() = 0;
    std::string get_file_name() const;

    virtual unsigned int get_number_of_frames() const = 0;
    virtual unsigned int get_number_of_nodes() const = 0;

    // SharedData(HDF5Group g, bool create);
    virtual ~SharedData();
    virtual std::string get_name(NodeID node) const = 0;
    virtual NodeType get_type(NodeID node) const = 0;

    virtual NodeID add_child(NodeID node, std::string name, NodeType t) = 0;
    virtual void add_child(NodeID node, NodeID child_node) = 0;
    virtual NodeIDs get_children(NodeID node) const = 0;

    virtual FrameID add_frame(std::string name, FrameType t) = 0;
    virtual void add_child_frame(FrameID child_node) = 0;
    virtual FrameIDs get_children(FrameID node) const = 0;

    virtual Categories get_categories() const = 0;
    virtual Category get_category(std::string name) = 0;
    virtual std::string get_name(Category kc) const = 0;
    virtual std::string get_description() const = 0;
    virtual void set_description(std::string str) = 0;
    virtual std::string get_producer() const = 0;
    virtual void set_producer(std::string str) = 0;
    virtual std::string get_loaded_frame_name() const = 0;
    virtual FrameType get_loaded_frame_type() const = 0;
    virtual std::string get_file_type() const = 0;
    virtual void reload() = 0;
    boost::iterator_range<boost::range_detail::integer_iterator<FrameID> >
    get_frames() const {
      return boost::irange(FrameID(0), FrameID(get_number_of_frames()));
    }

    boost::iterator_range<boost::range_detail::integer_iterator<NodeID> >
    get_nodes() const {
      return boost::irange(NodeID(0), NodeID(get_number_of_nodes()));
    }
  };

  /**
     Construct shared data for the RMF file in 'path', either creating or
     the file or opening an existing file according to the value of 'create'.
     Note on internal implementation - stores results in internal cache

     @param path path to RMF file
     @param create whether to create the file or just open it
     @exception IOException if couldn't create file or unsupported file format
   */
  RMFEXPORT boost::shared_ptr<SharedData> create_shared_data(std::string path,
                                                             bool create);

  /**
     Construct shared data for the RMF file in 'path' in read only mode
     Note on internal implementation - stores results in internal cache

     @param path path to RMF file
     @param create whether to create the file or just open it
     @exception RMF::IOException if couldn't open file or unsupported file
                format
   */
  RMFEXPORT boost::shared_ptr<SharedData> create_read_only_shared_data(
      std::string path);

  RMFEXPORT boost::shared_ptr<SharedData> create_shared_data_in_buffer(
      std::string& buffer, bool create);

  RMFEXPORT boost::shared_ptr<SharedData>
      create_read_only_shared_data_from_buffer(const std::string& buffer);

  }  // namespace internal
} /* namespace RMF */

#define RMF_BACKEND_VIRTUAL virtual
#define RMF_BACKEND_OVERRIDE RMF_OVERRIDE

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_H */
