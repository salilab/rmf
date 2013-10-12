/**
 *  \file RMF/FileConstHandle.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_FILE_CONST_HANDLE_H
#define RMF_FILE_CONST_HANDLE_H

#include <RMF/config.h>
#include "internal/SharedData.h"
#include "Key.h"
#include "NodeConstHandle.h"
#include <boost/functional/hash.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/range/irange.hpp>

RMF_ENABLE_WARNINGS

#define RMF_FILE_CATCH(extra_info)                                        \
  catch (Exception& e) {                                                  \
    RMF_RETHROW(                                                          \
        File(get_path()) << Frame(get_current_frame())                    \
                         << Operation(BOOST_CURRENT_FUNCTION) extra_info, \
        e);                                                               \
  }

#define RMF_HDF5_ROOT_CONST_KEY_TYPE_METHODS(                                  \
    lcname, UCName, PassValue, ReturnValue, PassValues, ReturnValues)          \
  UCName##Key get_##lcname##_key(Category category_id, std::string nm) const { \
    return get_key<UCName##Traits>(category_id, nm);                           \
  }                                                                            \
  std::string get_name(UCName##Key k) const {                                  \
    try {                                                                      \
      return shared_->get_name(k);                                             \
    }                                                                          \
    RMF_FILE_CATCH();                                                          \
  }                                                                            \
  Category get_category(UCName##Key k) const {                                 \
    return shared_->get_category(k);                                           \
  }                                                                            \
  /** This returns all the keys that are used in the current frame.            \
      Other frames may have different ones.*/                                  \
  UCName##Key##s get_##lcname##_keys(Category category_id) {                   \
    return get_keys<UCName##Traits>(category_id);                              \
  }
RMF_VECTOR_DECL(FileConstHandle);

namespace RMF {

class NodeConstHandle;
class FileHandle;

//! A handle for a read-only RMF file
/** Use this handle to perform operations relevant to the
    whole RMF hierarchy as well as to start traversal of the
    hierarchy.
    \see open_rmf_file_read_only
 */
class RMFEXPORT FileConstHandle {
  void gather_ids(NodeConstHandle n, Ints& ids, std::vector<std::string>& paths,
                  std::string path) const;
#ifndef SWIG
  friend class NodeConstHandle;
  friend void clone_file_info(FileConstHandle, FileHandle);
  friend void clone_hierarchy(FileConstHandle, FileHandle);
  friend void clone_static_frame(FileConstHandle, FileHandle);
  friend void clone_loaded_frame(FileConstHandle, FileHandle);
  friend bool get_equal_current_values(FileConstHandle, FileConstHandle);
  friend bool get_equal_static_values(FileConstHandle, FileConstHandle);
#endif
  int compare(const FileConstHandle& o) const {
    if (get_name() < o.get_name())
      return -1;
    else if (get_name() > o.get_name())
      return 1;
    else
      return 0;
  }

 protected:
  boost::shared_ptr<internal::SharedData> shared_;

 public:
  RMF_COMPARISONS(FileConstHandle);
  RMF_HASHABLE(FileConstHandle, return boost::hash_value(get_name()););
  RMF_SHOWABLE(FileConstHandle, get_name());
  //! Empty root handle, no open file.
  FileConstHandle() {}
#if !defined(RMF_DOXYGEN) && !defined(SWIG)
  FileConstHandle(boost::shared_ptr<internal::SharedData> shared);
  FileConstHandle(std::string name);
#endif

  //! Return the root of the hierarchy
  NodeConstHandle get_root_node() const {
    return NodeConstHandle(NodeID(0), shared_);
  }

  std::string get_name() const { return shared_->get_file_name(); }

  std::string get_path() const { return shared_->get_file_path(); }

  /** \name Methods for manipulating keys
      When using C++ it is most convenient to specify types
      when adding and using keys through template arguments. For python
      we provide non-template versions, below.
      @{
   */
  /** Get an existing key that has the given name of the
      given type or Key() if the key is not found.
   */
  template <class TypeT>
  Key<TypeT> get_key(Category category, std::string name) const {
    try {
      return shared_->get_key(category, name, TypeT());
    }
    RMF_FILE_CATCH(<< Category(get_name(category)) << Key(name));
  }
  template <class TypeT>
  std::vector<Key<TypeT> > get_keys(Category category_id,
                                    const Strings& names) const {
    try {
      std::vector<Key<TypeT> > ret(names.size());
      for (unsigned int i = 0; i < names.size(); ++i) {
        ret[i] = get_key<TypeT>(category_id, names[i]);
        if (ret[i] == Key<TypeT>()) {
          ret.clear();
          return ret;
        }
      }
      return ret;
    }
    RMF_FILE_CATCH(<< Category(get_name(category_id)));
  }
  /** Get a list of all keys of the given type,
   */
  template <class TypeT>
  std::vector<Key<TypeT> > get_keys(Category category) const {
    try {
      if (category == Category()) return std::vector<Key<TypeT> >();
      return shared_->get_keys(category, TypeT());
    }
    RMF_FILE_CATCH(<< Category(get_name(category)));
  }
  /** @} */

  /** The file always has a single frame that is currently active at any given
      point.

      A value of ALL_FRAMES means one is only dealing with static data.

      @{
   */
  FrameID get_current_frame() const {
    if (shared_->get_current_is_static())
      return ALL_FRAMES;
    else
      return shared_->get_loaded_frame();
  }
  FrameType get_current_frame_type() const {
    return shared_->get_type(get_current_frame());
  }
  std::string get_current_frame_name() const {
    return shared_->get_name(get_current_frame());
  }
  void set_current_frame(FrameID frame) const {
    try {
      if (frame == ALL_FRAMES) {
        shared_->set_current_is_static(true);
      } else {
        shared_->set_loaded_frame(frame);
        shared_->set_current_is_static(false);
      }
    }
    RMF_FILE_CATCH(<< Frame(frame));
  }

  /** Return the number of frames in the file.
   */
  unsigned int get_number_of_frames() const {
    try {
      return shared_->get_number_of_frames();
    }
    RMF_FILE_CATCH();
  }

  /** Return a string identifying the file type.
  */
  std::string get_file_type() const { return shared_->get_file_type(); }

#ifndef SWIG
  boost::iterator_range<boost::range_detail::integer_iterator<FrameID> >
  get_frames() const {
    return boost::irange(FrameID(0), FrameID(get_number_of_frames()));
  }

  boost::iterator_range<boost::range_detail::integer_iterator<NodeID> >
  get_node_ids() const {
    return boost::irange(NodeID(0), NodeID(shared_->get_number_of_nodes()));
  }
#endif

  /** \name Non-template versions for python

      Type is one of the \ref rmf_types "standard types".
      @{
   */

  RMF_FOREACH_TYPE(RMF_HDF5_ROOT_CONST_KEY_TYPE_METHODS);

/** @} */
#ifdef RMF_DOXYGEN
  /** \name Python only
      The following methods are only available in python.
      @{
   */
  //! Return a list with all the keys from that category
  PythonList get_keys(Category c) const;
/** @} */
#endif
#ifndef SWIG
  /** Each node in the hierarchy can be associated with some arbitrary bit
      of external data. Nodes can be extracted using
      these bits of data.
   */
  template <class T>
  NodeConstHandle get_node_from_association(const T& d) const {
    if (!shared_->get_has_associated_node(d)) {
      return NodeConstHandle();
    } else {
      return NodeConstHandle(shared_->get_associated_node(d), shared_);
    }
  }
#else
  NodeConstHandle get_node_from_association(void* v) const;
#endif
  NodeConstHandle get_node(NodeID id) const;

  /** Along with the associations for nodes, arbitrary data can
      be associated with the file in memory to aid in processing.
   */
  template <class T>
  void add_associated_data(int index, const T& t) {
    shared_->set_user_data(index, t);
  }
  /** To get back the ith user data.*/
  template <class T>
  T get_associated_data(int index) {
    return shared_->get_user_data<T>(index);
  }

  /** To get back the ith user data.*/
  bool get_has_associated_data(int index) {
    return shared_->get_has_user_data(index);
  }

  /** Each RMF structure has an associated description. This should
      consist of unstructured text describing the contents of the RMF
      data. Conventionally. this description can consist of multiple
      paragraphs, each separated by a newline character and should end
      in a newline.
   */
  std::string get_description() const;

  /** Each RMF structure has an associated field that the code that
      produced the file can use to describe itself.
   */
  std::string get_producer() const;

  /** \name Key categories methods
      Methods for managing the key categories in this RMF.
      @{
   */
  Category get_category(std::string name) {
    try {
      return shared_->get_category(name);
    }
    RMF_FILE_CATCH(<< Category(name));
  }
  /** This returns all the categories that are used in the current frame.
      Other frames may have different ones.*/
  Categories get_categories() const {
    try {
      return shared_->get_categories();
    }
    RMF_FILE_CATCH();
  }
  std::string get_name(Category kc) const {
    try {
      return shared_->get_name(kc);
    }
    RMF_FILE_CATCH();
  }
  /** @} */

  /** Run the various validators that attempt to check that the RMF file
      is correct. Print messages to the provided stream if errors are
      encounted.*/
  void validate(std::ostream& out);

  /** Like validate(std::ostream&) except it returns a sting describing
      the errors.*/
  std::string validate();

  /** Reread the file.
      \note This may invalidate various thing (eg the number of nodes may
      vary). Be careful.
   */
  void reload();
};

/**
   Open an RMF from a file system path in read-only mode.

   \param path the system path to the rmf file
   \exception RMF::IOException couldn't open file, or unsupported file format
 */
RMFEXPORT FileConstHandle open_rmf_file_read_only(std::string path);

/**
   Open an RMF from a buffer in read-only mode.

   \param buffer a buffer containing an RMF
   \exception RMF::IOException couldn't parse the buffer,
   or unsupported file format
 */
RMFEXPORT FileConstHandle open_rmf_buffer_read_only(const std::string& buffer);

/** \name Batch data access
    These methods provide batch access to attribute data to try
    to reduce the overhead of repeated function calls.

    The missing_value argument is a placeholder that can fill in
    for values which are not found in the respective node.

    \note These methods are experimental and subject to change.
    @{
 */
RMFEXPORT Floats
    get_values(const NodeConstHandles& nodes, FloatKey k,
               Float missing_value = std::numeric_limits<double>::max());
/** @} */

} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_FILE_CONST_HANDLE_H */
