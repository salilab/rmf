#!/usr/bin/env python
import unittest
import RMF
import shutil

class GenericTest(unittest.TestCase):
    def _show(self, g):
        for i in range(0, g.get_number_of_children()):
            print i, g.get_child_name(i), g.get_child_is_group(i)
    def _copy_to(self, last_frame, suffix):
        nm= RMF._get_test_input_file_path("sink.rmf")
        onm= RMF._get_temporary_file_path("sink_out."+suffix)
        print nm, onm
        f= RMF.open_rmf_file_read_only(nm)
        of= RMF.create_rmf_file(onm)
        RMF.clone_file_info(f, of)
        RMF.clone_hierarchy(f, of)
        RMF.clone_static_frame(f, of)
        for fr in f.get_frames():
            f.set_current_frame(fr)
            nfid = of.add_frame(f.get_current_frame_name(), f.get_current_frame_type())
            self.assertEqual(nfid, fr)
            RMF.clone_loaded_frame(f, of)
        print "deling"
        del of
        print "reopening"
        of= RMF.open_rmf_file_read_only(onm)
        self.assert_(RMF.get_equal_structure(f, of))
        self.assert_(RMF.get_equal_static_values(f, of))
        for i in range(0, last_frame+1):
            fid = RMF.FrameID(i)
            f.set_current_frame(fid)
            of.set_current_frame(fid)
            if suffix != "rmft":
                # going through a text format perturbs values
                self.assert_(RMF.get_equal_current_values(f, of))
    def test_perturbed(self):
        """Test copying an rmf file"""
        RMF.HDF5.set_show_errors(True)
        for suffix in RMF.suffixes:
            print suffix
            self._copy_to(-1, suffix)
            self._copy_to(1, suffix)

if __name__ == '__main__':
    unittest.main()
