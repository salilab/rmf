#!/usr/bin/env python
import unittest
import RMF

class GenericTest(unittest.TestCase):

    def test_aliases(self):
        """Test trivial usage"""
        for suffix in RMF.suffixes:
            path=RMF._get_temporary_file_path("trivial."+suffix)
            print path
            fh= RMF.create_rmf_file(path)
            cat = fh.get_category("MyCat")
            key = fh.get_int_key(cat, "MyKey")
            fh.set_current_frame(RMF.ALL_FRAMES)
            fh.get_root_node().set_value(key, 1)
            self.assertEqual(fh.get_root_node().get_value(key), 1)
            RMF.show_hierarchy_with_values(fh.get_root_node())
            del fh
            fh = RMF.open_rmf_file_read_only(path)
            cat = fh.get_category("MyCat")
            key = fh.get_int_key(cat, "MyKey")
            fh.set_current_frame(RMF.ALL_FRAMES)
            v = fh.get_root_node().get_value(key)
            self.assertEqual(v, 1)

if __name__ == '__main__':
    unittest.main()
