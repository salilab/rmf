#!/usr/bin/env python
import sys
import glob
import os
import os.path
import difflib


def _rewrite(filename, input, verbose=True):
    contents = "\n".join(input) + "\n"
    try:
        old = open(filename, "r").read()
        if old == contents:
            return
        elif verbose:
            print "    Different", filename
            for l in difflib.unified_diff(old.split("\n"), contents.split("\n")):
                stl = str(l)
                if (stl[0] == '-' or stl[0] == '+') and stl[1] != '-' and stl[1] != '+':
                    print "    " + stl
    except:
        pass
        # print "Missing", filename
    open(filename, "w").write(contents)


def _get_files(ds, suffix):
    patterns = [os.path.join(d, "*" + suffix) for d in ds]
    files = []
    for p in patterns:
        files += [f[len(ds[0]) + 1:].replace("\\", "/") for f in glob.glob(p)]
    files.sort()
    return files


def make_all_rmf_header():
    pat = os.path.join("include", "RMF", "*.h")
    allh = glob.glob(pat)
    allh.sort()

    out = []
    for g in allh:
        name = os.path.split(g)[1]
        if name == "HDF5.h":
            continue
        out.append("#include <RMF/" + name + ">")
    # a bit icky
    for d in ["alias_decorators.h",
              "publication_decorators.h",
              "external_decorators.h",
              "sequence_decorators.h",
              "feature_decorators.h",
              "shape_decorators.h",
              "physics_decorators.h"]:
        out.append("#include <RMF/%s>" % d)
    _rewrite(os.path.join("include", "RMF.h"), out)


def make_all_hdf5_header():
    pat = os.path.join("include", "RMF", "HDF5", "*.h")
    allh = glob.glob(pat)
    allh.sort()

    out = []
    for g in allh:
        name = os.path.split(g)[1]
        out.append("#include <RMF/HDF5/" + name + ">")
    _rewrite(os.path.join("include", "RMF", "HDF5.h"), out)


def make_files(d):
    output = os.path.join(d[0], "Files.cmake")
    cppfiles = _get_files(d, ".cpp")
    pyfiles = _get_files(d, ".py")
    jsonfiles = _get_files(d, ".json")
    out = ["set(pyfiles \"%s\")" % ";".join(pyfiles),
           "set(cppfiles \"%s\")" % ";".join(cppfiles)]
    if len(jsonfiles) > 0:
        out.append("set(jsonfiles \"%s\")" % ";".join(jsonfiles))
    _rewrite(output, out)

make_all_rmf_header()
make_all_hdf5_header()
src = ["src",
       os.path.join("src", "internal"),
       os.path.join("src", "backend"),
       os.path.join("src", "backend", "hdf5"),
       os.path.join("src", "backend", "avro")]
for d in [src, ["examples"], ["bin"], ["test"], ["benchmark"]]:
    make_files(d)
