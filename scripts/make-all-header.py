#!/usr/bin/env python
import sys
import glob
import os

output = []
for h in sys.argv[3:]:
    pat = os.path.join(h, "*.h")
    allh = sorted(glob.glob(pat))
    for g in allh:
        name = os.path.split(g)[1]
        output.append("#include <%s/" % sys.argv[2] + name + ">")

if os.path.exists(sys.argv[1]):
    old = open(sys.argv[1], "r").read()
    if old == "\n".join(output):
        sys.exit(0)
open(sys.argv[1], "w").write("\n".join(output))
