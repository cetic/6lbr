radio_model = 'udgm'
tx_range = 45
tx_interference = 45
#topology = 'grid_ratio'
topology = 'preset'
preset_data_path = 'coojagen/templates/preset-2dags-20nodes'
#step = 30
#xcount = 2
#ycount = 5
#ratio = "1:3"
#mote_count = range(10,31,10)
#mote_count = [10,]
assignment = {'all':'node', '0':'slipradio', '1':'slipradio', '-1':'node_delay'}
mote_types = []
outputfolder = 'coojagen/output'
template = 'coojagen/templates/cooja-template-udgm.csc'
mobility = '[APPS_DIR]/mobility/positions.dat'

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
