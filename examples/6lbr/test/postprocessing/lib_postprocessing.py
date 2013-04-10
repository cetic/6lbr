#!/usr/bin/python2.7
import re
import os
import sys

re_test_folder = re.compile("(.*).test_S([0-9]*)")


class Result:
    def __init__(self, report_path):
        match_test_folder = re_test_folder.match(os.path.basename(report_path))
        self.mode = match_test_folder.group(1)
        self.id = 'S'+match_test_folder.group(2)
        pathparts = report_path.split(os.path.sep)
        for part in pathparts:
            if 'coojasim' in part:
                self.topology = part
            elif 'iter-' in part:
                info = part.split('-')
                self.start_delay = info[-2]
                self.iteration = info[-1]
            elif 'run-' in part:
                self.timestamp = part.split('-')[-1]
        self.path = report_path
    def get_iter_folder(self):
        return os.path.dirname(self.path)
    def get_topology_folder(self):
        return os.path.dirname(os.path.dirname(self.path))
    def get_run_folder(self):
        return os.path.dirname(os.path.dirname(os.path.dirname(self.path)))
    def debug(self):
        print >> sys.stderr, "=======================\nDebugging %s object" % self.__class__.__name__
        print >> sys.stderr, "mode \t\t%s" % self.mode
        print >> sys.stderr, "id \t\t%s" % self.id
        print >> sys.stderr, "topology \t%s" % self.topology
        print >> sys.stderr, "start_delay \t%s" % self.start_delay
        print >> sys.stderr, "iteration \t%s" % self.iteration
        print >> sys.stderr, "path \t\t%s" % self.path
        print >> sys.stderr, "iter_folder \t%s" % self.get_iter_folder()
        print >> sys.stderr, "topology_folder %s" % self.get_topology_folder()
        print >> sys.stderr, "run_folder \t%s" % self.get_run_folder()
