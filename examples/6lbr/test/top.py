import coojagen.src.lib_generation as simgen
from os import system
import sys

parser = simgen.ConfigParser()
if not parser.parse_config('config_simgen.py'):
	sys.exit("topology generation error")
simfiles = parser.get_simfiles()

for simfile in simfiles:
  running_simfile = open('.NEXT_TOPOLOGY', 'w')
  running_simfile.write(simfile)
  running_simfile.close()
  system("./test.py")

system("rm .NEXT_TOPOLOGY")
