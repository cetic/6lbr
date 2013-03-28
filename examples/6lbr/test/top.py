import coojagen.src.lib_generation as simgen
from os import system
import sys
import time
from config import report_path
from shutil import rmtree

parser = simgen.ConfigParser()
if not parser.parse_config('config_simgen.py'):
	sys.exit("topology generation error")
simfiles = parser.get_simfiles()

for simfile in simfiles:
  #Create the report work directories
  if not os.path.exists(config.report_path):
    os.makedirs(config.report_path)
  #Open and run the next COOJA topology
  running_simfile = open('.NEXT_TOPOLOGY', 'w')
  simname = running_simfile.name
  running_simfile.write(simfile)
  running_simfile.close()
  #Run the test suite with the current topology
  system("./test.py")
  #Move the current coojasim working directory to its final location
  destdir = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(config.report_path))),'coojasim-%s' % simname)
  if os.path.exists(destdir):
    if os.path.isdir(desdir):
      shutil.rmtree(destdir)
    else:
      os.unlink(destdir)
  os.rename(config.report_path,desdir)

#Move the current run working directory to its final location
destdir = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(config.report_path)))),'run-%s' % time.strftime("%Y%m%d%H%M%S"))
if os.path.exists(destdir):
  if os.path.isdir(desdir):
    shutil.rmtree(destdir)
  else:
    os.unlink(destdir)
os.rename(config.report_path,desdir)

system("rm .NEXT_TOPOLOGY")
