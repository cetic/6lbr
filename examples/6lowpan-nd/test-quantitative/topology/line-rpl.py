
outputfolder = 'topology/line-rpl'
template = 'coojagen/templates/cooja-template-6lx.csc'
radio_model = 'udgm'
tx_range = 300
tx_interference = 400
topology = 'line'
step = 30
mote_count = range(2,7,1)
assignment = {'all':'6lr-rpl', '0':'6lbr-rpl', '-1':'6lh'} 
multi_br=0
script_file = "[CONFIG_DIR]/../line-rpl_script.js"

mote_types = []

mote_type_6lbr = {	'shortname':'6lbr-rpl', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lowpan-nd/6lbr-rpl/', 
			'maketarget':'ex-6lowpannd-6lbr', 
			'makeargs':'WITH_STATISTICS=1', 
			'description':"6LBR RPL routing",
			'serial':'socket',
			'platform': 'wismote' }

mote_type_6lr = {	'shortname':'6lr-rpl', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lowpan-nd/6lr-rpl/', 
			'maketarget':'ex-6lowpannd-6lr', 
			'makeargs':'WITH_STATISTICS=1', 
			'description':"6LR RPL routing",
			'serial':'socket',
			'platform': 'wismote' }

mote_type_6lh = {	'shortname':'6lh', 
			'fw_folder':'[CONTIKI_DIR]/examples/6lowpan-nd/6lh/', 
			'maketarget':'ex-6lowpannd-6lh', 
			'makeargs':'WITH_STATISTICS=1', 
			'description':"6LH manual routing",
			'serial':'socket',
			'platform': 'wismote' }


mote_types.append(mote_type_6lbr)
mote_types.append(mote_type_6lr)
mote_types.append(mote_type_6lh)
