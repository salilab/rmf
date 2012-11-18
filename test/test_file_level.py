#!/usr/bin/python
import unittest
import RMF
import shutil

class Tests(unittest.TestCase):
    def _show(self, g):
        for i in range(0, g.get_number_of_children()):
            print i, g.get_child_name(i), g.get_child_is_group(i)
    """Test the python code"""
    def test_perturbed(self):
        """Test creating a simple hierarchy file"""
        for suffix in RMF.suffixes:
            f= RMF.create_rmf_file(RMF._get_temporary_file_path("test_file_perturbed."+suffix))
            r= f.get_root_node()
            print r.get_type()
            sc= f.get_category("sequence")
            ik= f.get_int_key(sc, "ik0")
            f.set_current_frame(0)
            r.set_value(ik, 1)
            print r.get_value(ik)
            self.assertEqual(r.get_value(ik), 1)
    def test_frames(self):
        """Test creating a simple hierarchy file with frames"""
        for suffix in RMF.suffixes:
            f= RMF.create_rmf_file(RMF._get_temporary_file_path("test_file_frames."+suffix))
            r= f.get_root_node()
            print r.get_type()
            sc= f.get_category("sequence")
            ik= f.get_int_key(sc, "ik0")
            f.set_current_frame(0)
            r.set_value(ik, 1)
            self.assertEqual(r.get_value(ik), 1, 0)
            f.set_current_frame(1)
            r.set_value(ik, 2)
            self.assertEqual(r.get_value(ik), 2)
            f.set_current_frame(0)
            self.assertEqual(r.get_value(ik), 1)
            self.assertEqual(f.get_number_of_frames(), 2)
    def test_perturbed_values(self):
        """Test null values"""
        for suffix in RMF.suffixes:
            f= RMF.create_rmf_file(RMF._get_temporary_file_path("test_file."+suffix))
            r= f.get_root_node()
            print r.get_type()
            sc= f.get_category("sequence")
            ik= f.get_int_key(sc, "ik0")
            f.set_current_frame(0)
            r.set_value(ik, 1)
            ika= r.get_value_always(ik)
            self.assertEqual(ika, 1)
            f.set_current_frame(1)
            ikna= r.get_value_always(ik)
            self.assertEqual(ikna, RMF.NullInt)
            fk= f.get_float_key(sc, "fk0")
            f.set_current_frame(0)
            r.set_value(fk, 1)
            fka= r.get_value_always(fk)
            self.assertEqual(fka, 1)
            f.set_current_frame(1)
            fkna= r.get_value_always(fk)
            self.assertEqual(fkna, RMF.NullFloat)
            RMF.show_hierarchy(r)
            RMF.show_hierarchy_with_decorators(r)
if __name__ == '__main__':
    unittest.main()
