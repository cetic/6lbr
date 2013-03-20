import simgen
import generators

simfilefolder = '../../output/'
simfilepath = simfilefolder+'twistreplay.csc'
simgen.create_twistreplay_from_template(simfilepath)

simfile_lines = simgen.read_simfile(simfilepath)


#gengrid: length, width, unit step (in meters)
coords = generators.gengrid(10,10,10) 

nodelist = [] #nodelist is a list of dictionaries containing keys: 'fw', 'coords' and 'nodeid'

for nodeid,coord in enumerate(coords[0:96]):
	nodelist.append({'fw':'apptype1', 'coords':coord, 'nodeid':1 + nodeid}) #all nodes

simgen.add_motes(nodelist, simfile_lines)

simgen.save_simfile(simfile_lines, simfilepath)


