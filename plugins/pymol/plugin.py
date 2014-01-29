from pymol import cmd
from chempy import models
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
            self.reference_frame_factory = RMF.ReferenceFrameFactory(fh)
            self.color = ""
            self.coordinate_transformer = RMF.CoordinateTransformer()
            self.molecule_name = ""
            self.residue_index = 1
            self.residue_type = ""
            self.chain_id = ""
            self.segment = ""
            self.index = {}

    def recurse(self, **kwargs):
        ret = RecurseData(self.file, init=False)
        rvars = vars(ret)
        myvars = vars(self)
        for v in myvars:
            rvars[v] = myvars[v]
        for v in kwargs:
            rvars[v] = kwargs[v]
        return ret


def _create_atoms(n, model, data):
    if data.chain_factory.get_is(n):
        data.chain_id = data.chain_factory.get(n).get_chain_id()

    if data.residue_factory.get_is(n):
        rd = data.residue_factory.get(n)
        data.residue_index = rd.get_residue_index()
        data.residue_name = rd.get_residue_type()
    if data.reference_frame_factory:
        pass

    child = False
    for ch in n.get_children():
        if _create_atoms(ch, model, data):
            child = True
    if data.bond_factory.get_is(n):
        bd = data.bond_factory.get(n)
        b = chempy.Bond()
        b.index = [data.index[bd.get_bonded_0()], data.index[bd.get_bonded_1()]]
        model.add_bond(b)
    if not child and data.particle_factory.get_is(n):
        pd = data.particle_factory.get(n)
        element = ""
        if data.atom_factory.get_is(n):
            element = data.atom_factory.get(n).get_element()

        coords = [x for x in pd.get_coordinates()]

        at = chempy.Atom()
        at.index = n.get_id().get_index()
        at.name = n.get_name()
        at.resn = data.residue_type
        at.resi = data.residue_index
        at.chain = data.chain_id
        at.coord = coords
        at.segi = data.segment
        # fix later
        at.symbol = periodic_table[element]
        model.add_atom(at)
        index = model.index_atom(at)
        data.index[n] = index
        child = True
    return child


def _get_molecule_name(name):
    if name.find("/") != -1:
        return name.split("/")[-1].replace(":", "").replace(" ", "")
    else:
        return name.replace(":", "").replace(" ", "")


def _create_molecules(n, data):
    if n.get_type() == RMF.REPRESENTATION:
        model = models.Indexed()
        name = _get_molecule_name(n.get_name())
        print "creating molecule", name
        _create_atoms(n, model, data.recurse(molecule_name=name))
        cmd.load_model(model, name)
    else:
        for c in n.get_children():
            _create_molecules(c, data)


def _open_rmf(path):
    fh = RMF.open_rmf_file_read_only(path)
    data = RecurseData(fh)
    for f in fh.get_frames():
        fh.set_current_frame(f)
        _create_molecules(fh.get_root_node(), data)

cmd.extend('rmf', _open_rmf)
