#!/usr/bin/env python
from _decorators import *
import os.path
import os

try:
    os.makedirs(os.path.join("include", "RMF"))
except:
    pass

coordinates = Attributes("coordinates", "Float", ["cartesian x",
                                                  "cartesian y",
                                                  "cartesian z"],
                         "Vector3",
                         "The coordinates in angstroms.",
                         bulk=True)
orientation = Attributes("orientation", "Float", ["orientation r",
                                                  "orientation i",
                                                  "orientation j",
                                                  "orientation k"],
                         "Vector4",
                         "The orientation as a quaternion.")
mass = Attribute("mass", "Float", "The mass in Daltons.")
radius = Attribute("radius", "Float", "The radius in angstroms.")

particle = Decorator(["REPRESENTATION"], "physics",
                     "Particle", "These particles has associated coordinates and radius information.",
                     [coordinates, mass, radius])
iparticle = Decorator(["REPRESENTATION"], "physics",
                      "IntermediateParticle",
                      "These particles have associated coordinates and radius information.",
                      [coordinates, radius])
pparticle = Decorator(["REPRESENTATION"], "physics",
                      "RigidParticle", "These particles have associated coordinates and orientation information.",
                      [orientation, coordinates])
refframe = Decorator(["REPRESENTATION", "ORGANIZATIONAL"], "physics",
                     "ReferenceFrame",
                     "Define a transformation to be applied the the attributes of this and child particles, relative to the reference frame of the parent.",
                     [Attributes(
                         "rotation", "Float", ["reference frame orientation r",
                                               "reference frame orientation i",
                                               "reference frame orientation j",
                                               "reference frame orientation k"], "Vector4",
                         "The rotational part of the relative transformation as a quaternion."),
                      Attributes(
                          "translation", "Float", ["reference frame cartesian x",
                                                   "reference frame cartesian y",
                                                   "reference frame cartesian z"], "Vector3",
                          "The translation part of the relative transformion in angstroms.", False)])

bond = Decorator(["BOND"], "physics",
                 "Bond", "A bond between particles.",
                 [AttributePair("bonded", "NodeID", "BondEndpoints", "bond_0", "bond_1", "The bonded particles.")])

old_bond = Decorator(["BOND"], "physics",
                     "OldBond", "A bond between particles, for backwards compatibility.",
                     [Children("bonded", "The bonded particles.")])


atom = Decorator(["REPRESENTATION"], "physics",
                 "Atom", "Information regarding an atom.",
                 [coordinates, mass, radius,
                  Attribute("element", "Index", "The atomic number of the element.")])

diffuser = Decorator(["REPRESENTATION"], "physics",
                     "Diffuser", "Information regarding diffusion coefficients.",
                     [Attribute("diffusion coefficient", "Float", "The diffusion coefficient in A^2/fs.")])

force = Decorator(["REPRESENTATION"], "physics",
                  "Force", "Forces acting on particles in kCal/mol/A.",
                  [Attributes("force", "Float", ["force cartesian x",
                                                 "force cartesian y",
                                                 "force cartesian z"], "Vector3", "The force.")])

torque = Decorator(["REPRESENTATION"], "physics",
                   "Torque", "Torque acting on particles in kCal/mol/radian.",
                   [Attributes("torque", "Float", ["torque cartesian x",
                                                   "torque cartesian y",
                                                   "torque cartesian z"], "Vector3", "The torque.")])

make_header(
    "physics", [particle, iparticle, pparticle, diffuser,
                atom, bond, old_bond, refframe, torque],
    ["alias"])


score = Decorator(["FEATURE"], "feature",
                  "Score", "Associate a score with some set of particles. If the score is an energy, it should be in kJ/mol.",
                  [Children(
                      "representation", "The various components of this score node."),
                   Attribute("score", "Float", "The score.")])

make_header("feature", [score], [])


colored = Decorator(
    ["REPRESENTATION", "ORGANIZATIONAL", "ALIAS", "FEATURE", "GEOMETRY"],
    "shape",
    "Colored", "These particles have associated color information.",
    [Attributes("rgb color", "Float", ["rgb color red",
                                       "rgb color green",
                                       "rgb color blue"], "Vector3",
                "The RGB color. Each component has a value in [0...1].", False)])
geometry_coordinates = Attributes("coordinates", "Floats", ["cartesian xs",
                                                            "cartesian ys",
                                                            "cartesian zs"], "FloatsList",
                                  "Coordinates of the center in angstroms.")

geometry_index = Attribute(
    "type", "Index", "The type of the geometric object.")

ball = Decorator(["GEOMETRY"], "shape",
                 "Ball", "A geometric ball.",
                 [coordinates, radius],
                 internal_attributes=[geometry_index],
                 init_function="nh.set_value(type_, 0);",
                 check_function="nh.get_value(type_) == 0")

cylinder = Decorator(["GEOMETRY"], "shape",
                     "Cylinder", "A geometric cylinder.",
                     [geometry_coordinates, radius],
                     internal_attributes=[geometry_index],
                     init_function="nh.set_value(type_, 1);",
                     check_function="nh.get_value(type_) == 1")

segment = Decorator(["GEOMETRY"], "shape",
                    "Segment", "A geometric line setgment.",
                    [geometry_coordinates],
                    internal_attributes=[geometry_index],
                    init_function="nh.set_value(type_, 1);",
                    check_function="nh.get_value(type_) == 1")


make_header("shape", [colored, ball, cylinder, segment], [])


journal = Decorator(["ORGANIZATIONAL"], "publication",
                    "JournalArticle", "Information regarding a publication.",
                    [Attribute("title", "String", "The article title."),
                     Attribute(
                         "journal", "String", "The journal title."),
                     Attribute("pubmed id", "String", "The pubmed ID."),
                     Attribute("year", "Int", "The publication year."),
                     Attribute("authors", "Strings", "A list of authors as Lastname, Firstname")])

make_header("publication", [journal], [])


residue = Decorator(["REPRESENTATION"], "sequence",
                    "Residue", "Information regarding a residue.",
                    [SingletonRangeAttribute(
                        "index", "Int", "first residue index", "last residue index",
                        "The index of the residue."),
                     Attribute("residue type", "String", "The three letter name for the residue.", function_name="type")])

chain = Decorator(["REPRESENTATION"], "sequence",
                  "Chain", "Information regarding a chain.",
                  [Attribute("chain id", "String", "A string identifying the chain.")])

domain = Decorator(["REPRESENTATION"], "sequence",
                   "Domain", "Information regarding a domain of a molecule.",
                   [RangeAttribute("indexes", "Int", "first residue index",
                                   "last residue index",
                                   "The range for the residues, specified as [first_index...last_index].")])

fragment = Decorator(["REPRESENTATION"], "sequence",
                     "Fragment", "Information regarding a fragment of a molecule.",
                     [Attribute("indexes", "Indexes", "A list of indexes in the fragment")])

copy = Decorator(["REPRESENTATION"], "sequence",
                 "Copy", "Information regarding a copy of a molecule.",
                 [Attribute("copy index", "Index",
                            "This is the copy_indexth copy of the original.")])

typed = Decorator(["REPRESENTATION"], "sequence",
                  "Typed", "A numeric tag for keeping track of types of molecules.",
                  [Attribute("type name", "String", "An arbitrary tag representing the type.")])

make_header("sequence", [residue, chain, fragment, domain, typed, copy], [])


salias = Decorator(["ALIAS"], "alias",
                   "Alias", "Store a reference to another node as an alias.",
                   [NodeAttribute("aliased", "The node that is referenced.")])

make_header("alias", [salias], [])


external = Decorator(["REPRESENTATION"], "external",
                     "External", "A reference to something in an external file. A relative path is stored.",
                     [PathAttribute("path", "The absolute path to the external file.")])

make_header("external", [external], [])
