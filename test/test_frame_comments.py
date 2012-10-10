#!/usr/bin/python
import unittest
import RMF

class GenericTest(unittest.TestCase):
    def _test_one(self, name):
        f= RMF.create_rmf_file(name)
        f.set_frame_name(10, "hi")
        self.assertEqual(f.get_frame_name(1), "")
        self.assertEqual(f.get_frame_name(10), "hi")
    def test_open_2x(self):
        """Test frame comments"""
        self._test_one(RMF._get_temporary_file_path("comments.rmf"))
        self._test_one(RMF._get_temporary_file_path("comments.prmf"))

if __name__ == '__main__':
    unittest.main()
