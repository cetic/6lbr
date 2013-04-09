#!/usr/bin/python2.7

import coojagen.src.lib_generation as simgen
import os
import imp
from os import system
import sys
import time
import config
import shutil

gen_config_name='gen_config.py'

def generate_config(current_topo):
    try:
        os.unlink(gen_config_name)
    except OSError:
        pass
    gen_config = open( gen_config_name, 'w')
    print >> gen_config, "import config"
    print >> gen_config, "config.multi_br=%d" % current_topo.multi_br
    #config.start_delay
    #config.disjoint ?
    #config.stop_br
    gen_config.close()

for simgen_config_path in config.topologies:
    parser = simgen.ConfigParser()

    print("LOADING CONFIG %s" % simgen_config_path)
    config_simgen = imp.load_source('module.name', simgen_config_path)
    if not parser.parse_config(config_simgen):
	sys.exit("topology generation error")

    simfiles = parser.get_simfiles()

    for simfile in simfiles:
        #Create the report work directories
        if not os.path.exists(config.report_path):
            os.makedirs(config.report_path)
        #Open and run the next COOJA topology
        running_simfile = open('.NEXT_TOPOLOGY', 'w')
        simname = os.path.basename(simfile).replace('.csc','')
        running_simfile.write(simfile)
        running_simfile.close()
        generate_config(config_simgen)
        #Run the test suite with the current topology
        system("python2.7 ./test.py")
        #Move the current coojasim working directory to its final location
        srcdir = os.path.dirname(os.path.dirname(config.report_path))
        shutil.copyfile(os.path.join('coojagen/output',simname+'.csc'),os.path.join(srcdir,simname+'.csc'))
        shutil.copyfile(os.path.join('coojagen/output',simname+'.motes'),os.path.join(srcdir,simname+'.motes'))
        destdir = os.path.join(os.path.dirname(srcdir),simname)
        if os.path.exists(destdir):
            if os.path.isdir(destdir):
                shutil.rmtree(destdir)
            else:
                os.unlink(destdir)
            os.rename(srcdir,destdir)

    #Move the current run working directory to its final location
    srcdir = os.path.dirname(os.path.dirname(os.path.dirname(config.report_path)))
    destdir = os.path.join(os.path.dirname(srcdir),'run-%s' % time.strftime("%Y%m%d%H%M%S"))
    if os.path.exists(destdir):
        if os.path.isdir(destdir):
            shutil.rmtree(destdir)
        else:
            os.unlink(destdir)
        os.rename(srcdir,destdir)

    os.unlink(".NEXT_TOPOLOGY")
    os.unlink(gen_config_name)

