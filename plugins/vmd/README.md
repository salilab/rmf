# VMD RMF Plugin #

# Building and installing#
To compile this plugin, you need to pass `VMDPLUGIN_INCLUDE_DIR=/path/to/vmd/plugin/include` to `cmake`. The directory is the one that contains `molfile_plugin.h`. Once you build RMF after that, there will be a file called `rmfplugin.so` in the build directory. Creating a link to that in the `vmd/plugins/YOUR_ARCHITECTURE/molfile/` directory, should make `vmd` load the plugin next time it launches. To open an `rmf` file you have to tell `vmd` that the file type is `rmf`, eg `vmd -rmf path/to/my.rmf`.

# Caveats #
- VMD's plugin model supports static geometry and atoms (or pseudo atoms) that change with each frame. So no moving cylinders. Balls are mapped to atoms.
- VMD doesn't seem to support 64 bit builds on MacOS (at least no usable pre-built ones are provided). By default, RMF is build 64 bit, and so the plugin will not work. You can probably build a 32 bit version of the plugin, but we have not done so.
- the plugin is linked against `libRMF` and `Avro` in the build directory
- it is not been tried on windows
