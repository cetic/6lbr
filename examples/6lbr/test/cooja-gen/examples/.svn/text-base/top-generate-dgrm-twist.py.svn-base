import simgen
import generators
import sys

if len(sys.argv) < 2:
	sys.exit("Error, missing .dat DGRM file\nUse:\n\tpython3 top-generate_dgrm_twist.py dgrm_path")
else:
	dgrm_path = sys.argv[1]

simfilefolder = '../../output/'
simfilepath = simfilefolder+'twist.csc'
simgen.create_simfile_from_template(simfilepath)

simfile_lines = simgen.read_simfile(simfilepath)

simgen.create_sky_motetype('node', 'node', 'SkyMote Node', simfile_lines)
simgen.create_sky_motetype('sink1', 'sink1', 'SkyMote Sink 1', simfile_lines)
simgen.create_sky_motetype('sink2', 'sink2', 'SkyMote Sink 2', simfile_lines)

#generate a random grid, because we're using dgrm anyways...
coords = generators.genrandom(0,100,0,100,87) 

nodelist = [] #nodelist is a list of dictionaries containing keys: 'fw', 'coords' and 'nodeid'



for nodeid,coord in enumerate(coords):
	if nodeid == 28:
		nodelist.append({'fw':'sink1', 'coords':coords[0], 'nodeid': 1 + nodeid}) #first = sink1
	elif nodeid == 86:
		nodelist.append({'fw':'sink2', 'coords':coords[0], 'nodeid': 1 + nodeid}) #first = sink1
	else:	
		nodelist.append({'fw':'node', 'coords':coord, 'nodeid': 1 + nodeid}) #all other nodes



simgen.add_motes(nodelist, simfile_lines)
simgen.script_set_timeout(75000, simfile_lines) #stop time in ms


simgen.set_dgrm_model(dgrm_path,simfile_lines)
simgen.save_simfile(simfile_lines, simfilepath)
