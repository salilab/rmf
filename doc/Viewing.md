# Viewing

# Overview # {#viewers}

[TOC]

There are currently two molecular viewers that support RMFs:

# Chimera # {#chimera}

The [Chimera](https://www.cgl.ucsf.edu/chimera/) research group has
provided support for loading and interacing with RMF files. Chimera
supports
- interactive display of restraints and restraint scores
- geometry


# VMD # {#vmdplugin}

We provide a plugin for
[VMD](http://www.ks.uiuc.edu/Research/vmd/). Instructions for how to
install it can be found on the [VMD plugin page](\ref vmdplugin). The
plugin supports
- multiresolution modules using the IMP::decorator::Alternative decorator
- multiple states stored using the IMP::decorator::State decorator
- fast loading
- displaying restraints as bonds
- static geometry
