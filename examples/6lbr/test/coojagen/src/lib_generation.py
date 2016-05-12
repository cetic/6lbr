#Python COOJA simulation generator
import os
import sys
import shutil
import re
import generators
import imp
import time

class SimMoteType:
	def __init__(self, target, shortname, fw_folder, maketarget, makeargs, serial, description):
		self.target = target
		self.shortname = shortname
		self.fw_folder = os.path.normpath(fw_folder)
		self.maketarget = maketarget
		self.makeargs = makeargs
		self.serial = serial
		self.description = description

	def text_from_template(self):
		text_sky ="""    <motetype>\r\n
	  org.contikios.cooja.mspmote.SkyMoteType
	  <identifier>SHORTNAME</identifier>
	  <description>DESCRIPTION</description>
	  <source EXPORT="discard">FIRMWAREPATH</source>
	  <commands EXPORT="discard">make FIRMWARE.sky TARGET=sky MAKEARGS</commands>
	  <firmware EXPORT="copy">FIRMWAREBIN</firmware>
	  <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
	  <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
	  <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
	  <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
	  <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyButton</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyFlash</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyByteRadio</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.MspSerial</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyLED</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
	  <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>\r\n"""
		text_coojamote = """    <motetype>\r\n
	  org.contikios.cooja.contikimote.ContikiMoteType
	  <identifier>SHORTNAME</identifier>
	  <description>DESCRIPTION</description>
	  <source EXPORT="discard">FIRMWAREPATH</source>
	  <commands EXPORT="discard">make TARGET=cooja clean
	  make FIRMWARE.cooja TARGET=cooja MAKEARGS</commands>
	  <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
	  <moteinterface>org.contikios.cooja.interfaces.Battery</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiVib</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiMoteID</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiRS232</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiBeeper</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiIPAddress</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiRadio</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiButton</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiPIR</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiClock</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiLED</moteinterface>
	  <moteinterface>org.contikios.cooja.contikimote.interfaces.ContikiCFS</moteinterface>
	  <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
	  <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
	  <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
	  <symbols>false</symbols>
    </motetype>\r\n"""
		if self.target == 'cooja':
			text = text_coojamote
		else:
			text = text_sky
		text = text.replace('FIRMWAREPATH', self.fw_folder + os.path.sep + self.maketarget + '.c')
		text = text.replace('FIRMWAREBIN', self.fw_folder + os.path.sep + self.maketarget + '.sky')
		text = text.replace('SHORTNAME', self.shortname)
		text = text.replace('DESCRIPTION', self.description)
		text = text.replace('FIRMWARE', self.maketarget)
		text = text.replace('MAKEARGS', self.makeargs)
		return text

class SimMote:
	socket_base_port = 60000
	def __init__(self, mote_type, nodeid):
		self.mote_type = mote_type
		self.nodeid = nodeid
		self.mobility_data = None
	def set_coords(self, xpos, ypos, zpos=0):
		self.xpos = xpos
		self.ypos = ypos
		self.zpos = zpos

	def set_mobility_data(self, data):
		self.mobility_data = data

	def text_from_template(self):


		text = """    <mote>
      <breakpoints />
      <interface_config>
	org.contikios.cooja.mspmote.interfaces.MspMoteID
	<id>NODE_ID</id>
      </interface_config>
      <interface_config>
	org.contikios.cooja.interfaces.Position
	<x>XPOS</x>
	<y>YPOS</y>
	<z>ZPOS</z>
      </interface_config>
      <motetype_identifier>MOTETYPE_ID</motetype_identifier>
    </mote>\r\n"""

		if self.mote_type.target == 'cooja':
			text = text.replace('org.contikios.cooja.mspmote.interfaces.MspMoteID','org.contikios.cooja.contikimote.interfaces.ContikiMoteID')			
		text = text.replace('XPOS','%03.13f' % self.xpos)
		text = text.replace('YPOS','%03.13f' % self.ypos)
		text = text.replace('ZPOS','%03.13f' % self.zpos)
		text = text.replace('NODE_ID', str(self.nodeid))
		text = text.replace('MOTETYPE_ID', self.mote_type.shortname)
		return text

	def serial_text(self):
		if self.mote_type.serial == 'pty':
			text = """  <plugin>
    de.fau.cooja.plugins.Serial2Pty
    <mote_arg>MOTEARG</mote_arg>
    <width>250</width>
    <z>0</z>
    <height>100</height>
    <location_x>161</location_x>
    <location_y>532</location_y>
  </plugin>\r\n"""
		elif self.mote_type.serial == 'socket':
			text = """  <plugin>
    org.contikios.cooja.serialsocket.SerialSocketServer
    <mote_arg>MOTEARG</mote_arg>
    <width>459</width>
    <z>4</z>
    <height>119</height>
    <location_x>5</location_x>
    <location_y>525</location_y>
    <plugin_config>
	  <port>%d</port>
	  <bound>true</bound>
	</plugin_config>
  </plugin>\r\n""" % (self.socket_base_port + self.nodeid)
		else:
			return ''

		text = text.replace('MOTEARG','%d' % (self.nodeid-1))
		return text

class Sim():
	def __init__(self, templatepath):
		self.templatepath = templatepath
		self.simfile_lines = read_simfile(self.templatepath)

	def insert_motetype(self, mote_type):

		motetype_text = mote_type.text_from_template()

		motetype_indexes = all_indices("    <motetype>\r\n",self.simfile_lines)
		motetype_close_indexes = all_indices("    </motetype>\r\n",self.simfile_lines)

		if len(motetype_indexes) == 0 or len(motetype_indexes) != len(motetype_close_indexes):
			print "No motetype placeholder or file truncated"
			sys.exit(1)
		elif len(motetype_indexes) == 1:
			#in case of 1 motetype, check if it's the template version or a real mote
			if self.simfile_lines[motetype_indexes[0]+2] == "      <identifier>templatesky1</identifier>\r\n":
				#template version, we first remove the template motetype lines
				count = motetype_close_indexes[0] - motetype_indexes[0] + 1
				remove_n_at(motetype_indexes[0], count, self.simfile_lines)
				#insert the mote type
				self.simfile_lines = insert_list_at(motetype_text.splitlines(1), self.simfile_lines, motetype_indexes[0]) #1= we keep the endlines
			else:
				self.simfile_lines = insert_list_at(motetype_text.splitlines(1), self.simfile_lines, motetype_close_indexes[0]+1, )

		else:
			#if there are more than one, we know they are not template motetypes. We append a new one
			self.simfile_lines = insert_list_at(motetype_text.splitlines(1), self.simfile_lines, motetype_close_indexes[-1]+1)

	def add_mote(self, mote):

		mote_text = mote.text_from_template()
		
		mote_indexes = all_indices("    <mote>\r\n",self.simfile_lines)
		mote_close_indexes = all_indices("    </mote>\r\n",self.simfile_lines)
	
		if len(mote_indexes) == 1:
			#only 1 mote, check if it's the template
			if self.simfile_lines[mote_indexes[0]+4] == "        <x>XPOS</x>\r\n":
				#template version, we first remove the template motetype lines
				count = mote_close_indexes[0] - mote_indexes[0] + 1
				remove_n_at(mote_indexes[0], count, self.simfile_lines)
				#insert the mote type
				self.simfile_lines = insert_list_at(mote_text.splitlines(1), self.simfile_lines, mote_indexes[0])
			else:
				self.simfile_lines = insert_list_at(mote_text.splitlines(1), self.simfile_lines, mote_close_indexes[0]+1, )

		else:
			#if there are more than one, we know they are not template motetypes. We append a new one
			self.simfile_lines = insert_list_at(mote_text.splitlines(1), self.simfile_lines, mote_close_indexes[-1]+1)
		
		
		if mote.mote_type.serial != '':	
			plugin_indexes = all_indices("  </plugin>\r\n", self.simfile_lines)
			serial_text = mote.serial_text()
			self.simfile_lines = insert_list_at(serial_text.splitlines(1), self.simfile_lines, plugin_indexes[-1]+1)

	def add_motes(self, mote_list):
		for mote in mote_list:
			self.add_mote(mote)

	def udgm_set_range(self, mote_range):
		radiomedium_index = self.simfile_lines.index('    <radiomedium>\r\n')
		if self.simfile_lines[radiomedium_index+1] == '      org.contikios.cooja.radiomediums.UDGM\r\n':
			self.simfile_lines.pop(radiomedium_index+2)
			self.simfile_lines.insert(radiomedium_index+2,"      <transmitting_range>%f</transmitting_range>\r\n" % mote_range)
		
		else:
			print("ERROR: radio model is not UDGM\r\n")

	def udgm_set_interference_range(self, interference_range):
		radiomedium_index = self.simfile_lines.index('    <radiomedium>\r\n')
		if self.simfile_lines[radiomedium_index+1] == '      org.contikios.cooja.radiomediums.UDGM\r\n':
			self.simfile_lines.pop(radiomedium_index+3)
			self.simfile_lines.insert(radiomedium_index+3,"      <interference_range>%f</interference_range>\r\n" % interference_range)
		
		else:
			print("ERROR: radio model is not UDGM\r\n")

	def udgm_set_rx_tx_ratios(self, rx, tx):
		radiomedium_index = self.simfile_lines.index('    <radiomedium>\r\n')
		if self.simfile_lines[radiomedium_index+1] == '      org.contikios.cooja.radiomediums.UDGM\r\n':
			self.simfile_lines.pop(radiomedium_index+4)
			self.simfile_lines.pop(radiomedium_index+4)
			self.simfile_lines.insert(radiomedium_index+4,"      <success_ratio_tx>%f</success_ratio_tx>\r\n" % tx)
			self.simfile_lines.insert(radiomedium_index+5,"      <success_ratio_rx>%f</success_ratio_rx>\r\n" % rx)
		
		else:
			print("ERROR: radio model is not UDGM\r\n")

	def set_timeout(self, timeout):
		script_index = all_indices('      <script>\r\n' ,self.simfile_lines)[0]
		self.simfile_lines.pop(script_index+1)
		self.simfile_lines.insert(script_index+1, '        TIMEOUT(%d);\r\n' % timeout)

	def set_dgrm_model(self, dgrm_file_path):
		dgrm_file = open(dgrm_file_path, 'r')
		radiomedium_open_index = self.simfile_lines.index('    <radiomedium>\r\n')
		radiomedium_close_index = self.simfile_lines.index('    </radiomedium>\r\n')
		remove_n_at(radiomedium_open_index+1, radiomedium_close_index - radiomedium_open_index -1, self.simfile_lines)
		self.simfile_lines.insert(radiomedium_open_index+1, '      org.contikios.cooja.radiomediums.DirectedGraphMedium\r\n')

		ptr = radiomedium_open_index+2

		#format:
		#71 20 0.00 0 0 0 -10.0 0 0

		total_errors = 0
		re_dgrm_line = re.compile('([0-9]+) ([0-9]+) ([0-9,.]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9,.,-]+) ([0-9]+) ([0-9]+)\r\n')
		for line in dgrm_file:
			match_dgrm_line = re_dgrm_line.match(line)
			if not match_dgrm_line:
				total_errors += 1
			else:
				src = match_dgrm_line.group(1)
				dst = match_dgrm_line.group(2)
				prr = match_dgrm_line.group(3)
				prr_ci = match_dgrm_line.group(4)
				num_tx = match_dgrm_line.group(5)
				num_rx = match_dgrm_line.group(6)
				rssi = match_dgrm_line.group(7)
				rssi_min = match_dgrm_line.group(8)
				rssi_max = match_dgrm_line.group(9)

				delay = 0;

				radio_edge = dgrm_generate(src,dst,prr,rssi,delay)

				self.simfile_lines = insert_list_at(radio_edge.splitlines(1), self.simfile_lines, ptr)
				#we don't increment ptr, just let the next insertions shift the existing edges down

		print ("total_errors = %d\r\n",total_errors)

	def save_simfile(self, simfilepath):
		simfile = open(simfilepath,'w')
		for line in self.simfile_lines:
			simfile.write(line)
		simfile.close()

	def add_mobility(self, mobility_path):
		text="""  <plugin>
    Mobility
    <plugin_config>
      <positions EXPORT="copy">POSITIONSFILE</positions>
    </plugin_config>
    <width>500</width>
    <z>0</z>
    <height>200</height>
    <location_x>210</location_x>
    <location_y>210</location_y>
  </plugin>\r\n"""
		text = text.replace('POSITIONSFILE', mobility_path)
		plugin_indexes = all_indices("  </plugin>\r\n", self.simfile_lines)
		self.simfile_lines = insert_list_at(text.splitlines(1), self.simfile_lines, plugin_indexes[-1]+1)

		simulation_indexes = all_indices("  <simulation>\r\n", self.simfile_lines)
		text = '  <project EXPORT="discard">[APPS_DIR]/mobility</project>\r\n'
		self.simfile_lines = insert_list_at(text, self.simfile_lines, simulation_indexes[-1])

def new_sim(templatepath = 'cooja-template.csc'):
	return Sim(templatepath)

def create_twistreplay_from_template(simfilepath):
	templatepath = 'twistreplay-template.csc'
	shutil.copyfile(templatepath, simfilepath)

def read_simfile(simfilepath):
	simfile = open(simfilepath,'r')
	simfile_lines = simfile.readlines()
	simfile.close()
	return simfile_lines

def all_indices(value, qlist):
    indices = []
    idx = -1
    while 1:
        try:
            idx = qlist.index(value, idx+1)
            indices.append(idx)
        except ValueError:
            break
    return indices

def remove_n_at(index,count,qlist):
	while count > 0:
		qlist.pop(index)
		count = count - 1
	return qlist

def insert_list_at(src,dst,index):
	cpt = 0
	for elem in src:
		dst.insert(index+cpt,elem)
		cpt = cpt+1
	return dst

def dgrm_generate(src,dst,prr,rssi,delay):
	template = """      <edge>
	<source>SRC</source>
	<dest>
	  org.contikios.cooja.radiomediums.DGRMDestinationRadio
	  <radio>DST</radio>
	  <ratio>PRR</ratio>
	  <signal>RSSI</signal>
	  <delay>DELAY</delay>
	</dest>
      </edge>\r\n"""

	template = template.replace('SRC',src)
	template = template.replace('DST',dst)
	template = template.replace('PRR',prr)
	template = template.replace('RSSI',rssi)
	template = template.replace('DELAY',str(delay))
	return template

def extract_node_id_list(nodeidpath):
	nodeids = []
	nodeidfile = open(nodeidpath, 'r')
	for line in nodeidfile:
		nodeids.append(line.rstrip())
	return nodeids

def mkdir(adir):
	try:
		os.makedirs(adir)
	except OSError:
		if os.path.exists(adir):
			# We are safe
			pass
		else:
			# There was an error on creation, so make sure we know about it
			raise

def cleardir(adir):
	for afile in os.listdir(adir):
		file_path = os.path.join(adir, afile)
		try:
			os.unlink(file_path)
		except Exception, e:
			print e

def export_mote_list(exportpath, motelist):
	exportfile = open(exportpath, 'w')
	for mote in motelist:
		exportfile.write("%d;%s;%s" %(mote.nodeid, mote.mote_type.shortname, mote.mote_type.target))
		if mote.mobility_data != None:
			for xy in mote.mobility_data:
				exportfile.write(";%2.2f,%2.2f" % (xy[0], xy[1]))
		exportfile.write("\r\n")
	exportfile.close()


class ConfigParser():

	def __init__(self):
		self.mote_types = []
		self.motelist = []
		self.simfiles = []

	def assign_mote_types(self, assignment, mote_count):
		motenames = [assignment['all'] for i in range(mote_count)]
		for key, value in assignment.iteritems():
			if key != 'all':
				motenames[int(key)] = value
		return motenames

	def mote_type_from_shortname(self, shortname):
		for mote_type in self.mote_types:
			if mote_type.shortname == shortname:
				return mote_type
		return None

        def add_mobility_data(self, data):
		for index in data:
			self.motelist[int(index)].set_mobility_data(data[index])

	def parse_config_file(self, config_path):
		print("LOADING CONFIG %s" % config_path)
		config_simgen = imp.load_source('module.name', config_path)
                return self.parse_config(config_simgen)
	def parse_config(self, config_simgen):

		self.check_config(config_simgen)

		outputfolder = config_simgen.outputfolder
		template_path = config_simgen.template
		mkdir(outputfolder)
		cleardir(outputfolder)

		now = time.strftime("%Y%m%d%H%M%S")

		previous_count = 0
		if config_simgen.topology != 'preset':
			for mote_count in config_simgen.mote_count:
				sim = self.init_simulation(template_path, config_simgen)
				coords = generators.gen(config_simgen, mote_count)
				if(previous_count == len(coords)):
					continue
				previous_count = len(coords)
				simfilepath = os.path.normpath(outputfolder) + os.path.sep + 'coojasim-' + config_simgen.topology + '-' + str(len(coords)) + '-' + now + '.csc'
				self.place_motes(sim, coords, simfilepath, config_simgen)

		else: #preset
			simlist = generators.load_preset(config_simgen.preset_data_path)
			for coords in simlist:
				sim = self.init_simulation(template_path, config_simgen)
				simfilepath = os.path.normpath(outputfolder) + os.path.sep + 'coojasim-' + os.path.splitext(os.path.basename(config_simgen.preset_data_path))[0] + '-' + str(len(coords)) + '-' + now + '.csc'
				self.place_motes(sim, coords, simfilepath, config_simgen)

		print("Done. Generated %d simfiles" % len(self.simfiles))
		return True

	def check_config(self, conf):
		self.check_param(conf, 'outputfolder', False, '..' + os.path.sep + 'output')
		self.check_param(conf, 'template', False, '..' + os.path.sep + 'templates' + os.path.sep + 'cooja-template-udgm.csc')
		self.check_param(conf, 'radio_model', False, 'udgm', True)
		if conf.radio_model == 'udgm':
			self.check_param(conf, 'tx_range', True, None)
			self.check_param(conf, 'tx_range', False, conf.tx_range)
			self.check_param(conf, 'rx_success', False, 1.0)
			self.check_param(conf, 'tx_success', False, 1.0)

	def check_param(self, conf, name, mandatory, default, force=False):
		if not hasattr(conf, name):
			if mandatory:
				sys.exit("COOJAGEN: ERROR - Parameter %s is mandatory" % name)
			else:
				setattr(conf, name, default)
		if force:
			param = getattr(conf, name)
			if param != default:
				sys.exit("COOJAGEN: ERROR - Parameter %s must be %s" % (name, default))

	def init_simulation(self, template_path, conf):
		sim = Sim(template_path)
		for mote_type in conf.mote_types:
			if 'target' not in mote_type:
				mote_type['target']='cooja'
			mote_type_obj = SimMoteType(mote_type['target'],
									mote_type['shortname'],
							mote_type['fw_folder'],
							mote_type['maketarget'],
							mote_type['makeargs'],
							mote_type['serial'],
							mote_type['description'])
			self.mote_types.append(mote_type_obj)
			sim.insert_motetype(mote_type_obj)

		sim.udgm_set_range(conf.tx_range)
		sim.udgm_set_interference_range(conf.tx_interference)
		sim.udgm_set_rx_tx_ratios(conf.rx_success, conf.tx_success)
		return sim

	def place_motes(self, sim, coords, simfilepath, conf):
		motenames = self.assign_mote_types(conf.assignment, len(coords))
		for index,coord in enumerate(coords):
			nodeid = index + 1
			mote = SimMote(self.mote_type_from_shortname(motenames[index]), nodeid)
			mote.set_coords(coord[0], coord[1], coord[2])
			self.motelist.append(mote)

		sim.add_motes(self.motelist)
		sim.set_timeout(999999999) #stop time in ms

		if hasattr(conf, 'mobility'):
			sim.add_mobility(conf.mobility)

		if hasattr(conf, 'interactive_mobility'):
			self.add_mobility_data(conf.interactive_mobility)

		sim.save_simfile(simfilepath)
		self.simfiles.append(simfilepath)
		print("****\n%s" % simfilepath)
		export_mote_list(simfilepath[:-4]+'.motes', self.motelist)

		self.motelist=[]

	def get_simfiles(self):
		return self.simfiles
