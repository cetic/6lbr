#!/usr/bin/python2.7

import coojagen.src.lib_generation as simgen
import os
from os import system
import sys
import time
from config import report_path
from config import simgen_config_path
import shutil

parser = simgen.ConfigParser()

if not parser.parse_config(simgen_config_path):
	sys.exit("topology generation error")
simfiles = parser.get_simfiles()

for simfile in simfiles:
  #Create the report work directories
  if not os.path.exists(report_path):
    os.makedirs(report_path)
  #Open and run the next COOJA topology
  running_simfile = open('.NEXT_TOPOLOGY', 'w')
  simname = os.path.basename(simfile).replace('.csc','')
  running_simfile.write(simfile)
  running_simfile.close()
  #Run the test suite with the current topology
  system("python2.7 ./test.py")
  #Move the current coojasim working directory to its final location
  srcdir = os.path.dirname(os.path.dirname(report_path))
  shutil.copyfile(os.path.join('coojagen/output',simname+'.csc'),os.path.join(srcdir,simname+'.csc'))
  shutil.copyfile(os.path.join('coojagen/output',simname+'.motes'),os.path.join(srcdir,simname+'.motes'))
  destdir = os.path.join(os.path.dirname(srcdir),simname)
  if os.path.exists(destdir):
    if os.path.isdir(destdir):
      shutil.rmtree(destdir)
    else:
      os.unlink(destdir)
  os.rename(srcdir,destdir)

#Move the current run working directory to its final location
srcdir = os.path.dirname(os.path.dirname(os.path.dirname(report_path)))
destdir = os.path.join(os.path.dirname(srcdir),'run-%s' % time.strftime("%Y%m%d%H%M%S"))
if os.path.exists(destdir):
  if os.path.isdir(destdir):
    shutil.rmtree(destdir)
  else:
    os.unlink(destdir)
os.rename(srcdir,destdir)


system("rm .NEXT_TOPOLOGY")
