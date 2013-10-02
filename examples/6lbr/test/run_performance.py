#!/usr/bin/python2.7

import coojagen.src.lib_generation as simgen
import os
import imp
from os import system
import sys
import time
import shutil

if os.path.exists('performance_config.pyc'):
    os.unlink('performance_config.pyc')

try:
    import performance_config as config
except ImportError:
    print "Configuration file not found, using default..."

gen_config_name='gen_config.py'
gen_config_name_pyc='gen_config.pyc'

def init_performance_config():
    config.report_path=getattr(config, 'report_path', 'report')
    config.topologies=getattr(config, 'topologies', ['./coojagen/examples/config_preset_1dag_10nodes.py'])
    config.dag_stabilisation_delays=getattr(config, 'dag_stabilisation_delays', [0])
    config.udp_intervals=getattr(config, 'udp_intervals', [10])
    config.test_repeat=getattr(config, 'test_repeat', 1)
    config.test_modes=getattr(config, 'test_modes', [])
    config.test_scenarios=getattr(config, 'test_scenarios', '')


def generate_config(topo_file, report_path, dag_stabilisation_delay, udp_interval):
    gen_config = open( gen_config_name, 'w')
    print >> gen_config, "import config"
    if topo_file:
        print >> gen_config, "config.simulation_path='%s'" % topo_file
    print >> gen_config, "config.report_path='%s'" % report_path
    print >> gen_config, "config.dag_stabilisation_delay=%d" % dag_stabilisation_delay
    print >> gen_config, "config.udp_interval=%d" % udp_interval
    #config.stop_br
    gen_config.close()
    if os.path.exists(gen_config_name_pyc):
        os.unlink(gen_config_name_pyc)

def run_topo(runname, topo_name, topo_file):
    for dag_stabilisation_delay in config.dag_stabilisation_delays:
      for udp_interval in config.udp_intervals:
        for i in range(1,config.test_repeat+1):
            itername='iter-%03d-%03d-%02d'% (dag_stabilisation_delay, udp_interval, i)
            report_path=os.path.join(config.report_path, runname, topo_name, itername)
            os.makedirs(report_path)
            print >> sys.stderr, " ================================="
            print >> sys.stderr, " == ITER %02d : DAG: %03d UDP: %03d ==" % (i, dag_stabilisation_delay, udp_interval)
            generate_config(topo_file, report_path, dag_stabilisation_delay, udp_interval)
            modes=["--mode %s" % mode for mode in config.test_modes]
            system("python2.7 ./run_tests.py  --scenarios %s %s" % (config.test_scenarios, ' '.join(modes)))
            if topo_file:
                os.rename(gen_config_name, os.path.join(report_path, gen_config_name))
            if os.path.exists(gen_config_name_pyc):
                os.unlink(gen_config_name_pyc)

init_performance_config()
if not os.path.exists(config.report_path):
    os.makedirs(config.report_path)
mod = 0

runname='run-%s' % time.strftime("%Y%m%d%H%M%S")

#Check to make sure we're not on the testbed
if config.topologies != 'CETIC Testbed':
    for simgen_config_path in config.topologies:
        parser = simgen.ConfigParser()

        print("LOADING CONFIG %s" % simgen_config_path)
        config_simgen = imp.load_source('module.name_%d' % mod, simgen_config_path)
        mod += 1
        if not parser.parse_config(config_simgen):
    	    sys.exit("topology generation error")

        simfiles = parser.get_simfiles()

        for simfile in simfiles:
            simname = os.path.basename(simfile).replace('.csc','')
            run_topo(runname, simname, simfile)
            #Move the current coojasim working directory to its final location
            report_path=os.path.join(config.report_path, runname, simname)
            shutil.copyfile(os.path.join('coojagen/output', simname+'.csc'),os.path.join(report_path, simname+'.csc'))
            shutil.copyfile(os.path.join('coojagen/output', simname+'.motes'),os.path.join(report_path, simname+'.motes'))

#Else, we are on CETIC Testbed. 
#This is a hack while we make the previous piece of code independant of COOJA
else:
    run_topo(runname, 'testbed', None)