# Change Log #
- Decorators and factories have been moved into the RMF::decorator namespace in C++.
- Two new formats have been added: `.rmf3` and its compressed version
	`.rmfz`. You should use those in preference to the prior formats
	for new files.
- The RMF repository now has a git submodule. This means you need to do
	`git submodule init` after cloning (or `git clone
	git@github.com:salilab/RMF.git --recursive`) and then update with
	`git pull --recursive origin develop`.
- You can now only either create a new RMF or open an existing one read
	only. Most backends only supported those two anyway.
- Managing of data buffers has been revamped and wrapped in the
	RMF::BufferHandle class which tracks a shared buffer.
- Indexes and NodeID attributes are now just ints.
- You now get keys by passing the appropriate traits object instead of
	having the type in the function name: eg `f.get_key(cat, "name", RMF.IntTraits())`
- Appropriate decorators now return RMF::Vector3, RMF::Vector4 or
	RMF::Vector3s instead of RMF::Floats or RMF::FloatsList.
- There are now static and non-static versions of the decorators.
- You now have to explicitly add a frame or set the current frame before
	manipulating frame data.
- RMF::NodeConstHandle::get_value() subsumes get_value_always() which was
	removed. If there is no value, then either a RMF::Nullable for
	which RMF::Nullable::get_is_null() is true is returned (C++) or
	`None` is returned (python).
- RMF::FileHandle::get_node_from_id() was renamed to
	RMF::FileHandle::get_node()
- Various methods that took ints to identify frames now expect RMF::FrameID.
- A VMD plugin was added
