#!/usr/bin/python

import os
import shutil
import glob

def getCode(line):
	filename = line[line.index("(")+1:line.index(")")]
	f = open(filename, 'r')
	code = "";
	for l in f.readlines():
		code = code + l
	return code

def getCodeFile(filename):
	code = "";
	f = open(filename, 'r')
	torm = False
	for l in f.readlines():
		if torm:
			if "endload" in l:
				torm = False
			else:
				continue;
		code = code + l
		if " load" in l:
			code = code + getCode(l)
			torm = True
	f.close()
	return code

def writeToFile(filename, text):
	f = open(filename, 'w')
	f.write(text)
	f.close()


#Get last version of the library
shutil.copy("../../examples/6lowpan-nd/test/lib.js", "lib.js")

# Refresh .js
filelist = glob.glob("*/*.js");
for item in filelist:
	print "---------"+item+"---------"
	text = getCodeFile(item)
	#print text
	writeToFile(item, text)

#Remove all file no more used
os.remove("lib.js")

