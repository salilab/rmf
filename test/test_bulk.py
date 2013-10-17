#!/usr/bin/env python
import unittest
import RMF


class GenericTest(unittest.TestCase):

    def test_multiparent(self):
        """Test bulk fetching of coordinates"""
        for suffix in RMF.suffixes:
            path = RMF._get_temporary_file_path("bulk." + suffix)
            print path
            fh = RMF.create_rmf_file(path)
            fh.add_frame("root", RMF.FRAME)
            f = RMF.ParticleFactory(fh)
            rh = fh.get_root_node()
            nids = []
            for i in range(0, 10):
                nh = rh.add_child("hi", RMF.REPRESENTATION)
                d = f.get(nh)
                d.set_coordinates(RMF.Vector3(i, i, i))
                nids.append(nh.get_id())
            allcoords = f.get_coordinates(fh, nids)
            print allcoords
            assert(len(allcoords) == len(nids))
            for i, c in enumerate(allcoords):
                for j in range(0, 3):
                    assert(c[j] == i)

if __name__ == '__main__':
    unittest.main()
