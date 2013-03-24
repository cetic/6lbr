import lib_generation
import sys
import os
import generators

# USER CONFIG. MIGRATE ELSEWHERE
SLIP_RADIO_ID = [1,]
NODECOUNT = 5
STEP = 30
TXRANGE = 45
# END OF USER CONFIG

motelist = []


if len(sys.argv) < 2:
	sys.exit("Error, missing input files\nUse:\n\tpython3 %s template_path [output_folder]" % sys.argv[0])

if len(sys.argv) < 3: 
	outputfolder = '..' + os.path.sep + 'output' + os.path.sep;
else:
	outputfolder = sys.argv[2]

template_path = sys.argv[1]

lib_generation.mkdir(outputfolder)

simfilepath = os.path.normpath(outputfolder) + os.path.sep + 'simfile.csc'

sim = lib_generation.sim(simfilepath, template_path)
print(sim)

type_slipradio = lib_generation.sim_mote_type('slipradio', '[CONTIKI_DIR]/examples/ipv6/slip-radio/', 'slip-radio', '', "6LBR Slip Radio")
type_node = lib_generation.sim_mote_type('client', '[CONTIKI_DIR]/examples/6lbr-demo/', '6lbr-demo', 'WITH_SHELL=1 WITH_WEBSERVER=0 WITH_DELAY_IP=1', "6LBR Slip Radio")

sim.insert_sky_motetype(type_slipradio)
sim.insert_sky_motetype(type_node)
sim.udgm_set_range(TXRANGE)
sim.udgm_set_interference_range(TXRANGE)

coords = generators.genline(STEP, NODECOUNT)

for index,coord in enumerate(coords):
        index = index + 1
	nodeid = index
	if nodeid in SLIP_RADIO_ID:
		mote = lib_generation.sim_mote(type_slipradio, nodeid)
	else:
		mote = lib_generation.sim_mote(type_node, nodeid)
	mote.set_coords(coord[0], coord[1], coord[2])
	motelist.append(mote)

sim.add_motes(motelist)
sim.set_timeout(999999999) #stop time in ms

sim.save_simfile()


