"""
Single Border Router, Line Topology, Static

A line topology with a variable number of nodes. The mote_count variable is a list of mote counts.
There will be 1 topology generated per mote_count.
All motes are of type 'node' except the first which is the 6LBR's slip-radio and the last
which is the mote which we send commands to, 'node_delay'
"""

outputfolder = 'coojagen/output'
template = 'coojagen/templates/cooja-template-udgm.csc'
radio_model = 'udgm'
tx_range = 45
tx_interference = 45
topology = 'line'
step = 30
mote_count = range(2,11,2) # = 5 topologies, 2,4,6,8,10
assignment = {'all':'node', '0':'slipradio', '-1':'node_delay'} 
multi_br=0

mote_types = []

mote_type_slipradio = {	'target': 'cooja', 'shortname':'slipradio',
			'fw_folder':'[CONTIKI_DIR]/examples/ipv6/slip-radio/', 
			'maketarget':'slip-radio', 
			'makeargs':'', 
			'description':"6LBR Slip Radio",
			'serial':'socket' }

mote_type_6lbrdemo_delay = { 'target': 'cooja', 'shortname':'node_delay', 
				'fw_folder':'[CONTIKI_DIR]/examples/6lbr/test/coojagen/firmwares/6lbr-demo-delay', 
				'maketarget':'6lbr-demo', 
				'makeargs':'', 
				'description':"6LBR Demo with delay",
				'serial':'pty' }

mote_type_6lbrdemo = { 'target': 'cooja', 'shortname':'node', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lbr/test/coojagen/firmwares/6lbr-demo', 
			'maketarget':'6lbr-demo', 
			'makeargs':'', 
			'description':"6LBR Demo",
			'serial':'pty' }

mote_types.append(mote_type_slipradio)
mote_types.append(mote_type_6lbrdemo_delay)
mote_types.append(mote_type_6lbrdemo)
