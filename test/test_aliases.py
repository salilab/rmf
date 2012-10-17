#!/usr/bin/python
import unittest
import RMF

class GenericTest(unittest.TestCase):
    def test_aliases(self):
        """Test that aliases can be used and resolve"""
        path=RMF._get_temporary_file_path("alias.rmf")
        print path
        fh= RMF.create_rmf_file(path)
        rh= fh.get_root_node()
        nh= rh.add_child("hi", RMF.REPRESENTATION)
        nh.add_child(rh)
        ch= nh.get_children()
        self.assertEqual(len(ch), 1)
        print ch
        self.assertEqual(ch[0], rh)

if __name__ == '__main__':
    unittest.main()
