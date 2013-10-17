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
                         "The coordinates in angstroms.", False,
                         bulk=True)
static_coordinates = Attributes("coordinates", "Float", ["cartesian x",
                                                         "cartesian y",
                                                         "cartesian z"],
                                "Vector3",
                                "The coordinates in angstroms.", True,
                                bulk=True)
orientation = Attributes("orientation", "Float", ["orientation r",
                                                  "orientation i",
                                                  "orientation j",
                                                  "orientation k"],
                         "Vector4",
                         "The orientation as a quaternion.", False)
static_orientation = Attributes("orientation", "Float", ["orientation r",
                                                         "orientation i",
                                                         "orientation j",
                                                         "orientation k"],
                                "Vector4",
                                "The orientation as a quaternion.", True)
mass = Attribute("mass", "Float", "The mass in Daltons.", True)
radius = Attribute("radius", "Float", "The radius in angstroms.", True)

particle = Decorator(["REPRESENTATION"], "physics",
                     "Particle", "These particles has associated coordinates and radius information.",
                     [coordinates, mass, radius])
iparticle = Decorator(["REPRESENTATION"], "physics",
                      "IntermediateParticle",
                      "These particles have associated coordinates and radius information.",
                      [coordinates, radius])
static_particle = Decorator(["REPRESENTATION"], "physics",
                            "StaticParticle", "These particles has associated coordinates and radius information.",
                            [static_coordinates, mass, radius])
static_iparticle = Decorator(["REPRESENTATION"], "physics",
                             "StaticIntermediateParticle",
                             "These particles have associated coordinates and radius information.",
                             [static_coordinates, radius])
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
                         "The rotational part of the relative transformation as a quaternion.", False),
                      Attributes(
                          "translation", "Float", ["reference frame cartesian x",
                                                   "reference frame cartesian y",
                                                   "reference frame cartesian z"], "Vector3",
                          "The translation part of the relative transformion in angstroms.", False)])
static_refframe = Decorator(["REPRESENTATION", "ORGANIZATIONAL"], "physics",
                            "StaticReferenceFrame",
                            "Define a transformation to be applied the the attributes of this and child particles, relative to the reference frame of the parent.",
                            [Attributes(
                                "rotation", "Float", ["reference frame orientation r",
                                                      "reference frame orientation i",
                                                      "reference frame orientation j",
                                                      "reference frame orientation k"], "Vector4",
                                "The rotational part of the relative transformation as a quaternion.", True),
                                Attributes(
                                    "translation", "Float", ["reference frame cartesian x",
                                                             "reference frame cartesian y",
                                                             "reference frame cartesian z"], "Vector3",
                                    "The translation part of the relative transformion in angstroms.", True)])

bond = Decorator(["BOND"], "physics",
                 "Bond", "A bond between particles.",
                 [Children("bonded", "The bonded particles.", True)])

atom = Decorator(["REPRESENTATION"], "physics",
                 "Atom", "Information regarding an atom.",
                 [coordinates, mass, radius,
                  Attribute("element", "Index", "The atomic number of the element.", True)])

diffuser = Decorator(["REPRESENTATION"], "physics",
                     "Diffuser", "Information regarding diffusion coefficients.",
                     [Attribute("diffusion coefficient", "Float", "The diffusion coefficient in A^2/fs.", True)])

force = Decorator(["REPRESENTATION"], "physics",
                  "Force", "Forces acting on particles in kCal/mol/A.",
                  [Attributes("force", "Float", ["force cartesian x",
                                                 "force cartesian y",
                                                 "force cartesian z"], "Vector3", "The force.", False)])

torque = Decorator(["REPRESENTATION"], "physics",
                   "Torque", "Torque acting on particles in kCal/mol/radian.",
                   [Attributes("torque", "Float", ["torque cartesian x",
                                                   "torque cartesian y",
                                                   "torque cartesian z"], "Vector3", "The torque.", False)])

make_header(
    "physics", [static_particle, particle, static_iparticle, iparticle, pparticle, diffuser,
                atom, bond, refframe, static_refframe, force, torque],
    ["alias"])


score = Decorator(["FEATURE"], "feature",
                  "Score", "Associate a score with some set of particles. If the score is an energy, it should be in kJ/mol.",
                  [Children(
                      "representation", "The various components of this score node.", True),
                   Attribute("score", "Float", "The score.", False)])

make_header("feature", [score], [])


colored = Decorator(
    ["REPRESENTATION", "ORGANIZATIONAL", "ALIAS", "FEATURE", "GEOMETRY"],
    "shape",
    "Colored", "These particles have associated color information.",
    [Attributes("rgb color", "Float", ["rgb color red",
                                       "rgb color green",
                                       "rgb color blue"], "Vector3",
                "The RGB color. Each component has a value in [0...1].", False)])
static_colored = Decorator(
    ["REPRESENTATION", "ORGANIZATIONAL", "ALIAS", "FEATURE", "GEOMETRY"],
    "shape",
    "StaticColored", "These particles have associated color information.",
    [Attributes("rgb color", "Float", ["rgb color red",
                                       "rgb color green",
                                       "rgb color blue"], "Vector3",
                "The RGB color. Each component has a value in [0...1].", True)])
geometry_coordinates = Attributes("coordinates", "Floats", ["cartesian xs",
                                                            "cartesian ys",
                                                            "cartesian zs"], "FloatsList",
                                  "Coordinates of the center in angstroms.", False)
static_geometry_coordinates = Attributes(
    "coordinates", "Floats", ["cartesian xs",
                              "cartesian ys",
                              "cartesian zs"], "FloatsList",
    "Coordinates of the center in angstroms.", True)
geometry_index = Attribute(
    "type", "Index", "The type of the geometric object.", True)

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

static_ball = Decorator(["GEOMETRY"], "shape",
                        "StaticBall", "A geometric ball.",
                        [static_coordinates, radius],
                        internal_attributes=[geometry_index],
                        init_function="nh.set_value(type_, 0);",
                        check_function="nh.get_value(type_) == 0")

static_cylinder = Decorator(["GEOMETRY"], "shape",
                            "StaticCylinder", "A geometric cylinder.",
                            [static_geometry_coordinates, radius],
                            internal_attributes=[geometry_index],
                            init_function="nh.set_value(type_, 1);",
                            check_function="nh.get_value(type_) == 1")

static_segment = Decorator(["GEOMETRY"], "shape",
                           "StaticSegment", "A geometric line setgment.",
                           [static_geometry_coordinates],
                           internal_attributes=[geometry_index],
                           init_function="nh.set_value(type_, 1);",
                           check_function="nh.get_value(type_) == 1")


make_header("shape", [colored, ball, cylinder, segment,
            static_colored, static_ball, static_cylinder, static_segment], [])


journal = Decorator(["ORGANIZATIONAL"], "publication",
                    "JournalArticle", "Information regarding a publication.",
                    [Attribute("title", "String", "The article title.", True),
                     Attribute(
                         "journal", "String", "The journal title.", True),
                     Attribute("pubmed id", "String", "The pubmed ID.", True),
                     Attribute("year", "Int", "The publication year.", True),
                     Attribute("authors", "Strings", "A list of authors as Lastname, Firstname", True)])

make_header("publication", [journal], [])


residue = Decorator(["REPRESENTATION"], "sequence",
                    "Residue", "Information regarding a residue.",
                    [SingletonRangeAttribute(
                        "index", "Int", "first residue index", "last residue index",
                        "The index of the residue.", True),
                     Attribute("residue type", "String", "The three letter name for the residue.", True, function_name="type")])

chain = Decorator(["REPRESENTATION"], "sequence",
                  "Chain", "Information regarding a chain.",
                  [Attribute("chain id", "Index", "The one letter id for the chain (A is 0 etc).", True)])

domain = Decorator(["REPRESENTATION"], "sequence",
                   "Domain", "Information regarding a domain of a molecule.",
                   [RangeAttribute("indexes", "Int", "first residue index",
                                   "last residue index",
                                   "The range for the residues, specified as [first_index...last_index].", True)])

fragment = Decorator(["REPRESENTATION"], "sequence",
                     "Fragment", "Information regarding a fragment of a molecule.",
                     [Attribute("indexes", "Indexes", "A list of indexes in the fragment", True)])

copy = Decorator(["REPRESENTATION"], "sequence",
                 "Copy", "Information regarding a copy of a molecule.",
                 [Attribute("copy index", "Index",
                            "This is the copy_indexth copy of the original.", True)])

typed = Decorator(["REPRESENTATION"], "sequence",
                  "Typed", "A numeric tag for keeping track of types of molecules.",
                  [Attribute("type name", "String", "An arbitrary tag representing the type.", True)])

make_header("sequence", [residue, chain, fragment, domain, typed, copy], [])


salias = Decorator(["ALIAS"], "alias",
                   "Alias", "Store a reference to another node as an alias.",
                   [NodeAttribute("aliased", "The node that is referenced.", True)])

make_header("alias", [salias], [])


external = Decorator(["REPRESENTATION"], "external",
                     "External", "A reference to something in an external file. A relative path is stored.",
                     [PathAttribute("path", "The absolute path to the external file.", True)])

make_header("external", [external], [])
