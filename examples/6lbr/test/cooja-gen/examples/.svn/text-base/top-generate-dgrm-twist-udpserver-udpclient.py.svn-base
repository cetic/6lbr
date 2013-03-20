import lib_generation
import sys
import os
import generators

SINK_NODEID = 272

if len(sys.argv) < 4:
	sys.exit("Error, missing input files\nUse:\n\tpython3 top-generate_dgrm_twist.py dgrm_path nodeids_path template_path [output_path] ")

if len(sys.argv) < 5: 
	simfilepath = '../output/twist2.csc'
else:
	simfilepath = sys.argv[4]

dgrm_path = sys.argv[1]
nodeid_path = sys.argv[2]
template_path = sys.argv[3]

if not os.path.isdir(os.path.dirname(simfilepath)):
	sys.exit("Error, %s not a valid directory" % os.path.dirname(simfilepath))

nodeids = []
if nodeid_path != '':
	nodeids = lib_generation.extract_node_id_list(nodeid_path)

nodecount = len(nodeids)

sim = lib_generation.new_sim(simfilepath, template_path)

sim.create_sky_motetype('udp-server','udp-server', 'SkyMote UDP Server')
sim.create_sky_motetype('udp-client','udp-client', 'SkyMote UDP Client')

#generate a random grid, because we're using dgrm anyways...
coords = generators.genrandom(0,100,0,100,nodecount) 

nodelist = [] #nodelist is a list of dictionaries containing keys: 'fw', 'coords' and 'nodeid'

for index,coord in enumerate(coords):
	nodeid = int(nodeids[index])
	if nodeid == SINK_NODEID:
		nodelist.append({'fw':'udp-server', 'coords':coord, 'nodeid': nodeid}) #sink = 272
	else:
		nodelist.append({'fw':'udp-client', 'coords':coord, 'nodeid': nodeid}) #all other nodes

sim.add_motes(nodelist)
sim.set_timeout(3600000) #stop time in ms


sim.set_dgrm_model(dgrm_path)
sim.save_simfile()
