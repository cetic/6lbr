"""
Single Border Router, Line Topology, Static

A line topology with a variable number of nodes. The mote_count variable is a list of mote counts.
There will be 1 topology generated per mote_count.
All motes are of type 'node' except the first which is the 6LBR's slip-radio and the last
which is the mote which we send commands to, 'node_delay'
"""

outputfolder = 'topology/line'
template = 'coojagen/templates/cooja-template-6lx.csc'
radio_model = 'udgm'
tx_range = 45
tx_interference = 45
topology = 'line'
step = 30
mote_count = range(2,13,1)
assignment = {'all':'6lr', '0':'6lbr', '-1':'6lh'} 
multi_br=0
script_file = "[CONFIG_DIR]/../line_script.js"

mote_types = []

mote_type_6lbr = {	'shortname':'6lbr', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lowpan-nd/6lbr/', 
			'maketarget':'ex-6lowpannd-6lbr', 
			'makeargs':'WITH_STATISTICS=1', 
			'description':"6LBR manual routing",
			'serial':'socket' }

mote_type_6lr = {	'shortname':'6lr', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lowpan-nd/6lr/', 
			'maketarget':'ex-6lowpannd-6lr', 
			'makeargs':'WITH_STATISTICS=1', 
			'description':"6LR manual routing",
			'serial':'socket' }

mote_type_6lh = {	'shortname':'6lh', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lowpan-nd/6lh/', 
			'maketarget':'ex-6lowpannd-6lh', 
			'makeargs':'WITH_STATISTICS=1', 
			'description':"6LH manual routing",
			'serial':'socket' }


mote_types.append(mote_type_6lbr)
mote_types.append(mote_type_6lr)
mote_types.append(mote_type_6lh)
