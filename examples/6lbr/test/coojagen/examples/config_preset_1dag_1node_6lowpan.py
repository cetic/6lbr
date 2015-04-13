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
preset_data_path = 'coojagen/templates/preset_1dag_1node'
mote_types=[]
multi_br=0

#assignment: all nodes except first which is slip-radio and the last one which is an interactive mote (id: 23)
assignment = {'all':'node', '0':'slipradio'}

mote_type_slipradio = {	'shortname':'slipradio', 
			'fw_folder':'[CONTIKI_DIR]/examples/ipv6/slip-radio/', 
			'maketarget':'slip-radio', 
			'makeargs':'', 
			'description':"6LBR Slip Radio",
			'serial':'socket' }

mote_type_6lbrdemo_6lowpan_delay = {	'shortname':'node_delay', 
				'fw_folder':'[CONTIKI_DIR]/examples/6lbr/test/coojagen/firmwares/6lbr-demo-6lh-delay', 
				'maketarget':'6lbr-demo-6lh-delay', 
				'makeargs':'', 
				'description':"6LBR Demo with delay",
				'serial':'pty' }

mote_type_6lbrdemo_6lowpan = {	'shortname':'node', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lbr/test/coojagen/firmwares/6lbr-demo-6lh', 
			'maketarget':'6lbr-demo-6lh', 
			'makeargs':'', 
			'description':"6LBR Demo",
			'serial':'pty' }

mote_types.append(mote_type_slipradio)
#mote_types.append(mote_type_6lbrdemo_6lowpan_delay)
mote_types.append(mote_type_6lbrdemo_6lowpan)
