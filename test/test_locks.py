#!/usr/bin/python
import unittest
import RMF

class GenericTest(unittest.TestCase):
    def test_aliases(self):
        """Test that locks lock and unlock for rmf files"""
        fh= RMF.create_rmf_file(RMF._get_temporary_file_path("locks.rmf"))
        self.assert_(fh.set_is_locked(True))
        self.assert_(not fh.set_is_locked(True))
        self.assert_(not fh.set_is_locked(False))
        self.assert_( fh.set_is_locked(True))
    def test_aliases2(self):
        """Test FileLock"""
        fh= RMF.create_rmf_file(RMF._get_temporary_file_path("locks.rmf"))
        lk= RMF.FileLock(fh, 1000)
        self.assert_(not fh.set_is_locked(True))
        del lk
        self.assert_(fh.set_is_locked(True))


if __name__ == '__main__':
    unittest.main()
