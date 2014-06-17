#!/usr/bin/python

import sys
import json

data_file = "data.log"
matlab_file = "graph.m"

def getdata_from_file(tag, filename):
	data = {}
	f = open(filename, 'r')
	for line in f.readlines():
		if line.startswith("#"+tag+"->"):
			num, jsonstr = line.split("->")[1].split(":",1)
			data[int(num)] = json.loads(jsonstr)
	f.close()
	return data

def gen_matlab_vec(tag, data):
	numnote = ""
	time = ""
	num_msg_ip = ""
	num_msg_nd6 = ""

	for node in sorted(data):
		node_data = data[node]
		numnote += str(node)+" "
		time += str(node_data[u"time"])+" "
		msg_ip = str(node_data[u'msg'][u'ip'])
		num_msg_ip += msg_ip[1:len(msg_ip)-1]+" ; "
		msg_nd6 = str(node_data[u'msg'][u'nd6'])
		num_msg_nd6 += msg_nd6[1:len(msg_nd6)-1]+" ; "

	numnote = "number_of_note_"+tag+" = ["+numnote+"];"
	time = "time_"+tag+" = ["+time+"];"
	num_msg_ip = "msg_ip_"+tag+" = ["+num_msg_ip[0:len(num_msg_ip)-2]+"];"
	num_msg_nd6 = "msg_nd6_"+tag+" = ["+num_msg_nd6[0:len(num_msg_nd6)-2]+"];"

	return numnote+"\n"+time+"\n"+num_msg_ip+"\n"+num_msg_nd6+"\n";


def addDataFile(filename, data):
	code = "";
	f = open(filename, 'r')
	torm = False
	for l in f.readlines():
		if torm:
			if "enddata" in l:
				torm = False
			else:
				continue;
		code = code + l
		if " data" in l:
			code = code + data
			torm = True
	f.close()
	return code

def writeToFile(filename, text):
	f = open(filename, 'w')
	f.write(text)
	f.close()


if len(sys.argv)>1:

	dir_type = sys.argv[1]

	data_path = "../"+dir_type+"/"+data_file

	data_start = getdata_from_file("start", data_path)
	data_hour = getdata_from_file("1hour", data_path)

	matlab_data = gen_matlab_vec("start", data_start)+"\n"
	matlab_data += gen_matlab_vec("hour", data_hour)


	writeToFile(dir_type+"-"+matlab_file, addDataFile(matlab_file, matlab_data))



