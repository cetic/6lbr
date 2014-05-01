#!/usr/bin/python

import subprocess
import sys

filetotest = ["6lh","6lr","6lbr","rpl","6lr-rpl","6lbr-rpl"]
filename = "graph.m"

def  compile_msp340():
	for i in filetotest:
		command = 'cd '+i+'; make TARGET=sky;  cd ../; mv process.sky '+i+'.sky'
		print(command)
		process = subprocess.Popen(command,stdout=subprocess.PIPE, shell=True)
		proc_stdout, error = process.communicate()
		print(proc_stdout)
		if error:
			sys.exit("Error to compile")


def get_msp340_size():
	command = "msp430-size"
	for i in filetotest:
		command = command+" "+i+".sky"
	print(command)
	process = subprocess.Popen(command,stdout=subprocess.PIPE, shell=True)
	proc_stdout = process.communicate()[0].strip()
	print(proc_stdout)
	return proc_stdout

def  compile_clean():
	for i in filetotest:
		command = 'cd '+i+'; rm -rf *sky* *.c *.h'
		print(command)
		process = subprocess.Popen(command,stdout=subprocess.PIPE, shell=True)
		proc_stdout = process.communicate()[0].strip()
		print(proc_stdout)

def get_data(input_size):
	data_out = {}
	data = input_size.split('\n')
	i = 0
	for line in data[1:len(data)]:
		val = line.split();
		data_out[filetotest[i]] = val[0:3]
		i += 1
	return data_out	

def gen_matlab_vec(data):
	x = ""
	y = ""
	for name in filetotest:
		x += "; '"+name+"'"
		y += "; "
		for d in data[name]:
			y += d+" "
	x = "x = {"+x[2:len(x)]+"};"
	y = "y = ["+y[1:len(y)]+"];"
	return x+"\n"+y+"\n";



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


if len(sys.argv) > 1 and sys.argv[1]=="clean":
	compile_clean()

compile_msp340()
data = gen_matlab_vec(get_data(get_msp340_size()))
code = addDataFile(filename, data)
writeToFile(filename, code)
