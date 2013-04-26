#!/usr/bin/python2.7

import os
import sys
import subprocess
import pickle

if len(sys.argv) < 2:
    print "Missing arguments"
    print "Usage: python %s <file.pcap>" % os.path.basename(sys.argv[0])
    exit(1)

infile = sys.argv[1]

if not os.path.exists(infile):
    print "The specified file does not exist"
    exit(1)
    
if os.path.splitext(infile)[1] != ".pcap":
    print "The specified file is not a PCAP file"
    exit(1)

fields = {}
fields["frame.number"] = 0
fields["frame.time_epoch"] = 1
fields["ipv6.src"] = 2
fields["ipv6.dst"] = 3
fields["udp"] = 4
fields["data.data"] = 5
fields["http"] = 6
fields["icmpv6"] = 7
fields["icmpv6.type"] = 8
fields["icmpv6.opt.prefix"] = 9
fields["icmpv6.nd.ns.target_address"] = 10
fields["icmpv6.nd.na.target_address"] = 11
fields["icmpv6.opt.linkaddr"] = 12
fields["ipv6.hlim"] = 13
fields["6lowpan"] = 14
fields["6lowpan.src"] = 15
fields["6lowpan.dst"] = 16

tshark_base = "tshark -r %s -R '!dns' -C tshark-6lbr-raw-udp -Tfields -Eseparator='|'"

tshark_export = ""
for k in sorted(fields, key=fields.get):
    tshark_export = tshark_export + " -e%s" % k

tshark_cmd = tshark_base + tshark_export
capdump = subprocess.check_output((tshark_cmd % infile),shell=True)

capture = []

for line in capdump.split('\n'):
    if not line:
        continue
    rawinfo = line.split('|')
    info = {}
    info["number"] = rawinfo[fields["frame.number"]]
    info["time"] = rawinfo[fields["frame.time_epoch"]]
    info["src"] = rawinfo[fields["ipv6.src"]]
    info["dst"] = rawinfo[fields["ipv6.dst"]]
    info["hoplimit"] = rawinfo[fields["ipv6.hlim"]]
    
    if rawinfo[fields["icmpv6"]] == "icmpv6":
        if rawinfo[fields["icmpv6.type"]] == "133":
            info["type"] = "ndp_rs"
        elif rawinfo[fields["icmpv6.type"]] == "134":
            info["type"] = "ndp_ra"
            info["prefix"] = rawinfo[fields["icmpv6.opt.prefix"]]
        elif rawinfo[fields["icmpv6.type"]] == "135":
            info["type"] = "ndp_ns"
            info["target"] = rawinfo[fields["icmpv6.nd.ns.target_address"]]
        elif rawinfo[fields["icmpv6.type"]] == "136":
            info["type"] = "ndp_na"
            info["target"] = rawinfo[fields["icmpv6.nd.na.target_address"]]
            info["link"] = rawinfo[fields["icmpv6.nd.na.target_address"]]
        elif rawinfo[fields["icmpv6.type"]] == "128":
            info["type"] = "echo_request"
        elif rawinfo[fields["icmpv6.type"]] == "129":
            info["type"] = "echo_reply"
        elif rawinfo[fields["icmpv6.type"]] == "1":
            info["type"] = "unreach"
        else:
            info["type"] = "icmpv6_other"
            info["code"] = rawinfo[fields["icmpv6.type"]]
    elif rawinfo[fields["udp"]].startswith("User Datagram Protocol"):
        info["type"] = "udp"
        info["data"] = rawinfo[fields["data.data"]]
    elif rawinfo[fields["http"]] == "http":
        info["type"] = "http"
    else:
        info["type"] = "other"

    capture.append(info)

outfile = infile.replace(".pcap",".pkl")
pickle.dump(capture,open(outfile,"wb"))


    
