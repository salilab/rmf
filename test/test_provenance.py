from __future__ import print_function
import sys
import RMF
import unittest
import os

class Tests(unittest.TestCase):

    def test_provenance(self):
        """Test the Provenance decorator"""
        for suffix in RMF.suffixes:
            fname = RMF._get_temporary_file_path("provenance." + suffix)
            self._create(fname)
            self._read(fname)

    def _add_provenance_nodes(self, rmf, rt):
        """Add *Provenance nodes under rt. Return the root."""
        strucpf = RMF.StructureProvenanceFactory(rmf)
        samppf = RMF.SampleProvenanceFactory(rmf)
        clustpf = RMF.ClusterProvenanceFactory(rmf)

        clust_node = rt.add_child("clustering", RMF.PROVENANCE)
        clust = clustpf.get(clust_node)
        clust.set_members(10)

        samp_node = clust_node.add_child("sampling", RMF.PROVENANCE)
        samp = samppf.get(samp_node)
        samp.set_method("Monte Carlo")
        samp.set_frames(100)
        samp.set_iterations(10)

        struc_node = samp_node.add_child("structure", RMF.PROVENANCE)
        struc = strucpf.get(struc_node)
        struc.set_filename('foo.pdb')
        struc.set_chain('X')

        return clust_node

    def _create(self, fname):
        rmf = RMF.create_rmf_file(fname)
        rmf.add_frame('zero', RMF.FRAME)
        rt = rmf.get_root_node()

        pf = RMF.ProvenanceFactory(rmf)
        c0 = self._add_provenance_nodes(rmf, rt)
        c1 = rt.add_child("c1", RMF.REPRESENTATION)
        prov = pf.get(c1)
        prov.set_provenance(c0)
        self._check_provenance_nodes(rmf, c0, c1)
        self.assertEqual(prov.get_provenance(), c0)
        self.assertFalse(pf.get_is(c0))
        self.assertTrue(pf.get_is(c1))

    def _read(self, fname):
        rmf = RMF.open_rmf_file_read_only(fname)
        rt = rmf.get_root_node()
        c0, c1 = rt.get_children()
        self._check_provenance_nodes(rmf, c0, c1)

    def _check_provenance_nodes(self, rmf, prov_root, rep):
        pf = RMF.ProvenanceFactory(rmf)
        prov = pf.get(rep)
        self.assertEqual(prov.get_provenance(), prov_root)
        self.assertFalse(pf.get_is(prov_root))
        self.assertTrue(pf.get_is(rep))

        strucpf = RMF.StructureProvenanceFactory(rmf)
        samppf = RMF.SampleProvenanceFactory(rmf)
        clustpf = RMF.ClusterProvenanceFactory(rmf)

        self.assertTrue(clustpf.get_is(prov_root))
        clust = clustpf.get(prov_root)
        self.assertEqual(clust.get_members(), 10)

        samp_node = prov_root.get_children()[0]
        self.assertTrue(samppf.get_is(samp_node))
        samp = samppf.get(samp_node)
        self.assertEqual(samp.get_frames(), 100)
        self.assertEqual(samp.get_iterations(), 10)
        self.assertEqual(samp.get_method(), "Monte Carlo")

        struc_node = samp_node.get_children()[0]
        self.assertTrue(strucpf.get_is(struc_node))
        struc = strucpf.get(struc_node)
        self.assertEqual(struc.get_filename(), 'foo.pdb')
        self.assertEqual(struc.get_chain(), 'X')

if __name__ == '__main__':
    unittest.main()
