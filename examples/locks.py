## \example locks.py
## This example shows how to use the file locking support to regulate
## access to a file. If you pass a command line argument, then the
## script runs forever, to be used with a client program that is
## reading the library.
import RMF
import sys
import time
name=RMF._get_temporary_file_path("locks.rmf")
print "writing to file", name
f= RMF.create_rmf_file(name)

n= f.get_root_node().add_child("child", RMF.REPRESENTATION)
pf= RMF.ParticleFactory(f)
d= pf.get(n)
d.set_mass(1)
d.set_radius(1)

def update():
    # lock the file while updating, it will be deleted at end of
    # function
    time.sleep(1)
    print "locking...",
    lk= RMF.FileLock(f)
    print "done"
    fr=f.get_number_of_frames()
    d= pf.get(n, fr)
    d.set_coordinates([fr, 0, 0])
    print fr
    del lk

while len(sys.argv) >1 or f.get_number_of_frames() < 10:
    update()
