import RMF
import cPickle

b = RMF.BufferHandle()
f = RMF.create_rmf_buffer(b)

f.get_root_node().add_child("hi", RMF.ORGANIZATIONAL)

del f

picklestring = cPickle.dumps(b)
bb = cPickle.loads(picklestring)

f = RMF.open_rmf_buffer_read_only(bb)
print f.get_root_node().get_children()[0]