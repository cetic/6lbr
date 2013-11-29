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
parser.add_argument('--log', metavar='<test log file name>', dest='log', default='test.log',
                   help='Test log file name')

args = parser.parse_args()

test_scenarios_module = __import__(args.scenarios, fromlist=[args.scenarios])
test_scenarios = getattr(test_scenarios_module, args.scenarios)

config.scenarios=test_scenarios

for mode in args.modes:
    mode_module=__import__(mode, fromlist=[mode])
    setattr(sys.modules['__main__'], mode, getattr(mode_module, mode))
    
ut=unittest.main(argv=[sys.argv[0]], exit=False, verbosity=1)
result=open(os.path.join(config.report_path, args.log), "w")
print >> result, "SUMMARY\n"
print >> result, "Tests run: %d" % ut.result.testsRun
print >> result, "Tests skipped: %d" % len(ut.result.skipped)
print >> result, "Failed tests: %d" % len(ut.result.failures)
print >> result, "Aborted tests: %d" % len(ut.result.errors)
print >> result, "\nDETAILS\n"
for failure in ut.result.failures:
    (testcase, traceback) = failure
    print >> result, '======================================================================'
    print >> result, 'FAIL: %s.%s' % ( testcase.__class__.__name__, testcase._testMethodName)
    print >> result, '----------------------------------------------------------------------'
    print >> result, traceback
    print >> result, '----------------------------------------------------------------------'

for error in ut.result.errors:
    (testcase, traceback) = error
    print >> result, '======================================================================'
    print >> result, 'ERROR: %s.%s' % ( testcase.__class__.__name__, testcase._testMethodName)
    print >> result, '----------------------------------------------------------------------'
    print >> result, traceback
    print >> result, '----------------------------------------------------------------------'
for skipped in ut.result.skipped:
    (testcase, reason) = skipped
    print >> result, '======================================================================'
    print >> result, 'SKIPPED: %s.%s' % ( testcase.__class__.__name__, testcase._testMethodName)
    print >> result, '----------------------------------------------------------------------'
    print >> result, reason
    print >> result, '----------------------------------------------------------------------'

result.close()
