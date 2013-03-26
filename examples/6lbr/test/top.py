import coojagen.src.lib_generation as simgen
from os import system


parser = simgen.config_parser()
parser.parse_config('config_simgen.py')

system("./test.py")
