#!/usr/bin/python

import os
import shutil

rootdir = '/tmp/f/small_files_benchmark'

try:
  os.mkdir(rootdir)
except:
  'Directory Missing.'


fd = open('/dev/urandom');
data = fd.read(1024000);


for i in range(0, 200):
  destionation  = os.path.join(rootdir, str(i))
  file_fd = open(destionation, 'w')
  file_fd.write(data)
  file_fd.close()

print 'Done.'

