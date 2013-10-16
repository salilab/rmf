#include <RMF/FileHandle.h>
#include <boost/range.hpp>
#include <boost/foreach.hpp>
#include <sstream>

int main(int, char * []) {
  std::string fname = "ranges.rmf";
  RMF::FileHandle fh = RMF::create_rmf_file(fname);
  assert(boost::distance(fh.get_node_ids()) == 1);
  fh.add_frame("hi", RMF::FRAME);
  assert(boost::distance(fh.get_frames()) == 1);
  int count = 0;
  BOOST_FOREACH(RMF::NodeID n, fh.get_node_ids()) {
    ++count;
  }
  assert(count = 1);
  return 0;
}
