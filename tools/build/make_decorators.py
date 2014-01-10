#!/usr/bin/env python
from _decorators import *
import os.path
import os

try:
    os.makedirs(os.path.join("include", "RMF"))
except:
    pass

coordinates = Attribute(
    "coordinates",
    "Vector3")

orientation = Attribute(
    "orientation",
    "Vector4")
mass = Attribute("mass", "Float")
radius = Attribute("radius", "Float")

particle = Decorator(["REPRESENTATION"], "physics",
                     "Particle",
                     [mass, coordinates, radius])
iparticle = Decorator(["REPRESENTATION"], "physics",
                      "IntermediateParticle",
                      [radius, coordinates])
pparticle = Decorator(["REPRESENTATION"], "physics",
                      "RigidParticle",
                      [orientation, coordinates])
refframe = Decorator(["REPRESENTATION", "ORGANIZATIONAL"], "physics",
                     "ReferenceFrame",
                     [Attribute(
                         "rotation", "Vector4"),
                         Attribute(
                             "translation", "Vector3")])

bond = Decorator(["BOND"], "physics",
                 "Bond",
                 [Attribute(
                     "bonded 0", "Int"),
                  Attribute("bonded 1", "Int")])


atom = Decorator(["REPRESENTATION"], "physics",
                 "Atom",
                 [Attribute("element", "Int"), coordinates, mass, radius
                  ])

diffuser = Decorator(["REPRESENTATION"], "physics",
                     "Diffuser",
                     [Attribute("diffusion coefficient", "Float")])

make_header(
    "physics", [particle, iparticle, pparticle, diffuser,
                atom, bond, refframe],
    ["alias"])


score = Decorator(["FEATURE"], "feature",
                  "Score",
                  [Attribute("score", "Float")])
representation = Decorator(["FEATURE"], "feature",
                           "Representation",
                           [Attribute(
                               "representation", "Ints")])

make_header("feature", [score, representation], [])


colored = Decorator(
    ["REPRESENTATION", "ORGANIZATIONAL", "ALIAS", "FEATURE", "GEOMETRY"],
    "shape",
    "Colored",
    [Attribute("rgb color", "Vector3")])
geometry_coordinates = Attribute("coordinates list", "Vector3s")

geometry_index = Attribute(
    "type", "Int")

ball = Decorator(["GEOMETRY"], "shape",
                 "Ball",
                 [coordinates, radius])

ellipsoid = Decorator(["GEOMETRY"], "shape",
                      "Ellipsoid",
                      [Attribute("axis lengths", "Vector3"), orientation, coordinates])


cylinder = Decorator(["GEOMETRY"], "shape",
                     "Cylinder",
                     [radius, geometry_coordinates], check_all_attributes=True)

segment = Decorator(["GEOMETRY"], "shape",
                    "Segment",
                    [geometry_coordinates])


make_header("shape", [colored, ball, ellipsoid, cylinder, segment], [])


journal = Decorator(["ORGANIZATIONAL"], "publication",
                    "JournalArticle",
                    [Attribute("title", "String"),
                     Attribute(
                         "journal", "String"),
                     Attribute("pubmed id", "String"),
                     Attribute("year", "Int"),
                     Attribute("authors", "Strings")])

make_header("publication", [journal], [])


residue = Decorator(["REPRESENTATION"], "sequence",
                    "Residue",
                    [Attribute("residue index", "Int"),
                     Attribute("residue type", "String")])

chain = Decorator(["REPRESENTATION"], "sequence",
                  "Chain",
                  [Attribute("chain id", "String")])

domain = Decorator(["REPRESENTATION"], "sequence",
                   "Domain",
                   [RangeAttribute("indexes", "Int", "first residue index",
                                   "last residue index")])

fragment = Decorator(["REPRESENTATION"], "sequence",
                     "Fragment",
                     [Attribute("indexes", "Ints")])

copy = Decorator(["REPRESENTATION"], "sequence",
                 "Copy",
                 [Attribute("copy index", "Int")])

typed = Decorator(["REPRESENTATION"], "sequence",
                  "Typed",
                  [Attribute("type name", "String")])

make_header("sequence", [residue, chain, fragment, domain, typed, copy], [])


salias = Decorator(["ALIAS"], "alias",
                   "Alias",
                   [NodeAttribute("aliased")])

make_header("alias", [salias], [])


external = Decorator(["REPRESENTATION"], "external",
                     "External",
                     [PathAttribute("path")])

make_header("external", [external], [])
