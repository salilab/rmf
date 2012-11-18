/**
 *  \file RMF/paths.cpp
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include "avro_schemas.h"
#include "MultipleAvroFile.h"
#include <RMF/internal/paths.h>
#include <RMF/decorators.h>
#include <avro/Compiler.hh>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <stdexcept>

namespace RMF {
  namespace internal {

    void MultipleAvroFile::set_current_frame(int frame) {
      flush();
    }

    MultipleAvroFile::MultipleAvroFile(std::string path):
      AvroKeysAndCategories(path){
    }

    template <class It>
    std::vector<std::string> get_categories_from_disk(It a, It b) {
      std::vector<std::string> ret;
      for (; a != b; ++a) {
        if (a->path().extension()== "data") {
          ret.push_back(a->path().stem());
        }
      }
      return ret;
    }
    void MultipleAvroFile::initialize_categories() {
      std::vector<std::string> categories
        =get_categories_from_disk(boost::filesystem::directory_iterator(get_file_path()),
                                  boost::filesystem::directory_iterator());
      categories_.clear();
      for (unsigned int i=0; i< catgories.size(); ++i) {
        Category cat=get_category(categories[i]);
        initialize_category_data(cat);
      }
    }

    void MultipleAvroFile::initialize_node_keys() {
      clear_node_keys();
      for (unsigned int i=0; i< nodes_.size(); ++i) {
        add_node_key();
      }
    }

#define RMF_RELOAD(UCName, lcname)                                      \
    {                                                                   \
      avro::DataFileReader<UCName>                                      \
        re(get_##lcname##_file_path(), get_##UCName##_schema());        \
      re.read(lcname##_);                                               \
      lcname##_dirty_=false;
    }

    void MultipleAvroFile::reload() {
      RMF_RELOAD(File, file);
      RMF_RELOAD(Nodes, nodes);
      RMF_RELOAD(Frames, frames);
      RMF_RELOAD(StaticData, static_data);

      initialize_categories();
      initialize_node_keys();
    }

#define RMF_FLUSH(UCName, lcname)                                       \
    if (lcname##_dirty_) {                                              \
      avro::DataFileWriter<UCName>                                      \
        wr(get_##lcname##_file_path(), get_##UCName##_schema());        \
      wr.write(lcname##_);                                              \
    }

    void MultipleAvroFile::flush() {
      RMF_FLUSH(Nodes, nodes);
      RMF_FLUSH(Frames, frames);
      RMF_FLUSH(StaticData, static_data);
      for (unsigned int i=0; i< categories_.size(); ++i) {
        flush_category_data(Category(i));
      }
      // must be last
      RMF_FLUSH(File, file);
    }


    void MultipleAvroFile::initialize_category_data(Category cat) {
      while (categories_.size() <= cat.get_id()) {
        int id= categories_.size();
        categories_.push_back(CategoryData());
        categories_.back().name=get_category_file_path(Category(id));
        if (boost::filesystem::exists(cur_data.name)) {
          cur_data.reader=
            avro::DataFileReader<RMF_internal::Data>(cur_data.name,
                                                     get_Data_schema());
          categories_.back().data.frame=-2;
          cur_data.has_reader=true;
        } else {
          cur_data.data.frame=ALL_FRAMES;
          categories_.back().has_reader=false;
        }
        categories_.back().dirty=false;
        categories_.back().has_writer=false;
        categories_.back().audit();
      }
    }

    std::string MultipleAvroFile::get_category_file_path(Category cat) {
      boost::filesystem::path base=get_file_path();
      boost::filesystem::path full= base/get_category_name(cat)+".data";
      return full.string();
    }

    void MultipleAvroFile::CategoryData::audit() const {
      RMF_INTERNAL_CHECK(!file_name.empty(), "Bad file name");
      RMF_INTERNAL_CHECK(!has_reader || !has_writer,
                         "Can't have both a reader and a writer");
      RMF_INTERNAL_CHECK(has_reader || has_writer || !boost::file::exists(name),
                         "Either there must be a reader or a writer or no file");
      RMF_INTERNAL_CHECK(data.frame >=ALL_FRAMES,
                         "Bad frame index");
    }

    bool MultipleAvroFile::flush_category_frame(Category cat) {
      CategoryData &cur_data=categories_[cat.get_id()];
      if (cur_data.dirty) {
        RMF_USAGE_CHECK(cur_data.data.frame <= file_.number_of_frames,
                        "Trying to write a frame that is not the last frame.");
        if (!cur_data.has_writer) {
          cur_data.writer= avro::DataFileWriter<RMF_internal::Data>(cur_data.name,
                                                                    sget_Data_schema());
          cur_data.has_writer=true;
        }
        cur_data.writer.write(cur_data.data);
        cur_data.dirty=false;
        return true;
      }
      cur_data.audit();
      return false;
    }
    void MultipleAvroFile::load_category_frame(Category cat,
                                               int frame) {
      CategoryData &cur_data=categories_[cat.get_id()];
      cur_data.audit();
      if (frame==cur_data.data.frame) return;
      // check if we need to write and write if necessary
      if (flush_category_data(cat)) {
        // writing only
        cur_data.data=RMF_iternal::Data();
        cur_data.frame=frame;
        return;
      }
      // want to go backwards or have not initialized reader
      if (frame < cur_data.data.frame) {
        if (cur_data.has_reader) {
          cur_data.reader=
            avro::DataFileReader<RMF_internal::Data>(cur_data.name,
                                                     get_Data_schema());
        } else {
          cur_data.data=RMF_internal::Data();
          cur_data.data.frame=frame;
        }
      }
      while (frame>cur_data.data.frame) {
        if (!cur_data.reader.read(cur_data.data)) {
          cur_data.data=RMF_internal::Data();
          cur_data.data.frame=frame;
          return
        }
      }
      if (frame > cur_data.data.frame) {
        cur_data.data=RMF_internal::Data();
        cur_data.data.frame=frame;
      }
      cur_data.audit();
    }


  } // namespace internal
} /* namespace RMF */
