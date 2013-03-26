import coojagen.src.lib_generation as simgen
from os import system

parser = simgen.ConfigParser()
parser.parse_config('config_simgen.py')
simfiles = parser.get_simfiles()

for simfile in simfiles:
  running_simfile = open('.NEXT_TOPOLOGY', 'w')
  running_simfile.write(simfile)
  running_simfile.close()
  system("./test.py")

system("rm .NEXT_TOPOLOGY")
