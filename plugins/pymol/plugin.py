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


class MyData:

    def __init__(self, fh, diameter):
        self.atom_factory = RMF.AtomFactory(fh)
        self.bond_factory = RMF.BondFactory(fh)
        self.particle_factory = RMF.ParticleFactory(fh)
        self.ball_factory = RMF.BallFactory(fh)
        self.segment_factory = RMF.SegmentFactory(fh)
        self.cylinder_factory = RMF.CylinderFactory(fh)
        self.diameter = diameter * 1.5


def _handle_atom(helper, mydata, model, cgol):
    pd = mydata.particle_factory.get(helper)

    at = chempy.Atom()
    at.index = helper.set_is_displayed()
    at.name = helper.get_name()
    ri = helper.get_residue_index()
    if ri is not None:
        at.resn = helper.get_residue_type()
        at.resi = ri
    c = helper.get_rgb_color()
    if c:
        ck = tuple((int(255. * x) for x in c))
        if ck not in colors:
            name = "rmf" + str(len(colors))
            print "color", name, c, ck
            cmd.set_color(name, list(c))
            colors[ck] = name
        at.color = colors[ck]

    cid = helper.get_chain_id()
    if cid:
        at.chain = cid
    coords = [
        x for x in helper.get_global_coordinates(pd.get_coordinates())]
    coords[0] = coords[0] + helper.get_state_index() * mydata.diameter

    at.coord = coords
    at.segi = ""
    if mydata.atom_factory.get_is(helper):
        element = mydata.atom_factory.get(helper).get_element()
        at.symbol = periodic_table[element]
    model.add_atom(at)


def _handle_bond(helper, mydata, model, cgol):
    bd = mydata.bond_factory.get(helper)
    b = chempy.Bond()
    bd0 = bd.get_bonded_0()
    bd1 = bd.get_bonded_1()
    if helper.get_is_displayed(bd0) and helper.get_is_displayed(bd1):
        b.index = [helper.get_index(bd0),
                   helper.get_index(bd1)]
        model.add_bond(b)


def _create_atoms(helper, mydata, model, cgol):
    if helper.get_is_done():
        return
    child = False
    for ch in helper.get_children():
        if _create_atoms(helper.visit(ch), mydata, model, cgol):
            child = True
    if mydata.ball_factory.get_is(helper):
        d = mydata.ball_factory.get(helper)
        c = helper.get_rgb_color()
        if c:
            cgol.extend([cgo.COLOR] + helper.get_color())
        cgol.extend(
            [cgo.SPHERE] + d.get_coordinates() + [d.get_radius()])
    elif mydata.cylinder_factory.get_is(helper):
        d = mydata.cylinder_factory.get(helper)

    elif mydata.segment_factory.get_is(helper):
        pass

    elif mydata.bond_factory.get_is(helper):
        _handle_bond(helper, mydata, model, cgol)
    elif not child and mydata.particle_factory.get_is(helper):
        _handle_atom(helper, mydata, model, cgol)
        child = True
    return child


def _get_molecule_name(name, res):
    name = name.replace(":", "").replace(" ", "").replace(".", "_")
    if name.find("/") != -1:
        name = name.split("/")[-1]
    if res != -1:
        name = name + "-" + str(res)
    return name


def _create_molecules(n, mydata, resolution):
    if n.get_type() == RMF.REPRESENTATION:
        model = models.Indexed()
        name = _get_molecule_name(n.get_name(), resolution)
        print "creating molecule", name
        cgol = []
        _create_atoms(
            RMF.TraverseHelper(n, name, resolution),
            mydata,
            model,
            cgol)
        frame = n.get_file().get_current_frame().get_index() + 1
        cmd.load_model(model, name, frame)
        if len(cgol) > 0:
            print cgol
            cmd.load_cgo(cgol, name + "-graphics", frame)
    else:
        for c in n.get_children():
            _create_molecules(c, mydata, resolution)


def _do_it(path):
        fh = RMF.open_rmf_file_read_only(path)
        res = RMF.get_resolutions(fh.get_root_node(), RMF.PARTICLE, .1)
        if len(res) == 1:
            res = [-1]
        fh.set_current_frame(RMF.FrameID(0))
        diameter = RMF.get_diameter(fh.get_root_node())
        mydata = MyData(fh, diameter)
        for f in fh.get_frames():
            fh.set_current_frame(f)
            for r in res:
                _create_molecules(fh.get_root_node(), mydata, r)


def _open_rmf(path):
    # exec("_do_it(path)")
    cProfile.runctx("_do_it(path)", globals(), locals())
cmd.extend('rmf', _open_rmf)
