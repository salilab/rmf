# RMF #

The library provides support for the RMF file format for
storing hierarchical molecular data (such as atomic or coarse grained
representations of proteins), along with markup, including geometry
and score data.

Note the the repository has a subrepository so you must use
`git clone --recursive` when cloning (or call `git submodule init`
after cloning if you have an older git and do
`git pull --recurse-submodules` when updating (or
`git submodule update` after updating if your git is older).

The library uses the [HDF5](http://www.hdf5.org) library or
[Avro](http://avro.apache.org) library to manage the data on
disk. Other backends (eg mmCIF) could be used, if desired.

See
- [RMF file format](\ref format) for more information about the files,
- [RMF Library](\ref library) for more information about using the library.
- [Mapping data into RMF files](\ref mapping) for more information about how to structure data in RMF files.

Also see the [rmf examples](http://www.github.com/salilab/rmf_examples) repository
for examples of interesting or problematic RMF files.
