#!/usr/bin/python

import os
import os.path
import shutil

rootdir = '/tmp/f/dir_benchmark'

try:
  shutil.rmtree(rootdir)
except:
  'Directory Missing.'



for i in range(0, 10):
  for j in range(0, 10):
    leafdir = os.path.join(rootdir, str(i), str(j))
    os.makedirs(leafdir)

print 'Done.'


