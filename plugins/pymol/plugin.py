from pymol import cmd
from pymol import cgo
from chempy import models
import cProfile
import RMF

colors = {}

periodic_table = ["INV", "H", "He", "Li", "Be", "B", "C", "N", "O",
                  "F", "Ne", "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",
                  "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge",
                  "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru",
                  "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I", "Xe", "Cs", "Ba",
                  "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho",
                  "Er", "Tm", "Yb", "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au",
                  "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
                  "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No",
                  "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn", "Uut",
                  "Fl", "Uup", "Lv", "Uus", "Uuo"]


class RecurseData:

    def __init__(self, fh, init=True):
        self.file = fh
        if init:
            self.chain_factory = RMF.ChainFactory(fh)
            self.residue_factory = RMF.ResidueFactory(fh)
            self.atom_factory = RMF.AtomFactory(fh)
            self.bond_factory = RMF.BondFactory(fh)
            self.particle_factory = RMF.ParticleFactory(fh)
            self.colored_factory = RMF.ColoredFactory(fh)
            self.state_factory = RMF.StateFactory(fh)
            self.reference_frame_factory = RMF.ReferenceFrameFactory(fh)
            self.alternatives_factory = RMF.AlternativesFactory(fh)
            self.ball_factory = RMF.BallFactory(fh)
            self.segment_factory = RMF.SegmentFactory(fh)
            self.cylinder_factory = RMF.CylinderFactory(fh)
            self.color_name = None
            self.color = None
            self.color_key = None
            self.diameter = RMF.get_diameter(fh.get_root_node()) * 1.5
            self.coordinate_transformer = RMF.CoordinateTransformer()
            self.molecule_name = None
            self.residue_index = None
            self.residue_type = None
            self.chain_id = ""
            self.state = 0
            self.segment = ""
            self.index = {}
            self.resolution = -1

    def recurse(self, **kwargs):
        ret = RecurseData(self.file, init=False)
        rvars = vars(ret)
        myvars = vars(self)
        for v in myvars:
            rvars[v] = myvars[v]
        for v in kwargs:
            rvars[v] = kwargs[v]
        return ret


def _create_atoms(n, model, cgol, data):
    if data.resolution != -1 and data.alternatives_factory.get_is(n):
        n = data.alternatives_factory.get(
            n).get_alternative(data.resolution,
                               RMF.PARTICLE)
    if data.reference_frame_factory.get_is(n):
        data = data.recurse(coordinate_transformer=RMF.CoordinateTransformer(
            data.coordinate_transformer,
            data.reference_frame_factory.get(n)))
    if data.state_factory.get_is(n):
        data = data.recurse(state=data.state_factory.get(n).get_state())
    if data.colored_factory.get_is(n):
        c = data.colored_factory.get(n).get_rgb_color()
        ck = tuple((int(255. * x) for x in c))
        if data.color_key != ck:
            if ck not in colors:
                name = "rmf" + str(len(colors))
                print "color", name, c, ck
                cmd.set_color(name, list(c))
                colors[ck] = name
            data = data.recurse(color_name=colors[ck], color=c)
    if data.chain_factory.get_is(n):
        data = data.recurse(chain_id=data.chain_factory.get(n).get_chain_id())
    if data.ball_factory.get_is(n):
        d = data.ball_factory.get(n)
        cgol.extend([cgo.COLOR] + data.color)
        cgol.extend(
            [cgo.SPHERE,
             d.get_coordinates()[0],
             d.get_coordinates()[1],
             d.get_coordinates()[2],
             d.get_radius()])
    if data.cylinder_factory.get_is(n):
        d = data.cylinder_factory.get(n)

    if data.segment_factory.get_is(n):
        pass
    if data.residue_factory.get_is(n):
        rd = data.residue_factory.get(n)
        data = data.recurse(
            residue_index=rd.get_residue_index(),
            residue_name=rd.get_residue_type())
    child = False
    for ch in n.get_children():
        if _create_atoms(ch, model, cgol, data):
            child = True
    if data.bond_factory.get_is(n):
        bd = data.bond_factory.get(n)
        b = chempy.Bond()
        b.index = [
            data.index[bd.get_bonded_0()], data.index[bd.get_bonded_1()]]
        model.add_bond(b)
    if not child and data.particle_factory.get_is(n):
        pd = data.particle_factory.get(n)
        element = 0
        if data.atom_factory.get_is(n):
            element = data.atom_factory.get(n).get_element()

        coords = [x for x in pd.get_coordinates()]
        coords[0] = coords[0] + data.state * data.diameter
        at = chempy.Atom()
        at.index = n.get_id().get_index()
        at.name = n.get_name()
        if data.residue_index is not None:
            at.resn = data.residue_type
            at.resi = data.residue_index
        if data.color_name is not None:
            at.color = data.color_name
        at.chain = data.chain_id
        at.coord = data.coordinate_transformer.get_global_coordinates(coords)
        at.segi = data.segment
        at.symbol = periodic_table[element]
        model.add_atom(at)
        index = len(data.index)
        data.index[n] = index
        child = True
    return child


def _get_molecule_name(name, res):
    name = name.replace(":", "").replace(" ", "").replace(".", "_")
    if name.find("/") != -1:
        name = name.split("/")[-1]
    if res != -1:
        name = name + "-" + str(res)
    return name


def _create_molecules(n, data):
    if n.get_type() == RMF.REPRESENTATION:
        model = models.Indexed()
        name = _get_molecule_name(n.get_name(), data.resolution)
        print "creating molecule", name
        cgol = []
        _create_atoms(n, model, cgol, data.recurse(molecule_name=name))
        frame = n.get_file().get_current_frame().get_index() + 1
        cmd.load_model(model, name, frame)
        if len(cgol) > 0:
            print cgol
            cmd.load_cgo(cgol, name + "-graphics", frame)
    else:
        for c in n.get_children():
            _create_molecules(c, data)


def _do_it(path):
        fh = RMF.open_rmf_file_read_only(path)
        res = RMF.get_resolutions(fh.get_root_node(), RMF.PARTICLE, .1)
        if len(res) == 1:
            res = [-1]
        fh.set_current_frame(RMF.FrameID(0))
        data = RecurseData(fh)
        for f in fh.get_frames():
            fh.set_current_frame(f)
            for r in res:
                data.resolution = r
                _create_molecules(fh.get_root_node(), data)


def _open_rmf(path):
    # exec("_do_it(path)")
    cProfile.runctx("_do_it(path)", globals(), locals())
cmd.extend('rmf', _open_rmf)
