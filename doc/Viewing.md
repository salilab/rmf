Viewing {#viewing}
=======

# Overview # {#viewers}

[TOC]

There are currently two molecular viewers that support RMFs:

# Chimera # {#chimera}

The [Chimera](https://www.cgl.ucsf.edu/chimera/) research group has
provided support for loading and interacing with RMF files. Chimera
supports
- interactive display of restraints and restraint scores
- geometry


# VMD # {#vmd}

We provide a plugin for
[VMD](http://www.ks.uiuc.edu/Research/vmd/). Instructions for how to
install it can be found on the [VMD plugin page](\ref vmdplugin). The
plugin supports
- multiresolution modules using the RMF::decorator::Alternatives decorator
- multiple states stored using the RMF::decorator::State decorator
- fast loading
- displaying restraints as bonds
- static geometry

# Pymol # {#pymol}

We provide a plugin for Pymol. Instruction can be found at the
[Pymol plugin page](\ref pymolplugin). The plugin supports creating
multiple molecules from one RMF based on chains or RMF::decorator::Molecule
labels as well as dynamics geometry.
