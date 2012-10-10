#!/usr/bin/python
import unittest
import RMF
import shutil

class GenericTest(RMF.TestCase):
    def _show(self, g):
        for i in range(0, g.get_number_of_children()):
            print i, g.get_child_name(i), g.get_child_is_group(i)
    """Test the python code"""
    def test_perturbed(self):
        """Test creating a simple hierarchy file with multiple keys"""
        f= RMF.create_rmf_file(self.get_tmp_file_name("multikey.rmf"))
        r= f.get_root_node()
        print r.get_type()
        sc= f.add_category("multikey")
        ikfp= f.add_int_key(sc, "ik0", True)
        ik= f.add_int_key(sc, "ik0", False)
        on= r.add_child("oc", RMF.REPRESENTATION)
        r.set_value(ikfp, 1, 10)
        on.set_value(ik, 10)
        print r.get_value(ikfp, 10)
        print on.get_value(ik)

if __name__ == '__main__':
    unittest.main()