radio_model = 'udgm'
tx_range = 45
tx_interference = 45
topology = 'line'
step = 30
mote_count = range(2,6)
assignment = {'all':'node', '0':'slipradio', '-1':'node_delay'}
mote_types = []
outputfolder = 'coojagen/output'
template = 'coojagen/templates/cooja-template-udgm.csc'

mote_type_slipradio = {	'shortname':'slipradio', 
			'fw_folder':'[CONTIKI_DIR]/examples/ipv6/slip-radio/', 
			'maketarget':'slip-radio', 
			'makeargs':'', 
			'description':"6LBR Slip Radio",
			'serial':'socket' }

mote_type_6lbrdemo_delay = {	'shortname':'node_delay', 
				'fw_folder':'[CONTIKI_DIR]/examples/6lbr/test/coojagen/firmwares/6lbr-demo-delay', 
				'maketarget':'6lbr-demo', 
				'makeargs':'WITH_SHELL=1 WITH_WEBSERVER=0 WITH_DELAY_IP=1', 
				'description':"6LBR Demo with delay",
				'serial':'pty' }

mote_type_6lbrdemo = {	'shortname':'node', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lbr/test/coojagen/firmwares/6lbr-demo', 
			'maketarget':'6lbr-demo', 
			'makeargs':'WITH_SHELL=1 WITH_WEBSERVER=0 WITH_DELAY_IP=0', 
			'description':"6LBR Demo",
			'serial':'' }

mote_types.append(mote_type_slipradio)
mote_types.append(mote_type_6lbrdemo_delay)
mote_types.append(mote_type_6lbrdemo)
