/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF_INTERNAL_SINGLE_AVRO_FILE_READER_H
#define RMF_INTERNAL_SINGLE_AVRO_FILE_READER_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include <RMF/infrastructure_macros.h>
#include "MultipleAvroFileBase.h"
#include <backend/avro/AvroCpp/api/DataFile.hh>
#include "AllJSON.h"
#include "FrameJSON.h"
#include <boost/shared_ptr.hpp>

RMF_ENABLE_WARNINGS namespace RMF {
  namespace avro_backend {

  class MultipleAvroFileReader : public MultipleAvroFileBase {
    typedef MultipleAvroFileBase P;
    struct CategoryData {
      boost::shared_ptr<rmf_avro::DataFileReader<RMF_avro_backend::Data> >
          reader;
      // frame is always something valid
      RMF_avro_backend::Data data;
    };

    std::vector<CategoryData> categories_;

    void add_category_data(Category cat);
    void load_category_frame(Category cat, int frame);

    boost::unordered_map<int, RMF_avro_backend::Frame> frames_;
    boost::unordered_map<int, Ints> frame_children_;
    unsigned int number_of_frames_;

   protected:
    const RMF_avro_backend::Data& get_frame_data(Category cat,
                                                 FrameID frame) const {
      if (frame == ALL_FRAMES) {
        if (static_categories_.size() > cat.get_id()) {
          return static_categories_[cat.get_id()];
        } else {
          return null_static_data_;
        }
      } else {
        RMF_USAGE_CHECK(frame == get_loaded_frame(),
                        "Asking for a non-current frame");
        if (categories_.size() > cat.get_id() &&
            static_cast<unsigned int>(categories_[cat.get_id()].data.frame) ==
                frame.get_index()) {
          return categories_[cat.get_id()].data;
        } else {
          /*std::cout << "No data for category "
                    << get_category_name(cat)
                    << " at frame " << frame << std::endl;*/
          return null_data_;
        }
      }
    }

    RMF_avro_backend::Data& access_frame_data(Category /*cat*/,
                                              FrameID /*frame*/) {
      RMF_THROW(Message("Can't modify read only file"), IOException);
    }

    RMF_avro_backend::Node& access_node(NodeID /*node*/) {
      RMF_THROW(Message("Can't modify read only file"), IOException);
    }

    RMF_avro_backend::File& access_file() {
      RMF_THROW(Message("Can't modify read only file"), IOException);
    }

    void initialize_categories();

   public:
    void flush() {}
    void reload();

    MultipleAvroFileReader(std::string path, bool create, bool read_only);

    RMF_BACKEND_VIRTUAL void set_loaded_frame(
        FrameID frame) RMF_BACKEND_OVERRIDE;

    RMF_BACKEND_VIRTUAL FrameID
        add_frame(std::string name, FrameType t) RMF_BACKEND_OVERRIDE;
    RMF_BACKEND_VIRTUAL void add_child_frame(
        FrameID child_node) RMF_BACKEND_OVERRIDE;
    RMF_BACKEND_VIRTUAL FrameIDs get_children(FrameID node) const
        RMF_BACKEND_OVERRIDE;
    using AvroKeysAndCategories::get_name;
    std::string get_loaded_frame_name() const RMF_BACKEND_OVERRIDE;
    FrameType get_loaded_frame_type() const RMF_BACKEND_OVERRIDE;
    unsigned int get_number_of_frames() const RMF_BACKEND_OVERRIDE;
  };

  }  // namespace avro_backend
} /* namespace RMF */

RMF_DISABLE_WARNINGS

#endif /* RMF_INTERNAL_MULTIPLE_AVRO_FILE_READER_H */
