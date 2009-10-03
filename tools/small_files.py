#!/usr/bin/python

import os
import shutil
import random

rootdir = '/tmp/small_files_benchmark'
domain = 'abcdefghijklmnopqrstuvwxyz'
num_files = 200

random_string = ''

for i in random.sample(domain,random.randint(1,10)):
  random_string += i

print random_string

rootdir += "-" + random_string;

try:
  os.mkdir(rootdir)
except:
  'Directory Missing.'


fd = open('/dev/urandom');
data = fd.read(1024000);


for i in range(0, num_files):
  destionation  = os.path.join(rootdir, str(i))
#destionation  = os.path.join(rootdir, str(i))
  file_fd = open(destionation, 'w')
  file_fd.write(data)
  file_fd.close()

print 'Done.'

