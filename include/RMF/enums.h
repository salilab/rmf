/**
 *  \file RMF/enums.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_ENUMS_H
#define RMF_ENUMS_H

#include <RMF/config.h>
#include <string>
#include <iostream>

RMF_ENABLE_WARNINGS

namespace RMF {
//! The types of the nodes.
enum NodeType {
  //! The root node
  ROOT,
  //! Represent part of a molecule
  REPRESENTATION,
  //! Store a geometric object
  GEOMETRY,
  //! Store information about some feature of the system
  /** For example, the particles involved in scoring
      functions and their score can be encoded as feature
      nodes.
   */
  FEATURE,
  /** Store a reference to another node. This node should
      be an alias decorator node and have no other data,
      at least for now. Aliases should be thought of as simply referencing
      existing objects in the scene, not creating new objects.
   */
  ALIAS,
  //! Arbitrary data that is not standardized
  /** Programs can use these keys to store any extra data
      they want to put into the file.
   */
  CUSTOM,
  //! A link between two atoms
  /** These are mostly for display purposes eg to show a wireframe
      view of the molecule. */
  BOND,
  //! A node that is purely there for organizational purposes
  /** This includes nodes that are just RMF::ReferenceFrame nodes.
   */
  ORGANIZATIONAL,
#ifndef RMF_DOXYGEN
  //! An internal link to another node
  LINK
#endif
};


/** Return a string version of the type name.*/
RMFEXPORT std::string get_type_name(NodeType t);

#if !defined(RMF_DOXYGEN) && !defined(SWIG)
RMFEXPORT std::ostream& operator<<(std::ostream& out, NodeType t);
RMFEXPORT std::istream& operator>>(std::istream& in, NodeType& t);
#endif


//! The types of the frames.
enum FrameType {
  //! The root frame
  STATIC,
  //! A frame in a sequence of frames
  FRAME,
  //! An independent model
  MODEL,
  //! A cluster center
  CENTER,
  //! An alias for another frame
  FRAME_ALIAS
};

/** Return a string version of the type name.*/
RMFEXPORT std::string get_frame_type_name(FrameType t);

#if !defined(RMF_DOXYGEN) && !defined(SWIG)
RMFEXPORT std::ostream& operator<<(std::ostream& out, FrameType t);
RMFEXPORT std::istream& operator>>(std::istream& in, FrameType& t);
#endif


} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_ENUMS_H */
