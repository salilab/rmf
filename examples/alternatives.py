## \example alternatives.py
# The RMF format supports storing alternative representations of the structure
# through the RMF::decorator::Alternatives decorator and associated types.
# Multiple versions of the hierarchy are stored, all representing the same
# biological entity. Different representations can be used for different
# purposes based on resolution or other criteria.

import RMF

tfn = RMF._get_temporary_file_path("alternatives.rmf3")
print "File is", tfn

f = RMF.create_rmf_file(tfn)
f.add_frame("frame", RMF.FRAME)

# make a trivial set of multiple resolution represntations
rh = f.get_root_node()

pf = RMF.ParticleFactory(f)


def frange(x, y, jump):
    while x < y:
        yield x
        x += jump


def create_hierarchy(radius):
    root = f.add_node(str(radius), RMF.REPRESENTATION)
    for i in frange(0., 16, radius):
        for j in frange(0., 16, radius):
            for k in frange(0., 16, radius):
                center = RMF.Vector3(
                    i + radius / 2,
                    j + radius / 2,
                    k + radius / 2)
                n = root.add_child(
                    str(i) + "-" + str(j) + "-" + str(k),
                    RMF.REPRESENTATION)
                d = pf.get(n)
                d.set_static_radius(radius)
                d.set_frame_coordinates(center)
                d.set_static_mass(radius ** 3)
    return root

n = create_hierarchy(4)
rh.add_child(n)

af = RMF.AlternativesFactory(f)
da = af.get(n)
#da.add_alternative(create_hierarchy(1), RMF.PARTICLE)
#da.add_alternative(create_hierarchy(2), RMF.PARTICLE)
#da.add_alternative(create_hierarchy(4), RMF.PARTICLE)
da.add_alternative(create_hierarchy(8), RMF.PARTICLE)
