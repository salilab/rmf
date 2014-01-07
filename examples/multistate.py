# \example multistate.py
# Show how to create an detect files where frames represent multiple
# states of the same thing.

import RMF

tfn = RMF._get_temporary_file_path("multistate.rmf3")
print "File is", tfn

fh = RMF.create_rmf_file(tfn)

rh = fh.get_root_node()

pf = RMF.ParticleFactory(fh)

for i in range(0, 3):
    n = rh.add_child(str(i), RMF.REPRESENTATION)
    pf.get(n).set_static_mass(1)
    pf.get(n).set_static_radius(1)


def fill_coordinates(frame, subframe):
    for i, n in enumerate(rh.get_children()):
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

del pf
del rh
del fh
del n

fh = RMF.open_rmf_file_read_only(tfn)

for f in fh.get_root_frames():
    print f, "has subframes", fh.get_children(f)
