#!/usr/bin/env python
import unittest
import RMF

class GenericTest(unittest.TestCase):
    def test_open_2x(self):
        """Test frame comments"""
        f = RMF.create_rmf_file(RMF._get_temporary_file_path("frames.rmf"))
        cat = f.get_category("cat")
        key = f.get_int_key(cat, "key")
        n = f.get_root_node().add_child("node", RMF.REPRESENTATION)
        n.set_value(key, 1)
        f.add_frame("next", RMF.FRAME)
        print dir(f)
        print f.get_keys(cat)
        for fr in f.get_frames():
            print fr
        self.assertEqual(len(f.get_frames()), 2)


if __name__ == '__main__':
    unittest.main()
