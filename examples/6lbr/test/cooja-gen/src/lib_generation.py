#Python COOJA simulation generator
import os
import sys
import shutil
import re
import generators

class sim:
	def __init__(self, simfilepath, templatepath):
		self.templatepath = templatepath
		self.simfilepath = simfilepath
		shutil.copyfile(self.templatepath, self.simfilepath)
		self.simfile_lines = read_simfile(self.simfilepath)

	def create_sky_motetype(self, firmware_name, shortname, description):

		#simfile_lines = read_simfile(simfilepath)
		motetype_text = motetype_from_template(firmware_name, shortname, description)

		motetype_indexes = all_indices("    <motetype>\n",self.simfile_lines)
		motetype_close_indexes = all_indices("    </motetype>\n",self.simfile_lines)

		print(motetype_indexes)

		if len(motetype_indexes) == 1:
			#in case of 1 motetype, check if it's the template version or a real mote
			if self.simfile_lines[motetype_indexes[0]+2] == "      <identifier>templatesky1</identifier>\n":
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

	def add_mote(self, motetype_name, short_id, xpos, ypos):

		mote_text = mote_from_template(motetype_name, short_id, xpos, ypos)

		mote_indexes = all_indices("    <mote>\n",self.simfile_lines)
		mote_close_indexes = all_indices("    </mote>\n",self.simfile_lines)
	
		if len(mote_indexes) == 1:
			#only 1 mote, check if it's the template
			if self.simfile_lines[mote_indexes[0]+4] == "        <x>XPOS</x>\n":
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

	def add_motes(self, node_list):
	
	#nodelist.append(('sink1', coords[0], 1))
		for node in node_list:
			print("addmote: %s, %s, %f, %f" % (node['fw'],str(node['nodeid']), node['coords'][0], node['coords'][1]))
			self.add_mote(node['fw'], str(node['nodeid']), node['coords'][0], node['coords'][1])

	def udgm_set_range(self, mote_range):
		radiomedium_index = self.simfile_lines.index('    <radiomedium>\n')
		if self.simfile_lines[radiomedium_index+1] == '      se.sics.cooja.radiomediums.UDGM\n':
			self.simfile_lines.pop(radiomedium_index+2)
			self.simfile_lines.insert(radiomedium_index+2,"      <transmitting_range>%.1f</transmitting_range>\n" % mote_range)
		
		else:
			print("ERROR: radio model is not UDGM\n")

	def udgm_set_interference_range(self, interference_range):
		radiomedium_index = self.simfile_lines.index('    <radiomedium>\n')
		if self.simfile_lines[radiomedium_index+1] == '      se.sics.cooja.radiomediums.UDGM\n':
			self.simfile_lines.pop(radiomedium_index+3)
			self.simfile_lines.insert(radiomedium_index+3,"      <interference_range>%.1f</interference_range>\n" % interference_range)
		
		else:
			print("ERROR: radio model is not UDGM\n")

	def udgm_set_rx_tx_ratios(self, rx, tx):
		radiomedium_index = self.simfile_lines.index('    <radiomedium>\n')
		if self.simfile_lines[radiomedium_index+1] == '      se.sics.cooja.radiomediums.UDGM\n':
			self.simfile_lines.pop(radiomedium_index+4)
			self.simfile_lines.pop(radiomedium_index+4)
			self.simfile_lines.insert(radiomedium_index+4,"      <success_ratio_tx>%.1f</success_ratio_tx>\n" % tx)
			self.simfile_lines.insert(radiomedium_index+5,"      <success_ratio_rx>%.1f</success_ratio_rx>\n" % rx)
		
		else:
			print("ERROR: radio model is not UDGM\n")

	def set_timeout(self, timeout):
		script_index = all_indices('    <script>\n' ,self.simfile_lines)[0]
		self.simfile_lines.pop(script_index+1)
		self.simfile_lines.insert(script_index+1, '        TIMEOUT(%d);\n' % timeout)

	def set_dgrm_model(self, dgrm_file_path):
		dgrm_file = open(dgrm_file_path, 'r')
		radiomedium_open_index = self.simfile_lines.index('    <radiomedium>\n')
		radiomedium_close_index = self.simfile_lines.index('    </radiomedium>\n')
		remove_n_at(radiomedium_open_index+1, radiomedium_close_index - radiomedium_open_index -1, self.simfile_lines)
		self.simfile_lines.insert(radiomedium_open_index+1, '      se.sics.cooja.radiomediums.DirectedGraphMedium\n')

		ptr = radiomedium_open_index+2

		#format:
		#71 20 0.00 0 0 0 -10.0 0 0

		total_errors = 0
		re_dgrm_line = re.compile('([0-9]+) ([0-9]+) ([0-9,.]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9,.,-]+) ([0-9]+) ([0-9]+)\n')
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

		print ("total_errors = %d\n",total_errors)

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

def motetype_from_template(firmware_name, shortname, description):
	motetype ="""    <motetype>
      se.sics.cooja.mspmote.SkyMoteType
      <identifier>SHORTNAME</identifier>
      <description>DESCRIPTION</description>
      <source EXPORT="discard">[CONFIG_DIR]/FIRMWARE.c</source>
      <commands EXPORT="discard">make FIRMWARE.sky TARGET=sky</commands>
      <firmware EXPORT="copy">[CONFIG_DIR]/FIRMWARE.sky</firmware>
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
    </motetype>\n"""

	motetype = motetype.replace('SHORTNAME', shortname)
	motetype = motetype.replace('DESCRIPTION', description)
	motetype = motetype.replace('FIRMWARE', firmware_name)
	return motetype

def remove_n_at(index,count,qlist):
	while count > 0:
		qlist.pop(index)
		count = count - 1
	return qlist



def mote_from_template(motetype_name, short_id, xpos, ypos):

	if 'apptype' in motetype_name:
		mote = """	<mote>
      <interface_config>
	se.sics.cooja.motes.AbstractApplicationMoteType$SimpleMoteID
	<id>SHORT_ID</id>
      </interface_config>"""
	else:
		mote = """    <mote>
      <breakpoints />
      <interface_config>
	se.sics.cooja.mspmote.interfaces.MspMoteID
	<id>SHORT_ID</id>
      </interface_config>"""

	mote += """
      <interface_config>
	se.sics.cooja.interfaces.Position
	<x>XPOS</x>
	<y>YPOS</y>
	<z>ZPOS</z>
      </interface_config>
      
      <motetype_identifier>MOTETYPE_ID</motetype_identifier>
    </mote>\n"""

	mote = mote.replace('XPOS','%03.13f'%xpos)
	mote = mote.replace('YPOS','%03.13f'%ypos)
	mote = mote.replace('ZPOS','0')
	mote = mote.replace('SHORT_ID', short_id)
	mote = mote.replace('MOTETYPE_ID', motetype_name)

	return mote

def insert_list_at(src,dst,index):
	cpt = 0
	for elem in src:
		dst.insert(index+cpt,elem)
		cpt = cpt+1
	return dst

#    <radiomedium>
#      se.sics.cooja.radiomediums.UDGM
#      <transmitting_range>50.0</transmitting_range>
#      <interference_range>100.0</interference_range>
#      <success_ratio_tx>1.0</success_ratio_tx>
#      <success_ratio_rx>1.0</success_ratio_rx>
#    </radiomedium>


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
      </edge>\n"""

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



