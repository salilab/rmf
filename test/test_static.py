import RMF
for suffix in RMF.suffixes:
    path=RMF._get_temporary_file_path("alias."+suffix)
    f = RMF.create_rmf_file(path)
    f0 = f.add_frame("frame", RMF.FRAME)
    cat = f.get_category("cat")
    k = f.get_int_key(cat, "key")
    f.set_current_frame(RMF.ALL_FRAMES)
    n = f.get_root_node()
    n.set_value(k, 1)
    f.set_current_frame(f0)
    assert(n.get_value(k) == 1)
