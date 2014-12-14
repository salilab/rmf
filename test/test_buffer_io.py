#!/usr/bin/env python
from __future__ import print_function
import unittest
import RMF


class Tests(unittest.TestCase):

    def test_data_types(self):
        """Test that import works"""
        buf = "something unfortunately with no higher order bits set"
        RMF.set_log_level("trace")
        bh = RMF.BufferConstHandle(buf)
        name = RMF._get_temporary_file_path("buffer")
        print(name)
        RMF.write_buffer(bh, name)
        bbh = RMF.read_buffer(name)
        self.assertEqual(bbh.get_string(), bh.get_string())

if __name__ == '__main__':
    unittest.main()
