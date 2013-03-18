#!/usr/bin/env python
from _decorators import *
import os.path
import os

colored= Decorator(["REPRESENTATION", "ORGANIZATIONAL", "ALIAS", "FEATURE", "GEOMETRY"],
"Colored", "These particles have associated color information.",
                   [DecoratorCategory("shape", [Attributes("Float", "Floats",
                                                              "rgb_color", ["rgb color red",
                                                                            "rgb color green",
                                                                            "rgb color blue"],
                                                           "The RGB color. Each component has a value in [0...1].")])],
                   "")
particle= Decorator(["REPRESENTATION"], "Particle", "These particles has associated coordinates and radius information.",
                   [DecoratorCategory("physics", [Attributes("Float", "Floats",
                                                                "coordinates", ["cartesian x",
                                                                                "cartesian y",
                                                                                "cartesian z"],
                                                             "The coordinates in angstroms."),
                                                     Attribute("Float", "radius", "radius", "The radius in angstroms."),
                                                     Attribute("Float", "mass", "mass", "The mass in Daltons.")])],
                   "")
iparticle= Decorator(["REPRESENTATION"], "IntermediateParticle", "These particles have associated coordinates and radius information.",
                   [DecoratorCategory("physics", [Attributes("Float", "Floats",
                                                                "coordinates", ["cartesian x",
                                                                                "cartesian y",
                                                                                "cartesian z"],
                                                             "The coordinates in angstroms."),
                                                     Attribute("Float", "radius", "radius",
                                                               "The radius in angstroms.")])],
                   "")
pparticle= Decorator(["REPRESENTATION"], "RigidParticle", "These particles have associated coordinates and orientation information.",
                   [DecoratorCategory("physics", [Attributes("Float", "Floats",
                                                                "orientation", ["orientation r",
                                                                                "orientation i",
                                                                                "orientation j",
                                                                                "orientation k"],
                                                             "The orientation as a quaternion."),
                                                     Attributes("Float", "Floats",
                                                                "coordinates", ["cartesian x",
                                                                                "cartesian y",
                                                                                "cartesian z"],
                                                                "The coordinates of the center in angstroms.")])],
                   "")
refframe= Decorator(["REPRESENTATION", "ORGANIZATIONAL"],
"ReferenceFrame", "Define a transformation to be applied the the attributes of this and child particles, relative to the reference frame of the parent.",
                   [DecoratorCategory("physics",
                                      [Attributes("Float", "Floats",
                                                  "rotation", ["reference frame orientation r",
                                                               "reference frame orientation i",
                                                               "reference frame orientation j",
                                                               "reference frame orientation k"],
                                                  "The rotational part of the relative transformation as a quaternion."),
                                        Attributes("Float", "Floats",
                                                   "translation", ["reference frame cartesian x",
                                                                   "reference frame cartesian y",
                                                                   "reference frame cartesian z"],
                                                   "The translation part of the relative transformion in angstroms.")])],
    "")

score= Decorator(["FEATURE"], "Score", "Associate a score with some set of particles.",
                   [DecoratorCategory("feature", [Children("representation", "The various components of this score node."),
                                                  Attribute("Float", "score", "score", "The score.")])],
                   "")

bond= Decorator(["BOND"], "Bond", "A bond between particles.",
                   [DecoratorCategory("physics", [Children("bonded", "The bonded particles.")])],
                   "")

ball= Decorator(["GEOMETRY"], "Ball", "A geometric ball.",
                   [DecoratorCategory("shape", [PluralAttributes("Float", "Floats",
                                                              "coordinates", ["cartesian x",
                                                                              "cartesian y",
                                                                              "cartesian z"],
                                                                 "Coordinates of the center in angstroms."),
                                                   Attribute("Float", "radius", "radius", "The radius in angstroms.")],
                                      internal_attributes=[Attribute("Index", "type", "type", "The type of the geometric object.")])],
                   ["nh.set_value(type_, 0);"], ["nh.get_value(type_)==0"])
cylinder= Decorator(["GEOMETRY"], "Cylinder", "A geometric cylinder.",
                   [DecoratorCategory("shape", [PluralAttributes("Floats", "FloatsList",
                                                              "coordinates", ["cartesian xs",
                                                                              "cartesian ys",
                                                                              "cartesian zs"],
                                                                 "The coordinates of the endpoints in angstroms. The returned list has 3 components, a list of x coordinates, a list of y coordinates and a list of z coordinates."),
                                                   Attribute("Float", "radius", "radius", "The radius in angstroms.")],
                                      internal_attributes=[Attribute("Index", "type", "type", "The type of the geometric object.")])],
                   ["nh.set_value(type_, 1);"], ["nh.get_value(type_)==1"])

segment= Decorator(["GEOMETRY"], "Segment", "A geometric line setgment.",
                   [DecoratorCategory("shape", [PluralAttributes("Floats", "FloatsList",
                                                              "coordinates", ["cartesian xs",
                                                                              "cartesian ys",
                                                                              "cartesian zs"], "The coordinates of the endpoints in angstroms. The returned list has 3 components, a list of x coordinates, a list of y coordinates and a list of z coordinates.")],
                                      internal_attributes=[Attribute("Index", "type", "type", "The type of the geometric object.")])],
                    ["nh.set_value(type_, 1);"], ["nh.get_value(type_)==1"])

journal= Decorator(["ORGANIZATIONAL"], "JournalArticle", "Information regarding a publication.",
                   [DecoratorCategory("publication", [Attribute("String", "title", "title", "The article title."),
                                                         Attribute("String", "journal", "journal", "The journal title."),
                                                         Attribute("String", "pubmed_id", "pubmed id", "The pubmed ID."),
                                                         Attribute("Int", "year", "year", "The publication year."),
                                                         Attribute("Strings", "authors", "authors", "A list of authors as Lastname, Firstname"),])],
                   "")

atom= Decorator(["REPRESENTATION"], "Atom", "Information regarding an atom.",
                   [DecoratorCategory("physics", [Attributes("Float", "Floats",
                                                                "coordinates", ["cartesian x",
                                                                                "cartesian y",
                                                                                "cartesian z"], "The coordinates in angstroms."),
                                                     Attribute("Float", "radius", "radius", "The radius in angstroms."),
                                                     Attribute("Float", "mass", "mass", "The mass in Daltons."),
                                                     Attribute("Index", "element", "element", "The atomic number of the element.")])],
                   "")


residue= Decorator(["REPRESENTATION"], "Residue", "Information regarding a residue.",
                   [DecoratorCategory("sequence", [SingletonRangeAttribute("Int", "index", "first residue index", "last residue index",
                                                                           "The index of the residue."),
                                                   Attribute("String", "type", "residue type", "The three letter name for the residue.")])],
                   "")

chain= Decorator(["REPRESENTATION"], "Chain", "Information regarding a chain.",
                   [DecoratorCategory("sequence", [Attribute("Index", "chain_id", "chain id", "The one letter id for the chain (A is 0 etc).")])],
                   "")

fragment= Decorator(["REPRESENTATION"], "Domain", "Information regarding a fragment of a molecule.",
                   [DecoratorCategory("sequence", [RangeAttribute("Int", "indexes", "first residue index", "last residue index", "The range for the residues, specified as [first_index...last_index].")])],
                   "")

copy= Decorator(["REPRESENTATION"], "Copy", "Information regarding a copy of a molecule.",
                   [DecoratorCategory("sequence", [Attribute("Index", "copy_index", "copy index", "This is the copy_indexth copy of the original.")])],
                   "")
diffuser= Decorator(["REPRESENTATION"], "Diffuser", "Information regarding diffusion coefficients.",
                   [DecoratorCategory("physics", [Attribute("Float", "diffusion_coefficient", "diffusion coefficient", "The diffusion coefficient in XXXX.")])],
                   "")
typed= Decorator(["REPRESENTATION"], "Typed", "A numeric tag for keeping track of types of molecules.",
                   [DecoratorCategory("sequence", [Attribute("String", "type_name", "type name", "An arbitrary tag representing the type.")])],
                   "")

salias= Decorator(["ALIAS"], "Alias", "Store a reference to another node as an alias.",
                  [DecoratorCategory("alias", [NodeAttribute("NodeID", "aliased", "aliased", "The node that is referenced.")])],
                   "")
external= Decorator(["REPRESENTATION"], "External", "A reference to something in an external file. A relative path is stored.",
                   [DecoratorCategory("external", [PathAttribute("String", "path", "path", "The absolute path to the external file.")])],
                   "")
try:
  os.makedirs(os.path.join("include", "RMF"))
except:
  pass
make_header("physics", [particle, iparticle, pparticle, diffuser,  bond, refframe], ["alias"])
make_header("sequence", [residue, atom, chain, fragment, typed, copy], [])
make_header("geometry", [colored, ball, cylinder, segment], [])
make_header("external", [external], [])
make_header("feature", [score], [])
make_header("alias", [salias], [])
make_header("publication", [journal], [])
