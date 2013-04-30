#!/usr/bin/python2.7
import sys
import coojagen.src.lib_generation as simgen

if len(sys.argv) < 2:
    sys.exit("Error: missing input params")

config_simgen = sys.argv[1]
parser = simgen.ConfigParser()
res = parser.parse_config_file(config_simgen)

tempfile = open('./coojagen/output/LASTFILE', 'w')
tempfile.write(parser.simfiles[0])

tempfile.close()
