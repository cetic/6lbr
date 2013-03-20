import simgen
import generators

simfilefolder = '../../output/'
simfilepath = simfilefolder+'grid-15m.csc'
simgen.create_simfile_from_template(simfilepath)

simfile_lines = simgen.read_simfile(simfilepath)


simgen.create_sky_motetype('node', 'node', 'SkyMote Node', simfile_lines)
simgen.create_sky_motetype('sink1', 'sink1', 'SkyMote Sink 1', simfile_lines)
simgen.create_sky_motetype('sink2', 'sink2', 'SkyMote Sink 2', simfile_lines)

#gengrid: length, width, unit step (in meters)
coords = generators.gengrid(10,7,10) 

nodelist = [] #nodelist is a list of dictionaries containing keys: 'fw', 'coords' and 'nodeid'

nodelist.append({'fw':'sink1', 'coords':coords[0], 'nodeid':1}) #top left = sink1

for nodeid,coord in enumerate(coords[1:-1]):
	nodelist.append({'fw':'node', 'coords':coord, 'nodeid':2 + nodeid}) #all nodes

nodelist.append({'fw':'sink2', 'coords':coords[-1], 'nodeid':len(coords)}) #bottom right = sink2

simgen.add_motes(nodelist, simfile_lines)
simgen.script_set_timeout(75000, simfile_lines)

simgen.udgm_set_range(15,simfile_lines)
simgen.udgm_set_interference_range(15,simfile_lines)
simgen.save_simfile(simfile_lines, simfilepath)

#same topology with different radio range: 12m
simfilepath=simfilefolder+'grid-12m.csc'
simgen.udgm_set_range(12,simfile_lines)
simgen.udgm_set_interference_range(12,simfile_lines)
simgen.save_simfile(simfile_lines, simfilepath)

#same topology with different radio range: 22m
simfilepath=simfilefolder+'grid-22m.csc'
simgen.udgm_set_range(22,simfile_lines)
simgen.udgm_set_interference_range(22,simfile_lines)
simgen.save_simfile(simfile_lines, simfilepath)

#same topology with different radio range: 25m
simfilepath=simfilefolder+'grid-25m.csc'
simgen.udgm_set_range(25,simfile_lines)
simgen.udgm_set_interference_range(25,simfile_lines)
simgen.save_simfile(simfile_lines, simfilepath)

#same topology with different radio range: 32m
simfilepath=simfilefolder+'grid-32m.csc'
simgen.udgm_set_range(32,simfile_lines)
simgen.udgm_set_interference_range(32,simfile_lines)
simgen.save_simfile(simfile_lines, simfilepath)

#same topology with different radio range: 42m
simfilepath=simfilefolder+'grid-42m.csc'
simgen.udgm_set_range(42,simfile_lines)
simgen.udgm_set_interference_range(42,simfile_lines)
simgen.save_simfile(simfile_lines, simfilepath)

#same topology with different radio range: 52m
simfilepath=simfilefolder+'grid-52m.csc'
simgen.udgm_set_range(52,simfile_lines)
simgen.udgm_set_interference_range(52,simfile_lines)
simgen.save_simfile(simfile_lines, simfilepath)

