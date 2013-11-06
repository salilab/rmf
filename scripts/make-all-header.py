#!/usr/bin/env python
import sys
import glob
import os
for h in sys.argv[2:]:
    pat = os.path.join(h, "*.h")
    allh = sorted(glob.glob(pat))
    for g in allh:
        name = os.path.split(g)[1]
        print "#include <%s/" % sys.argv[1] + name + ">"
