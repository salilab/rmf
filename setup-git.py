#! /usr/bin/env python
import os
import os.path
os.system("git submodule update --init --recursive")
os.system("git submodule update --recursive")
os.system(os.path.join("tools", "developer_tools", "git", "setup-git.py"))
