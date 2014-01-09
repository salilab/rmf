## \example multistate.py
# Show how to create an detect files where frames represent multiple
# states of the same thing.

import RMF

tfn = RMF._get_temporary_file_path("multistate.rmf3")
print "File is", tfn


def create_rmf(tfn):

    fh = RMF.create_rmf_file(tfn)
    rh = fh.get_root_node()
    pf = RMF.ParticleFactory(fh)
    bf = RMF.BondFactory(fh)

    ns = []
    for i in range(0, 3):
        n = rh.add_child(str(i), RMF.REPRESENTATION)
        ns.append(n)
        pf.get(n).set_static_mass(1)
        pf.get(n).set_static_radius(1)
    for i in range(0, 2):
        b = rh.add_child("b", RMF.BOND)
        bd = bf.get(b)
        bd.set_bonded_0(ns[i].get_id().get_index())
        bd.set_bonded_1(ns[i + 1].get_id().get_index())

    def fill_coordinates(frame, subframe):
        for i, n in enumerate(ns):
            pf.get(n).set_coordinates(RMF.Vector3(i + frame + .1 * subframe,
                                                  frame + .1 * subframe,
                                                  frame + .1 * subframe))
    # create 4 frames with 3 different configurations each
    for i in range(0, 4):
        cur_frame = fh.add_frame(str(i), RMF.FRAME)
        fill_coordinates(i, 0)
        for j in range(0, 2):
            fh.add_frame(str(i) + "-" + str(j), cur_frame, RMF.ALTERNATE)
            fill_coordinates(i, j + 1)

create_rmf(tfn)

fh = RMF.open_rmf_file_read_only(tfn)

for f in fh.get_root_frames():
    print f, "has subframes", fh.get_children(f)
