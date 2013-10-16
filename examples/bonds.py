## \example bonds.py
## Show how to extract bonds from a file.
import RMF

def print_bonds(node, bf):
    if (bf.get_is(node)):
        bd = bf.get(node)
        print "bond", bd.get_bonded()
    else:
        for c in node.get_children():
            print_bonds(c, bf)

fh = RMF.open_rmf_file_read_only(RMF.get_example_path("simple.rmf"))
fh.set_current_frame(RMF.FrameID(0))
bf = RMF.BondConstFactory(fh)

print_bonds(fh.get_root_node(), bf)
