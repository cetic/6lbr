"""
Load a preset topology: preset-2dags-20nodes, which is 2 totally disjoint DODAGs with 10 nodes in each.
The first 2 nodes in the generated array are the ones meant to be use as slip-radio, so we assign them as such
"""

outputfolder = 'coojagen/output'
template = 'coojagen/templates/cooja-template-udgm.csc'
radio_model = 'udgm'
tx_range = 45
tx_interference = 45
topology = 'preset'
preset_data_path = 'coojagen/templates/preset_3slipradio_2dags_20nodes_disjoint'
mote_types=[]
interactive_mobility = {'-1':[[55,37],[-37,51]]}

#assignment: all nodes except first 2 which are slip-radios and the last one which is an interactive mote (id: 23)
assignment = {'all':'node', '0':'slipradio', '1':'slipradio', '2':'slipradio', '-1':'node_delay'}

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
			'serial':'' }

mote_types.append(mote_type_slipradio)
mote_types.append(mote_type_6lbrdemo_delay)
mote_types.append(mote_type_6lbrdemo)
