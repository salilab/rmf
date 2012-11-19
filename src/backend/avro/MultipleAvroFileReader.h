/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF__INTERNAL_SINGLE_AVRO_FILE_READER_H
#define RMF__INTERNAL_SINGLE_AVRO_FILE_READER_H

#include <RMF/config.h>
#include <RMF/internal/SharedData.h>
#include <RMF/infrastructure_macros.h>
#include "MultipleAvroFileBase.h"
#include <avro/DataFile.hh>
#include <backend/avro/AllJSON.h>
#include <boost/shared_ptr.hpp>

namespace RMF {
  namespace internal {

    class RMFEXPORT MultipleAvroFileReader: public MultipleAvroFileBase {
      typedef MultipleAvroFileBase P;
      struct CategoryData {
        boost::shared_ptr<avro::DataFileReader<RMF_internal::Data > > reader;
        // frame is always something valid
        RMF_internal::Data data;
      };

      vector<CategoryData> categories_;

      void add_category_data(Category cat);
      void load_category_frame(Category cat,
                               int frame);

    protected:
      const RMF_internal::Data &get_frame_data(Category cat,
                                               int frame) const {
        if (frame== ALL_FRAMES) {
          if (static_categories_.size() > cat.get_id()) {
            return static_categories_[cat.get_id()];
          } else {
            return null_static_data_;
          }
        } else {
          if (categories_.size() > cat.get_id()
              && categories_[cat.get_id()].data.frame == frame ) {
            return categories_[cat.get_id()].data;
          } else {
            return null_data_;
          }
        }
      }

      RMF_internal::Data &access_frame_data(Category cat,
                                            int frame) {
        RMF_THROW("Can't modify read only file",
                  IOException);
      }

      RMF_internal::Node &access_node(unsigned int node) {
        RMF_THROW("Can't modify read only file",
                  IOException);
      }


      RMF_internal::File &access_file() {
        RMF_THROW("Can't modify read only file",
                  IOException);
      }


      RMF_internal::Frame& access_frame(int i) {
        RMF_THROW("Can't modify read only file",
                  IOException);
      }

      void initialize_categories();
    public:
      void flush() {
      }
      void reload();

      MultipleAvroFileReader(std::string path,
                             bool create, bool read_only);

      void set_current_frame(int frame);

    };

  } // namespace internal
} /* namespace RMF */


#endif /* RMF__INTERNAL_MULTIPLE_AVRO_FILE_READER_H */