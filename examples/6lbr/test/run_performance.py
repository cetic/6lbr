#!/usr/bin/python2.7

import coojagen.src.lib_generation as simgen
import os
import imp
from os import system
import sys
import time
import shutil

try:
    import performance_config as config
except ImportError:
    print "Configuration file not found, using default..."

gen_config_name='gen_config.py'
gen_config_name_pyc='gen_config.pyc'

def init_performance_config():
    config.report_path=getattr(config, 'report_path', 'report')
    config.topologies=getattr(config, 'topologies', ['./coojagen/examples/config_preset_1dag_10nodes.py'])
    config.start_delays=getattr(config, 'start_delays', [0])
    config.test_repeat=getattr(config, 'test_repeat', 1)
    config.test_modes=getattr(config, 'test_modes', [])
    config.test_scenarios=getattr(config, 'test_scenarios', '')


def generate_config(name, report_path, start_delay=0):
    gen_config = open( gen_config_name, 'w')
    print >> gen_config, "import config"
    print >> gen_config, "config.simulation_path='%s'" % name
    print >> gen_config, "config.report_path='%s'" % report_path
    print >> gen_config, "config.start_delay=%d" % start_delay
    #config.stop_br
    gen_config.close()
    if os.path.exists(gen_config_name_pyc):
        os.unlink(gen_config_name_pyc)

init_performance_config()
if not os.path.exists(config.report_path):
    os.makedirs(config.report_path)
mod = 0

runname='run-%s' % time.strftime("%Y%m%d%H%M%S")

for simgen_config_path in config.topologies:
    parser = simgen.ConfigParser()

    print("LOADING CONFIG %s" % simgen_config_path)
    config_simgen = imp.load_source('module.name_%d' % mod, simgen_config_path)
    mod += 1
    if not parser.parse_config(config_simgen):
	sys.exit("topology generation error")

    simfiles = parser.get_simfiles()

    for simfile in simfiles:
        #Open and run the next COOJA topology
        simname = os.path.basename(simfile).replace('.csc','')
        for start_delay in config.start_delays:
            for i in range(1,config.test_repeat+1):
                itername='iter-%03d-%02d'% (start_delay, i)
                report_path=os.path.join(config.report_path, runname, simname, itername)
                os.makedirs(report_path)
                print >> sys.stderr, " ======================"
                print >> sys.stderr, " == ITER %03d : %02d ==" % (start_delay, i)
                generate_config(simfile, report_path, start_delay)
                #R² the test suite with the current topology
                modes=["--mode %s" % mode for mode in config.test_modes]
                system("python2.7 ./run_tests.py  --scenarios %s %s" % (config.test_scenarios, ' '.join(modes)))
                os.rename(gen_config_name, os.path.join(report_path, gen_config_name))
                os.unlink(gen_config_name_pyc)
        #Move the current coojasim working directory to its final location
        shutil.copyfile(os.path.join('coojagen/output', simname+'.csc'),os.path.join(report_path, simname+'.csc'))
        shutil.copyfile(os.path.join('coojagen/output', simname+'.motes'),os.path.join(report_path, simname+'.motes'))


