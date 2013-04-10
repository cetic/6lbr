#!/usr/bin/python2.7

import os
import sys
import re
from lib_postprocessing import *

"""
Test	Short description				Router	 Smart B    Transp B	Multi Router	Multi Smart B	Multi Trans B
S100x	Setup dag and ping mote				  Y	    Y	       Y
S110x	Setup dag and switch prefix with global repair		    Y
S111x	Setup dag and switch prefix with global repair		    Y
S200x	Setup dag and ping external host from mote	  Y	    Y	       Y			
S400x	Setup multi-br dag and ping fixed mote						    -		     Y		     Y
S500x	Setup multi-br disjoint dag and ping moving mote				    -		     Y		     Y
S501x	Setup multi-br overlapping dag and ping moving mote				    -		     Y		     Y
S502x	Setup multi-br overlapping dag and ping mote aver BR shutdown			    -		     Y		     Y
"""

results = []

#Generate a list of report dirs <mode>.test_Sxxxx
#Path example: report/run-20130411002034/coojasim-preset-1dag-10nodes-12-20130411002000/iter-000-01/RplRootTransparentBridge.test_S1101
for root, subFolders, files in os.walk("../report"):
    match_test_folder = re_test_folder.match(os.path.basename(root))
    if match_test_folder:
        results.append(Result(root, files))

map(lambda x: x.check_dependencies(), results)






