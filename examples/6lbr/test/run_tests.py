#!/usr/bin/python2.7

import unittest
import argparse
import sys
import os

if os.path.exists('config.pyc'):
    os.unlink('config.pyc')

try:
    import config
except ImportError:
    print "Configuration file not found, using default..."


parser = argparse.ArgumentParser(description='6LBR tests')
parser.add_argument('--modes', metavar='<mode name>', dest='modes', action='append',
                   help='Modes to run')
parser.add_argument('--scenarios', metavar='<test scenarios name>', dest='scenarios',
                   help='Test scenarios to be executed')

args = parser.parse_args()

test_scenarios_module = __import__(args.scenarios, fromlist=[args.scenarios])
test_scenarios = getattr(test_scenarios_module, args.scenarios)

config.scenarios=test_scenarios

for mode in args.modes:
    mode_module=__import__(mode, fromlist=[mode])
    setattr(sys.modules['__main__'], mode, getattr(mode_module, mode))
    
unittest.main(argv=[sys.argv[0]], exit=False, verbosity=1)
