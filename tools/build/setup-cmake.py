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

src = ["src",
       os.path.join("src", "internal"),
       os.path.join("src", "decorator"),
       os.path.join("src", "backend"),
       os.path.join("src", "backend", "hdf5"),
       os.path.join("src", "backend", "avro"),
       os.path.join("src", "backend", "avro2")]
for d in [src, ["examples"], ["bin"], ["test"], ["benchmark"]]:
    make_files(d)
