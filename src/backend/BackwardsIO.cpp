#include "BackwardsIO.h"
#include <boost/unordered_map.hpp>
#include <utility>
#include <boost/array.hpp>

RMF_ENABLE_WARNINGS

namespace RMF {
namespace backends {
const std::pair<std::string, boost::array<std::string, 3> > vector_3_names[] = {
  {"coordinates", {{"cartesian x", "cartesian y", "cartesian z"}}},
  {"rgb color", {{"rgb color red", "rgb color green", "rbg color blue"}}},
  {"force", {{"force cartesian x", "force cartesian y", "force cartesian z"}}},
    {"torque",
     {{"torque cartesian x", "torque cartesian y", "torque cartesian z"}}}};
const int vector_3_names_size =
    sizeof(vector_3_names) /
    sizeof(std::pair<std::string, boost::array<std::string, 3> >);

V3N vector_3_names_map(vector_3_names, vector_3_names + vector_3_names_size);

const std::pair<std::string, boost::array<std::string, 4> > vector_4_names[] = {
    {"orientation",
     {{"orientation r", "orientation i", "orientation j", "orientation k"}}},
    {"rotation",
     {{"reference frame orientation r", "reference frame orientation i",
       "reference frame orientation j", "reference frame orientation k"}}}};
const int vector_4_names_size =
    sizeof(vector_4_names) /
    sizeof(std::pair<std::string, boost::array<std::string, 4> >);

V4N vector_4_names_map(vector_4_names, vector_4_names + vector_4_names_size);

const std::pair<std::string, boost::array<std::string, 3> > vectors_3_names[] =
    {{"coordinates list", {{"cartesian xs", "cartesian ys", "cartesian zs"}}}};
const int vectors_3_names_size =
    sizeof(vectors_3_names) /
    sizeof(std::pair<std::string, boost::array<std::string, 3> >);

V3N vectors_3_names_map(vectors_3_names,
                        vectors_3_names + vectors_3_names_size);
}
}

RMF_DISABLE_WARNINGS
