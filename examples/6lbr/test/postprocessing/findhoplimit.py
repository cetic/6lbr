#!/usr/bin/python2.7

import os
import sys
import pickle
import re

if len(sys.argv)<2:
    print sys.argv
    exit(1)
    
basedir = sys.argv[1]

print "---" 
print basedir



remid = re.compile(".*serialpty;open;(\d+);.*")

moteid = 0
for line in open(os.path.join(basedir,"COOJA.log"),"r"):
    if "serialpty;open;" in line:
        matchid = remid.match(line)
        if matchid:
            moteid = int(matchid.group(1))

print "MoteId = %d (%02x)" % (moteid,moteid)

if moteid == 0:
    exit(1)

moteaddr = '74%02x:00%02x:%02x%02x'%(moteid,moteid,moteid,moteid)

#Compact the ipv6 address 
while True:
    moteaddrtmp = moteaddr.replace(":0",":")
    moteaddrtmp = moteaddrtmp.replace(":::","::")
    if moteaddrtmp == moteaddr:
        break
    else:
        moteaddr = moteaddrtmp
print "Mote suffix = %s" % moteaddr

os.system("./pcap2py.py %s" % os.path.join(basedir,"br0.pcap"))
brcap = pickle.load(open(os.path.join(basedir,"br0.pkl")))

#Serach for the last echo reply
pingreply = None
pingreplyindex = 0
for p in reversed(brcap):
    if p["type"]=="echo_reply" and p["dst"].endswith(moteaddr):
        pingreply = p
    pingreplyindex = pingreplyindex+1

if pingreply is None:
    print "No echo reply found"
    exit(1)
        
#print pingreplyindex
#print pingreply

pingrequest = None
pingrequestindex = 0
for i in range(pingreplyindex-1,0,-1):
    if brcap[i]["type"]=="echo_request":
        if brcap[i]["src"].endswith(moteaddr):
            pingrequest = brcap[i]
            pingrequestindex = i
            break

if pingrequest is None:
    print "No echo request found"
    exit(1)
#print pingrequestindex
#print pingrequest

print "Mote hop count = %d" % (64-int(pingrequest["hoplimit"]))