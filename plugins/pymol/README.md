# VMD Plugin #

# Using the plugin # {#pymolplugin}
[TOC]

Pymol does not seem to support a file type plugin architecture. So
opening RMFs is a bit awkward. The `lib/pymol_rmf_plugin.py` script
adds a new command to Pymol, `rmf` which loads an RMF file. Using that
one can open a file from the command line with a command like `pymol
lib/pymol_rmf_plugin.py -d "rmf ../rmf/foo.rmf"`. Not very pretty.