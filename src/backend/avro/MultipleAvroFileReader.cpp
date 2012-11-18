/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include "avro_schemas.h"
#include "MultipleAvroFileReader.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <stdexcept>

namespace RMF {
  namespace internal {

    namespace {
      void clear_data(RMF_internal::Data &data,
                      int frame) {
        data= RMF_internal::Data();
        data.frame=frame;
      }
    }

    void MultipleAvroFileReader::set_current_frame(int frame) {
      for (unsigned int i=0; i< categories_.size(); ++i) {
        if (!categories_[i].reader) {
          clear_data(categories_[i].data, frame);
        }
        if (frame < categories_[i].data.frame) {
          std::string name= get_category_dynamic_file_path(Category(i));
          categories_[i].reader
            .reset( new avro::DataFileReader<RMF_internal::Data >(name.c_str(),
                                                                  get_Data_schema()));
          if (!categories_[i].reader->read(categories_[i].data)) {
            clear_data(categories_[i].data, frame);
          }
        }
        while (frame > categories_[i].data.frame) {
          if (!categories_[i].reader->read(categories_[i].data)) {
            clear_data(categories_[i].data, frame);
            break;
          }
          if (frame < categories_[i].data.frame) {
            clear_data(categories_[i].data, frame);
            break;
          }
        }
      }
      MultipleAvroFileBase::set_current_frame(frame);
    }

    MultipleAvroFileReader::MultipleAvroFileReader(std::string path,
                                                   bool create, bool read_only):
      MultipleAvroFileBase(path){
      RMF_INTERNAL_CHECK(!create, "Can't create a file for reading");
      RMF_INTERNAL_CHECK(read_only, "RMF2 files open for reading must be read-only");
      reload();
    }

    template <class It>
    std::vector<std::string> get_categories_from_disk(It a, It b) {
      std::vector<std::string> ret;
      for (; a != b; ++a) {
        if (a->path().extension()== "frames" || a->path().extension()== "static") {
          ret.push_back(a->path().stem().string());
        }
      }
      return ret;
    }
    void MultipleAvroFileReader::initialize_categories() {
      std::vector<std::string> categories
        =get_categories_from_disk(boost::filesystem::directory_iterator(get_file_path()),
                                  boost::filesystem::directory_iterator());
      categories_.clear();
      for (unsigned int i=0; i< categories.size(); ++i) {
        Category cat=get_category(categories[i]);
        // not safe, need to handle reload separately
        add_category_data(cat);
      }
    }



#define RMF_RELOAD(UCName, lcname)                                      \
    {                                                                   \
      avro::DataFileReader<UCName>                                      \
        re(get_##lcname##_file_path().c_str(),                          \
           get_##UCName##_schema());                                    \
      re.read(lcname##_);                                               \
    }

    void MultipleAvroFileReader::reload() {
      RMF_RELOAD(File, file);
      RMF_RELOAD(Nodes, nodes);
      RMF_RELOAD(Frames, frames);

      initialize_categories();
      initialize_node_keys();
      set_current_frame(get_current_frame());
    }

    void MultipleAvroFileReader::add_category_data(Category cat) {
      RMF_INTERNAL_CHECK(cat.get_id()== categories_.size(),
                         "Bad category added");
      categories_.resize(cat.get_id()+1);
      static_categories_.resize(cat.get_id()+1);

      std::string dynamic_path=get_category_dynamic_file_path(cat);
      if (boost::filesystem::exists(dynamic_path)) {
        categories_[cat.get_id()].reader
          .reset(new avro::DataFileReader<RMF_internal::Data>(dynamic_path.c_str(),
                                                              get_Data_schema()));
        categories_[cat.get_id()].data.frame=-2;
      } else {
        categories_[cat.get_id()].data.frame=-2;
      }

      std::string static_path=get_category_static_file_path(cat);
      if (boost::filesystem::exists(static_path)) {
        avro::DataFileReader<RMF_internal::Data>
          reader(static_path.c_str(), get_Data_schema());
        reader.read(static_categories_[cat.get_id()]);
      } else {
        static_categories_[cat.get_id()].frame=ALL_FRAMES;
      }
    }

  } // namespace internal
} /* namespace RMF */
