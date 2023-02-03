import unittest
import RMF


class Tests(unittest.TestCase):

    def test_closed_repr(self):
        """Test show of closed file handle"""
        fh = RMF.FileConstHandle()
        _ = repr(fh)
        fh = RMF.FileHandle()
        _ = repr(fh)


if __name__ == '__main__':
    unittest.main()
