# Change Log #
- RMF::NodeConstHandle::get_value() subsumes get_value_always() which was
	removed. If there is no value, then either a RMF::Nullable for
	which RMF::Nullable::get_is_null() is true is returned (C++) or
	`None` is returned (python).
- RMF::FileHandle::get_node_from_id() was renamed to RMF::FileHandle::get_node()
- Various methods that took ints to identify frames now expect RMF::FrameID.
- A VMD plugin was added
