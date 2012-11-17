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
      const RMF_internal::NodeData &data= get_node_frame_data(node,           \
                                                        category, frame); \
      return data.lcname##_data;                                        \
    }                                                                   \
    Ucname##Data &                                                      \
    access_frame_##lcname##_data(unsigned int node,                     \
                                 std::string category, int frame) {     \
      return access_node_data(node, category, frame).lcname##_data;     \
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
    get_##lcname##_keys(Category cat) {                                 \
      set<Key<Ucname##Traits> > ret;                                    \
      std::string cat_name= get_category_name(cat);                     \
      const RMF_internal::Data &data= get_frame_data(cat_name,          \
                                                     get_current_frame()); \
      for ( std::map<std::string, RMF_internal::NodeData>::const_iterator it \
              = data.nodes.begin(); it != data.nodes.end(); ++it) {     \
        for (std::map<std::string, Avro##Ucname>::const_iterator iti     \
               = it->second.lcname##_data.begin();                      \
             iti != it->second.lcname##_data.end(); ++iti) {              \
          ret.insert(get_##lcname##_key(cat, iti->first));               \
        }                                                               \
      }                                                                 \
      const RMF_internal::Data &staticdata= get_frame_data(cat_name,    \
                                                           ALL_FRAMES); \
      for ( std::map<std::string, RMF_internal::NodeData>::const_iterator it \
              = staticdata.nodes.begin(); it != staticdata.nodes.end(); ++it) { \
        for (std::map<std::string, Avro##Ucname>::const_iterator iti    \
               = it->second.lcname##_data.begin();                      \
             iti != it->second.lcname##_data.end(); ++iti) {              \
          ret.insert(get_##lcname##_key(cat, iti->first));               \
        }                                                               \
      }                                                                 \
      return vector<Key<Ucname##Traits> >(ret.begin(), ret.end());      \
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

      RMF_internal::Data null_frame_data_;

      const RMF_internal::Data &get_frame_data(std::string category,
                                 int frame) const {
        std::map<std::string, RMF::vector<RMF_internal::Data > >::const_iterator
          it= all_.category.find(category);
        if (it==all_.category.end()) {
          return null_frame_data_;
        }
        if (it->second.size() <= (frame+1)) {
          return null_frame_data_;
        } else {
          return it->second[frame+1];
        }
      }

      const RMF_internal::Node &get_node(unsigned int node) const {
        return all_.nodes[node];
      }

      const std::vector<RMF_internal::Node> &get_nodes_data() const {
        return all_.nodes;
      }

      RMF_internal::Node &access_node(unsigned int node) {
        dirty_=true;
        if (all_.nodes.size() <= node) {
          all_.nodes.resize(node+1);
        }
        return all_.nodes[node];
      }

      const RMF_internal::File &get_file() const {
        return all_.file;
      }

      RMF_internal::File &access_file() {
        dirty_=true;
        return all_.file;
      }

      RMF_internal::NodeData null_node_data_;

      const RMF_internal::NodeData &get_node_frame_data(int node,
                                    std::string category,
                                    int frame) const {
        const RMF_internal::Data &data= get_frame_data(category, frame);
        std::map<std::string, RMF_internal::NodeData>::const_iterator
          nit= data.nodes.find(get_node_string(node));
        if (nit == data.nodes.end()) {
          return null_node_data_;
        } else {
          return nit->second;
        }
      }

      const RMF_internal::Frame& get_frame(int i) const {
        return all_.frames[i+1];
      }

      RMF_internal::Frame& access_frame(int i) {
        dirty_=true;
        if (all_.frames.size() <= i+1) {
          all_.frames.resize(i+1);
        }
        return all_.frames[i];
      }

      RMF_internal::Data &access_frame_data(std::string category,
                              int frame) {
        dirty_=true;
        if (all_.category[category].size() <= (frame+1)) {
          all_.category[category].resize(frame+2);
          dirty_=true;
        }
        return all_.category[category][frame+1];
      }


      RMF_internal::NodeData &access_node_data(int node,
                                 std::string category,
                                 int frame) {
        dirty_=true;
        RMF_internal::Data &data= access_frame_data(category, frame);
        return data.nodes[get_node_string(node)];
      }

      template <class TypeTraits>
    Key<TypeTraits>
        get_key_helper(Category category,
                       std::string name) {
        typename NameKeyInnerMap::const_iterator it
          = name_key_map_[category].find(name);
        if (it == name_key_map_[category].end()) {
          unsigned int id= key_data_map_.size();
          key_data_map_[id].name=name;
          key_data_map_[id].category=category;
          key_data_map_[id].type_index=TypeTraits::get_index();
          name_key_map_[category][name]=id;
          RMF_INTERNAL_CHECK(get_key_helper<TypeTraits>(category,
                                                        name)
                             == Key<TypeTraits>(id),
                             "Keys don't match");
          return Key<TypeTraits>(id);
        } else {
          int id= it->second;
          RMF_INTERNAL_CHECK(name==it->first,
                             "Odd names");
          return Key<TypeTraits>(id);
        }
      }

      const std::string &get_node_string(int node) const {
        return node_keys_[node];
      }

      template <class TypeTraits>
        const std::string &get_key_string(Key<TypeTraits> k) const {
        RMF_INTERNAL_CHECK(k.get_id() >=0,
                           "Bad key");
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
