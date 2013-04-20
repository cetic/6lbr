#!/usr/bin/python2.7

"""
Start Test= 1365719423.979371
ms since start...
Network start = 1001.649857
Network started = 18143.102884
Mote start = 18151.414871
Mote detect start = 19177.054882
Mote detected = 19177.067995
Mote ping = 19177.072048
Mote reached = 28441.315889
Mote stopped = 28754.582882
Network stop = 28757.023811
Network stopped = 28757.169008
Stop Test = 29759.605885
"""

import os
import sys
import ast
import pylab


def parse_times(infile):

    if not os.path.exists(infile):
        exit(1)

    time_relative = {}
    time_delta = {}
    
    with open(infile) as f:
        for line in f:
            if line.find(" = ") != -1:
                rawinfo = line.split(" = ")
                if "Ping stat" in rawinfo[0]:
                    time_relative[rawinfo[0]] = ast.literal_eval(rawinfo[1])
                else:
                    time_relative[rawinfo[0]] = float(rawinfo[1])
    time_delta["test"] = time_relative["Stop Test"]
    time_delta["network-uptime"] = time_relative["Network stopped"] - time_relative["Network start"]
    time_delta["network-start"] = time_relative["Network started"] - time_relative["Network start"]
    time_delta["network-stop"] = time_relative["Network stopped"] - time_relative["Network stop"]
    time_delta["mote-uptime"] = time_relative["Mote stopped"] - time_relative["Mote start"]
    #time_delta["mote-start"] = time_relative["Mote detect start"] - time_relative["Mote start"]
    time_delta["mote-stop"] = time_relative["Mote stopped"] - time_relative["Mote reached"]
    time_delta["mote-detect"] = time_relative["Mote detected"] - time_relative["Mote start"]

    time_delta["ping1"] = time_relative["Mote reached"] - time_relative["Mote ping"]

    if time_relative.has_key("Moved mote ping"):
        time_delta["pingm"] = time_relative["Moved mote reached"] - time_relative["Moved mote ping"]
    elif time_relative.has_key("Mote ping2"):
        time_delta["ping2"] = time_relative["Mote reached2"] - time_relative["Mote ping2"]

    if time_relative.has_key("Ping stat"):
        #time_relative["Ping stat"] = ast.literal_eval(time_relative["Ping stat"])
        time_delta["ping2-stat"] = time_relative["Ping stat"]
        time_delta["ping2-mean"] = pylab.mean(time_relative["Ping stat"])
        time_delta["ping2-std"] = pylab.std(time_relative["Ping stat"])
        time_delta["ping2-var"] = pylab.var(time_relative["Ping stat"])

    return time_delta

if __name__ == "__main__":
    if len(argv)<2:
        exit(1)
    parse_times(sys.argv[1])
