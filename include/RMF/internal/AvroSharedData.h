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
#include "../constants.h"
#include "map.h"
#include "set.h"
#include <RMF/internal/AllJSON.h>

namespace RMF {
  namespace internal {

    typedef int32_t AvroInt;
    typedef double AvroFloat;
    typedef std::string AvroString;
    typedef int32_t AvroIndex;
    typedef int32_t AvroNodeID;
    typedef std::vector<std::string> AvroStrings;
    typedef std::vector<int32_t> AvroInts;
    typedef std::vector<double> AvroFloats;
    typedef std::vector<int32_t> AvroIndexes;
    typedef std::vector<int32_t> AvroNodeIDs;

    template <class Out, class In>
    void avro_assign(Out &out, In in) {
      out=in;
    }

    inline void avro_assign(NodeID &out, int32_t in) {
      out= NodeID(in);
    }
    inline void avro_assign(int32_t &out, NodeID in) {
      out=in.get_index();
    }

#if RMF_USE_DEBUG_VECTOR
    template <class Out, class In>
    void avro_assign(std::vector<Out> &out, vector<In> in) {
      out.resize(in.size());
      for (unsigned int i=0; i< in.size(); ++i) {
        avro_assign(out[i], in[i]);
      }
    }
    template <class Out, class In>
    void avro_assign(vector<Out> &out, std::vector<In> in) {
     out.resize(in.size());
      for (unsigned int i=0; i< in.size(); ++i) {
        avro_assign(out[i], in[i]);
      }
    }
#else
     template <class Out, class In>
     void avro_assign(std::vector<Out> &out, const std::vector<In>& in) {
       out.resize(in.size());
       for (unsigned int i=0; i< in.size(); ++i) {
         avro_assign(out[i], in[i]);
       }
     }
#endif


#define RMF_AVRO_SHARED_TYPE(lcname, Ucname, PassValue, ReturnValue,    \
                                   PassValues, ReturnValues)            \
    private:                                                            \
    typedef std::map<std::string, Avro##Ucname> Ucname##Data;           \
    Ucname##Data empty_##lcname##_data_;                                \
    const Ucname##Data &                                                \
    get_frame_##lcname##_data(unsigned int node,                        \
                              std::string category,                     \
                              int frame) const {                        \
      std::map<std::string, std::vector<RMF_internal::Data > >::const_iterator it \
        = all_.category.find(category);                                 \
      if (it== all_.category.end()                                      \
          || it->second.size() <= frame+1){                             \
        return empty_##lcname##_data_;                                  \
      }                                                                 \
      std::map<std::string, RMF_internal::NodeData>::const_iterator     \
        nit= it->second[frame+1].nodes.find(get_node_string(node));      \
      if (nit == it->second[frame+1].nodes.end()) {                      \
        return empty_##lcname##_data_;                                  \
      } else {                                                          \
        return nit->second.lcname##_data;                               \
      }                                                                 \
    }                                                                   \
    Ucname##Data &                                                      \
    access_frame_##lcname##_data(unsigned int node,                     \
                                 std::string category, int frame) {     \
      if (all_.category[category].size() <= frame+1) {                 \
        all_.category[category].resize(frame+1);                       \
      }                                                                 \
      return all_.category[category][frame+1].nodes[get_node_string(node)] \
        .lcname##_data;                                                 \
    }                                                                   \
    public:                                                             \
    Ucname##Traits::Type get_value(unsigned int node,                   \
                                   Key<Ucname##Traits> k) const {       \
      Category cat= get_category(k);                                    \
      std::string cat_name= get_category_name(cat);                     \
      const Ucname##Data &data= get_frame_##lcname##_data(node,         \
                                                          cat_name,     \
                                                          get_current_frame()); \
      Ucname##Data::const_iterator it= data.find(get_key_string(k));    \
      if (it != data.end()) {                                           \
        Ucname##Traits::Type ret;                                       \
        avro_assign(ret, it->second);                                   \
        return ret;                                                     \
      }                                                                 \
      if (get_current_frame() == ALL_FRAMES) {                          \
        return Ucname##Traits::get_null_value();                        \
      }                                                                 \
      const Ucname##Data &staticdata= get_frame_##lcname##_data(node,   \
                                                                cat_name, \
                                                                ALL_FRAMES); \
      Ucname##Data::const_iterator staticit= staticdata.find(get_key_string(k)); \
      if (staticit != staticdata.end()) {                               \
        Ucname##Traits::Type ret;                                       \
        avro_assign(ret, staticit->second);                             \
        return ret;                                                     \
      }                                                                 \
      return Ucname##Traits::get_null_value();                          \
    }                                                                   \
    Ucname##Traits::Types get_all_values(unsigned int node,             \
                                         Key<Ucname##Traits> k)  {      \
      Ucname##Traits::Types ret;                                        \
      for (unsigned int i=0; i< get_number_of_frames(); ++i) {          \
        set_current_frame(i);                                           \
        ret.push_back(get_value(node, k));                              \
      }                                                                 \
      return ret;                                                       \
    }                                                                   \
    void set_value(unsigned int node,                                   \
                   Key<Ucname##Traits> k,                               \
                   Ucname##Traits::Type v) {                            \
      Category cat= get_category(k);                                    \
      std::string cat_name= get_category_name(cat);                     \
      Ucname##Data &data= access_frame_##lcname##_data(node, cat_name,  \
                                                       get_current_frame());\
      avro_assign(data[get_key_string(k)], v);                          \
      dirty_=true;                                                      \
    }                                                                   \
    bool get_has_frame_value(unsigned int node,                         \
                             Key<Ucname##Traits> k) const {             \
      Category cat= get_category(k);                                    \
      std::string cat_name= get_category_name(cat);                     \
      const Ucname##Data &data= get_frame_##lcname##_data(node,         \
                                                          cat_name,     \
                                                          get_current_frame()); \
      Ucname##Data::const_iterator it= data.find(get_key_string(k));    \
      return (it != data.end());                                        \
    }                                                                   \
    vector<Key<Ucname##Traits> >                                        \
    get_##lcname##_keys(Category category) const {                      \
      return vector<Key<Ucname##Traits> >();                            \
    }                                                                   \
    Key<Ucname##Traits>                                                 \
    get_##lcname##_key(Category category,                               \
                       std::string name) {                              \
      return get_key_helper<Ucname##Traits>(category, name);            \
    }                                                                   \
    std::string get_name(Key<Ucname##Traits> k) const {                 \
      return key_data_map_.find(k.get_id())->second.name;               \
    }                                                                   \
    Category get_category(Key<Ucname##Traits> k) const {                \
      return key_data_map_.find(k.get_id())->second.category;           \
    }

    class RMFEXPORT AvroSharedData: public SharedData {
      RMF_internal::All all_;
      bool dirty_;
      bool read_only_;

      typedef map<Category, std::string> CategoryNameMap;
      typedef map<std::string, Category> NameCategoryMap;
      CategoryNameMap category_name_map_;
      NameCategoryMap name_category_map_;
      struct KeyData {
        std::string name;
        Category category;
        int type_index;
      };
      typedef map<unsigned int, KeyData> KeyDataMap;
      KeyDataMap key_data_map_;
      typedef map<std::string, unsigned int> NameKeyInnerMap;
      typedef map<Category, NameKeyInnerMap> NameKeyMap;
      NameKeyMap name_key_map_;

      vector<std::string> node_keys_;

      template <class TypeTraits>
    Key<TypeTraits>
        get_key_helper(Category category,
                       std::string name) {
        typename NameKeyInnerMap::iterator it
          = name_key_map_[category].find(name);
        if (it == name_key_map_[category].end()) {
          unsigned int id= key_data_map_.size();
          key_data_map_[id].name=name;
          key_data_map_[id].category=category;
          key_data_map_[id].type_index=TypeTraits::get_index();
          name_key_map_[category][name]=id;
          return Key<TypeTraits>(id);
        } else {
          return it->second;
        }
      }

      const std::string &get_node_string(int node) const {
        return node_keys_[node];
      }

      template <class TypeTraits>
        const std::string &get_key_string(Key<TypeTraits> k) const {
        return key_data_map_.find(k.get_id())->second.name;
      }

      void initialize_categories();
      void initialize_node_keys();
    public:
      RMF_FOREACH_TYPE(RMF_AVRO_SHARED_TYPE);

      AvroSharedData(std::string g, bool create, bool read_only);
      ~AvroSharedData();
      void flush();
      std::string get_name(unsigned int node) const;
      unsigned int get_type(unsigned int node) const;
      int add_child(int node, std::string name, int t);
      void add_child(int node, int child_node);
      Ints get_children(int node) const;
      void save_frames_hint(int) {}
      unsigned int get_number_of_frames() const;
      Categories get_categories() const;
      Category get_category(std::string name);
      std::string get_category_name(Category kc) const;
      std::string get_description() const;
      void set_description(std::string str);

      void set_frame_name(std::string str);
      std::string get_frame_name() const;

      void reload();
      void set_current_frame(int frame);
    };

  } // namespace internal
} /* namespace RMF */


#endif /* RMF__INTERNAL_AVRO_SHARED_DATA_H */
