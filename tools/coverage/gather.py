#!/usr/bin/env python

import subprocess
import os
import sys

def gather_python():
    subprocess.check_call(['coverage', 'combine'], cwd='coverage')

def run_gcov():
    top_dir = os.path.abspath(os.path.join(sys.argv[0], '..', '..'))
    for dirpath, dirnames, filenames in os.walk('.'):
        for f in filenames:
            if f.endswith('.gcno'):
                subprocess.check_call(['gcov', '-s', top_dir, '-rlp',
                                       os.path.join(dirpath, f)])

def main():
    gather_python()
    run_gcov()

if __name__ == '__main__':
    main()
