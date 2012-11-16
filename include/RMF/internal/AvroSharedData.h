/**
 *  \file RMF/internal/SharedData.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef RMF__INTERNAL_AVRO_SHARED_DATA_H
#define RMF__INTERNAL_AVRO_SHARED_DATA_H

#include <RMF/config.h>
#include "SharedData.h"
#include "../infrastructure_macros.h"
#include "map.h"
#include "set.h"
#include <RMF/internal/AllJSON.h>

namespace RMF {


  namespace internal {


#define RMF_AVRO_SHARED_TYPE(lcname, Ucname, PassValue, ReturnValue,    \
                                   PassValues, ReturnValues)            \
    Ucname##Traits::Type get_value(unsigned int node,                   \
                                   Key<Ucname##Traits> k) const {       \
    }                                                                   \
    Ucname##Traits::Types get_all_values(unsigned int node,             \
                                         Key<Ucname##Traits> k)  {      \
    }                                                                   \
    void set_value(unsigned int node,                                   \
                   Key<Ucname##Traits> k,                               \
                   Ucname##Traits::Type v) {                            \
    }                                                                   \
    bool get_has_frame_value(unsigned int node,                         \
                             Key<Ucname##Traits> k) const {             \
    }                                                                   \
    vector<Key<Ucname##Traits> >                                        \
    get_##lcname##_keys(Category category) const {                      \
    }                                                                   \
    Key<Ucname##Traits>                                                 \
    get_##lcname##_key(Category category,                               \
                       std::string name) {                              \
    }                                                                   \
    std::string get_name(Key<Ucname##Traits> k) const {                 \
    }                                                                   \
    Category get_category(Key<Ucname##Traits> key) const {              \
    }


    class RMFEXPORT AvroSharedData: public SharedData {
      RMF_internal::File file_;
      bool file_data_dirty_;
      RMF_internal::Hierarchy hierarchy_;
      bool hierarchy_dirty_;
      RMF_internal::Frames frames_;
      bool frames_dirty_;
      struct Data {
        RMF_internal::Data frame_data_;
        bool frame_data_dirty_;
        RMF_internal::Data static_data_;
        bool static_data_dirty_;
        avro::DataFileReader<RMF_internal::Data> frame_data_reader_;
        avro::DataFileWriter<RMF_internal::Data> frame_data_writer_;
        RMF_internal::Keys keys_;
        bool keys_dirty_;
      };
      map<int, Data> category_data_;
      map<std::string, int> category_name_to_id_;
      map<int, std::string> category_id_to_name_;
      map<int, std::string> node_keys_;

      /*
      template <class Map, class Traits>
        Traits::Type get_value_from_map(const Map &m,
                                        Key<Traits> k) const {
        std::string sk= get_node_key(k);
        if (m.find(sk) == m.end()) return Traits::get_null_value();
        else return m.find(sk)->second;
      }
      template <class Traits>
        Traits::Type get_value_from_data(unsigned int node,
                                         Key<Traits> k,
                                         const RMF_internal::Data&data) const {
        std::string nk= get_node_key(node);
        if (data.nodes.find(nk)== data.nodes.end()) {
          return Traits::get_null_value();
        } else {
          return get_value_from_map(data.nodes.find(nk)->second.lcname##_data,
                         k);
        }
      }
      template <class Traits>
        void set_value_in_data(unsigned int node,
                               Key<Traits> k,
                               Traits::Type v,
                               RMF_internal::Data&data) {
        data.nodes[get_node_key(node)].lcname##_data[get_key_key(k)]=v;
      }
      template <class TraitsT>
        typename TraitsT::Type get_value_impl(unsigned int node,
                                              Key<TraitsT> k) const {
        Data *data=get_data(k.get_category());
        if (!data) return TraitsT::get_null_value();
        typename TraitsT::Type ret= get_value(node, k, data->frame_data_);
        if (!TraitsT::get_is_null(ret)) return ret;
        else return get_value(node, k, data->static_data_);
      }
      template <class TraitsT>
        void set_value_impl(unsigned int node,
                            Key<TraitsT> k,
                            typename TraitsT::Type v) {
        Data *data=get_data_always(k.get_category());
        if (get_current_frame() == ALL_FRAMES) {
          set_value(node, k, v, data->static_data_);
        } else {
          set_value(node, k, v, data->frame_data_);
        }
      }
      std::string get_node_key(unsigned int n) {
        map<int, std::string>::iterator it= node_keys_.find(n);
        if (it == node_keys_.end()) {
          std::sstringstream ss;
          ss << n;
          node_keys_[n]=ss.str();
          return ss.str();
        } else {
          return it->second;
        }
        }*/
    public:
      RMF_FOREACH_TYPE(RMF_AVRO_SHARED_TYPE);

      AvroSharedData(std::string g, bool create);
      ~AvroSharedData();
      RMF_FOREACH_TYPE(RMF_AVRO_SHARED_TYPE);
      void flush() const;
      //!
      std::string get_name(unsigned int node) const {
         return hierarchy_.hierarchy.find(node)->name;
      }
      //!
      unsigned int get_type(unsigned int node) const {
        return boost::lexical_cast<NodeType>(hierarchy_.hierarchy.find(node)->type);
      }
      int add_child(int node, std::string name, int t);
      void add_child(int node, int child_node);
      Ints get_children(int node) const;
      void save_frames_hint(int) {}
      //!
      unsigned int get_number_of_frames() const {
        return file_.number_of_frames();
      }
      int add_category(std::string name);
      Categories get_categories() const;
      Category get_category(std::string name);
      std::string get_category_name(Category kc) const  {
        return category_data_map_.find(kc)->second.name;
      }

      std::string get_description() const {
        return file_.description;
      }
      void set_description(std::string str) {
        file_.description=str;
        file_dirty_=true;
      }

      void set_frame_name(std::string str) {
        return
      }
      std::string get_frame_name() const;

      void reload();
      void set_current_frame(int frame);
    };

  } // namespace internal
} /* namespace RMF */


#endif /* RMF__INTERNAL_AVRO_SHARED_DATA_H */
