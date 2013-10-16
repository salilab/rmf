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
#include "SharedDataUserData.h"
#include "SharedDataPath.h"
#include "SharedDataData.h"
#include "SharedDataFile.h"
#include "SharedDataCategory.h"
#include "SharedDataKeys.h"
#include "SharedDataHierarchy.h"
#include "SharedDataLoadedFrame.h"
#include "../Key.h"
#include "../types.h"
#include "../names.h"
#include "../enums.h"
#include "../ID.h"
#include "../constants.h"
#include "../infrastructure_macros.h"
#include <boost/range/irange.hpp>
#include <boost/shared_ptr.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {

namespace backends {
struct IO;
}

#define RMF_HOIST(lcname, UCName, PassValue, ReturnValue, PassValues, \
                  ReturnValues)                                       \
  using SharedDataData<UCName##Traits>::get_static_value;             \
  using SharedDataData<UCName##Traits>::set_static_value;             \
  using SharedDataData<UCName##Traits>::clear_static_values;          \
  using SharedDataData<UCName##Traits>::get_loaded_value;             \
  using SharedDataData<UCName##Traits>::set_loaded_value;             \
  using SharedDataData<UCName##Traits>::clear_loaded_values;          \
  using SharedDataKeys<UCName##Traits>::get_key;                      \
  using SharedDataKeys<UCName##Traits>::get_name;                     \
  using SharedDataKeys<UCName##Traits>::get_keys;                     \
  using SharedDataKeys<UCName##Traits>::get_category;

namespace internal {

class RMFEXPORT SharedData : public SharedDataUserData,
                             public SharedDataPath,
                             public SharedDataFile,
                             public SharedDataHierarchy,
                             public SharedDataCategory,
                             public SharedDataLoadedFrame,
                             public SharedDataKeys<IntTraits>,
                             public SharedDataKeys<StringTraits>,
                             public SharedDataKeys<FloatTraits>,
                             public SharedDataKeys<IndexTraits>,
                             public SharedDataKeys<NodeIDTraits>,
                             public SharedDataKeys<IntsTraits>,
                             public SharedDataKeys<StringsTraits>,
                             public SharedDataKeys<FloatsTraits>,
                             public SharedDataKeys<IndexesTraits>,
                             public SharedDataKeys<NodeIDsTraits>,
                             public SharedDataData<IntTraits>,
                             public SharedDataData<StringTraits>,
                             public SharedDataData<FloatTraits>,
                             public SharedDataData<IndexTraits>,
                             public SharedDataData<NodeIDTraits>,
                             public SharedDataData<IntsTraits>,
                             public SharedDataData<StringsTraits>,
                             public SharedDataData<FloatsTraits>,
                             public SharedDataData<IndexesTraits>,
                             public SharedDataData<NodeIDsTraits> {
  unsigned int number_of_frames_;
  bool write_, created_;
  boost::shared_ptr<backends::IO> io_;

  bool get_static_is_dirty() const;
  void set_static_is_dirty(bool tf);
  void clear_loaded_values();
  void clear_static_values();

  void save_static_frame();
  void save_loaded_frame();
  void load_static_frame();
  void load_loaded_frame();

 public:
  using SharedDataHierarchy::get_name;
  using SharedDataCategory::get_name;
  using SharedDataHierarchy::set_name;
  using SharedDataHierarchy::set_type;

  RMF_FOREACH_TYPE(RMF_HOIST);

  SharedData(boost::shared_ptr<backends::IO> io, std::string name, bool write,
             bool created);
  Category get_category(std::string name);
  void set_loaded_frame(FrameID frame);
  FrameID add_frame(std::string name, FrameType type);
  void flush();
  void reload();
  unsigned int get_number_of_frames() const { return number_of_frames_; }
  void set_number_of_frames(unsigned int n) { number_of_frames_ = n; }
  std::vector<char> get_buffer();
  ~SharedData();
};

}  // namespace internal
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_SHARED_DATA_H */
