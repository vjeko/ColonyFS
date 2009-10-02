#!/usr/bin/python

import os
import shutil
import random

rootdir = '/tmp/small_files_benchmark'
domain = 'abcdefghijklmnopqrstuvwxyz'

random_string = ''

for i in random.sample(domain,random.randint(1,10)):
  random_string += i

print random_string

try:
  os.mkdir(rootdir)
except:
  'Directory Missing.'


fd = open('/dev/urandom');
data = fd.read(1024000);


for i in range(0, 200):
  destionation  = os.path.join(rootdir, random_string + str(i))
  file_fd = open(destionation, 'w')
  file_fd.write(data)
  file_fd.close()

print 'Done.'

