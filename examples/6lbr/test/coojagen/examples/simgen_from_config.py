import lib_generation
import sys
import os
import generators
import config_simgen

mote_types = []
motelist = []
simfiles = []

def assign_mote_types(assignment, mote_count):
	motenames = [assignment['all'] for i in range(mote_count)]
	for key, value in assignment.iteritems():
		if key != 'all':
			motenames[int(key)] = value
	return motenames

def mote_type_from_shortname(shortname):
	for mote_type in mote_types:
		if mote_type.shortname == shortname:
			return mote_type
	return None

if hasattr(config_simgen, 'outputfolder'):
	outputfolder = config_simgen.outputfolder
else:
	outputfolder = '..' + os.path.sep + 'output'

if hasattr(config_simgen, 'template'):
	template_path = config_simgen.template
else:
	template_path = '..' + os.path.sep + 'templates' . os.path.sep + 'cooja-template-udgp.csc'


lib_generation.mkdir(outputfolder)
lib_generation.cleardir(outputfolder)

for mote_count in config_simgen.mote_count:
	motenames = assign_mote_types(config_simgen.assignment, mote_count)
	simfilepath = os.path.normpath(outputfolder) + os.path.sep + 'simfile-' + str(mote_count) + '-nodes.csc'
	sim = lib_generation.sim(simfilepath, template_path)

	for mote_type in config_simgen.mote_types:
		mote_type_obj = lib_generation.sim_mote_type(	mote_type['shortname'],
								mote_type['fw_folder'],
								mote_type['maketarget'],
								mote_type['makeargs'],
								mote_type['serial'],
								mote_type['description'])
		mote_types.append(mote_type_obj)
		sim.insert_sky_motetype(mote_type_obj)

	sim.udgm_set_range(config_simgen.tx_range)
	sim.udgm_set_interference_range(config_simgen.tx_interference)

	coords = generators.genline(config_simgen.step, mote_count)

	for index,coord in enumerate(coords):
		nodeid = index + 1
		mote = lib_generation.sim_mote(mote_type_from_shortname(motenames[index]), nodeid)
		mote.set_coords(coord[0], coord[1], coord[2])
		motelist.append(mote)

	sim.add_motes(motelist)
	sim.set_timeout(999999999) #stop time in ms

	sim.save_simfile()
	simfiles.append(simfilepath)
	print("****\n%s" % simfilepath)

	motelist=[]

print("Done. Generated %d simfiles" % len(simfiles))

	

