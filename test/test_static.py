import RMF
for suffix in RMF.suffixes:
    path=RMF._get_temporary_file_path("alias."+suffix)
    f = RMF.create_rmf_file(path)
    fs = f.get_current_frame()
    f0 = fs.add_child("frame", RMF.FRAME)
    cat = f.get_category("cat")
    k = f.get_int_key(cat, "key")
    fs.set_as_current_frame()
    n = f.get_root_node()
    n.set_value(k, 1)
    f0.set_as_current_frame()
    assert(n.get_value(k) == 1)
