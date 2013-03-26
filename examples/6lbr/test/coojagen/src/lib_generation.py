#Python COOJA simulation generator
import os
import sys
import shutil
import re
import generators
import imp

class sim_mote_type:
	def __init__(self, shortname, fw_folder, maketarget, makeargs, serial, description):
		self.shortname = shortname
		self.fw_folder = os.path.normpath(fw_folder)
		self.maketarget = maketarget
		self.makeargs = makeargs
		self.serial = serial
		self.description = description

	def text_from_template(self):
		text ="""    <motetype>
      se.sics.cooja.mspmote.SkyMoteType
      <identifier>SHORTNAME</identifier>
      <description>DESCRIPTION</description>
      <source EXPORT="discard">FIRMWAREPATH</source>
      <commands EXPORT="discard">make FIRMWARE.sky TARGET=sky MAKEARGS</commands>
      <firmware EXPORT="copy">FIRMWAREBIN</firmware>
      <moteinterface>se.sics.cooja.interfaces.Position</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyButton</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyFlash</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyByteRadio</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>\r\n"""
		text = text.replace('FIRMWAREPATH', self.fw_folder + os.path.sep + self.maketarget + '.c')
		text = text.replace('FIRMWAREBIN', self.fw_folder + os.path.sep + self.maketarget + '.sky')
		text = text.replace('SHORTNAME', self.shortname)
		text = text.replace('DESCRIPTION', self.description)
		text = text.replace('FIRMWARE', self.maketarget)
		text = text.replace('MAKEARGS', self.makeargs)
		return text

class sim_mote:
	def __init__(self, mote_type, nodeid):
		self.mote_type = mote_type
		self.nodeid = nodeid
	def set_coords(self, xpos, ypos, zpos=0):
		self.xpos = xpos
		self.ypos = ypos
		self.zpos = zpos

	def text_from_template(self):


		text = """    <mote>
      <breakpoints />
      <interface_config>
	se.sics.cooja.mspmote.interfaces.MspMoteID
	<id>NODE_ID</id>
      </interface_config>
      <interface_config>
	se.sics.cooja.interfaces.Position
	<x>XPOS</x>
	<y>YPOS</y>
	<z>ZPOS</z>
      </interface_config>
      <motetype_identifier>MOTETYPE_ID</motetype_identifier>
    </mote>\r\n"""

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
    SerialSocketServer
    <mote_arg>MOTEARG</mote_arg>
    <width>459</width>
    <z>4</z>
    <height>119</height>
    <location_x>5</location_x>
    <location_y>525</location_y>
  </plugin>\r\n"""
		else:
			return ''

		text = text.replace('MOTEARG','%d' % (self.nodeid-1))
		return text

class Sim():
	def __init__(self, simfilepath, templatepath):
		self.templatepath = templatepath
		self.simfilepath = simfilepath
		shutil.copyfile(self.templatepath, self.simfilepath)
		self.simfile_lines = read_simfile(self.simfilepath)

	def insert_sky_motetype(self, mote_type):

		motetype_text = mote_type.text_from_template()

		motetype_indexes = all_indices("    <motetype>\r\n",self.simfile_lines)
		motetype_close_indexes = all_indices("    </motetype>\r\n",self.simfile_lines)

		if len(motetype_indexes) == 1:
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
		if self.simfile_lines[radiomedium_index+1] == '      se.sics.cooja.radiomediums.UDGM\r\n':
			self.simfile_lines.pop(radiomedium_index+2)
			self.simfile_lines.insert(radiomedium_index+2,"      <transmitting_range>%.1f</transmitting_range>\r\n" % mote_range)
		
		else:
			print("ERROR: radio model is not UDGM\r\n")

	def udgm_set_interference_range(self, interference_range):
		radiomedium_index = self.simfile_lines.index('    <radiomedium>\r\n')
		if self.simfile_lines[radiomedium_index+1] == '      se.sics.cooja.radiomediums.UDGM\r\n':
			self.simfile_lines.pop(radiomedium_index+3)
			self.simfile_lines.insert(radiomedium_index+3,"      <interference_range>%.1f</interference_range>\r\n" % interference_range)
		
		else:
			print("ERROR: radio model is not UDGM\r\n")

	def udgm_set_rx_tx_ratios(self, rx, tx):
		radiomedium_index = self.simfile_lines.index('    <radiomedium>\r\n')
		if self.simfile_lines[radiomedium_index+1] == '      se.sics.cooja.radiomediums.UDGM\r\n':
			self.simfile_lines.pop(radiomedium_index+4)
			self.simfile_lines.pop(radiomedium_index+4)
			self.simfile_lines.insert(radiomedium_index+4,"      <success_ratio_tx>%.1f</success_ratio_tx>\r\n" % tx)
			self.simfile_lines.insert(radiomedium_index+5,"      <success_ratio_rx>%.1f</success_ratio_rx>\r\n" % rx)
		
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
		self.simfile_lines.insert(radiomedium_open_index+1, '      se.sics.cooja.radiomediums.DirectedGraphMedium\r\n')

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

	def save_simfile(self):
		simfile = open(self.simfilepath,'w')
		for line in self.simfile_lines:
			simfile.write(line)
		simfile.close()

def new_sim(simfilepath, templatepath = 'cooja-template.csc'):
	return sim(simfilepath, templatepath)

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
	  se.sics.cooja.radiomediums.DGRMDestinationRadio
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

class config_parser():

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

	def parse_config(self, config_path):
		config_simgen = imp.load_source('module.name', config_path)
		if hasattr(config_simgen, 'outputfolder'):
			outputfolder = config_simgen.outputfolder
		else:
			outputfolder = '..' + os.path.sep + 'output'

		if hasattr(config_simgen, 'template'):
			template_path = config_simgen.template
		else:
			template_path = '..' + os.path.sep + 'templates' . os.path.sep + 'cooja-template-udgm.csc'


		mkdir(outputfolder)
		cleardir(outputfolder)
		

		for mote_count in config_simgen.mote_count:
			motenames = self.assign_mote_types(config_simgen.assignment, mote_count)
			simfilepath = os.path.normpath(outputfolder) + os.path.sep + 'simfile-' + str(mote_count) + '-nodes.csc'
			sim = Sim(simfilepath, template_path)

			for mote_type in config_simgen.mote_types:
				mote_type_obj = sim_mote_type(	mote_type['shortname'],
										mote_type['fw_folder'],
										mote_type['maketarget'],
										mote_type['makeargs'],
										mote_type['serial'],
										mote_type['description'])
				self.mote_types.append(mote_type_obj)
				sim.insert_sky_motetype(mote_type_obj)

			sim.udgm_set_range(config_simgen.tx_range)
			sim.udgm_set_interference_range(config_simgen.tx_interference)

			coords = generators.genline(config_simgen.step, mote_count)

			for index,coord in enumerate(coords):
				nodeid = index + 1
				mote = sim_mote(self.mote_type_from_shortname(motenames[index]), nodeid)
				mote.set_coords(coord[0], coord[1], coord[2])
				self.motelist.append(mote)

			sim.add_motes(self.motelist)
			sim.set_timeout(999999999) #stop time in ms

			sim.save_simfile()
			self.simfiles.append(simfilepath)
			print("****\n%s" % simfilepath)

			self.motelist=[]

		print("Done. Generated %d simfiles" % len(self.simfiles))

	def get_simfiles(self):
		return self.simfiles
