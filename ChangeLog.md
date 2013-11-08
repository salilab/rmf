# Change Log #
- Appropriate decorators now return RMF::Vector3, RMF::Vector4 or
	RMF::Vector3s instead of RMF::Floats.
- There are now static and non-static versions of the decorators.
- You now have to explicitly add a frame or set the current frame before
	manipulating frame data.
- RMF::NodeConstHandle::get_value() subsumes get_value_always() which was
	removed. If there is no value, then either a RMF::Nullable for
	which RMF::Nullable::get_is_null() is true is returned (C++) or
	`None` is returned (python).
- RMF::FileHandle::get_node_from_id() was renamed to RMF::FileHandle::get_node()
- Various methods that took ints to identify frames now expect RMF::FrameID.
- A VMD plugin was added
