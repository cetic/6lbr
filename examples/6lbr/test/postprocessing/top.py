#!/usr/bin/python2.7

import os
import sys
import re
from lib_postprocessing import *
import code
import readline

"""
Test	Short description				Router	 Smart B    Transp B	Multi Router	Multi Smart B	Multi Trans B
S100x	Setup dag and ping mote				  Y	    Y	       Y
S110x	Setup dag and switch prefix with global repair		    Y
S111x	Setup dag and switch prefix without global repair	    Y
S200x	Setup dag and ping external host from mote	  Y	    Y	       Y			
S400x	Setup multi-br dag and ping fixed mote						    -		     Y		     Y
S500x	Setup multi-br disjoint dag and ping moving mote				    -		     Y		     Y
S501x	Setup multi-br overlapping dag and ping moving mote				    -		     Y		     Y
S502x	Setup multi-br overlapping dag and ping mote after BR shutdown			    -		     Y		     Y
"""

results = []

#Generate a list of report dirs <mode>.test_Sxxxx
#Path example: report/run-20130411002034/coojasim-preset-1dag-10nodes-12-20130411002000/iter-000-01/RplRootTransparentBridge.test_S1101

report_root = "../report"
#report_root = "../report/run-20130417142318"
for root, subFolders, files in os.walk(report_root):
    match_test_folder = re_test_folder.match(os.path.basename(root))
    if match_test_folder:
        results.append(Result(root, files))

map(lambda x: x.check_dependencies(False), results)
results = prune_failed(results)

resfile = open("allres.txt", 'w')

for result in results:
    extract_ping_info(result)
    extract_time_info(result)
    topo_csc = result.get_file_topo_config()
    if topo_csc != None:
        topo_csc = os.path.basename(topo_csc)

    #print(result.mode, result.id, result.start_delay, result.iteration, result.ping_info, result.time_info['ping1'], topo_csc)

    if 'ping1' in result.time_info:
        ping1 = result.time_info['ping1']
    else:
        ping1 = -1
    if 'ping2' in result.time_info:
        ping2 = result.time_info['ping2']
    else:
        ping2 = -1
    if 'pingm' in result.time_info:
        pingm = result.time_info['pingm']
    else:
        pingm = -1

    if 'ping2-mean' in result.time_info:
        ping2mean = result.time_info['ping2-mean']
    else:
        ping2mean = -1
    if 'ping2-std' in result.time_info:
        ping2std = result.time_info['ping2-std']
    else:
        ping2std = -1
    if 'ping2-var' in result.time_info:
        ping2var = result.time_info['ping2-var']
    else:
        ping2var = -1
        
    resfile.write("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % (result.mode, result.id, result.start_delay, result.iteration, result.ping_info, ping1, ping2, pingm, ping2mean, ping2std, ping2var, topo_csc, result.test_path))

plot_all(results)

resfile.close()

vars = globals().copy()
vars.update(locals())
shell = code.InteractiveConsole(vars)
# shell.interact()







