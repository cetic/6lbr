#!/usr/bin/python2.7
import re
import os
import sys

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

re_test_folder = re.compile("(.*).test_S([0-9]*)")
dep = {'all' : ['COOJA.testlog', 'COOJA.log', 'br0.pcap', 'time.log', 'radiolog.pcap']}

class Result:
    def __init__(self, report_path, files = None):
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
        self.test_path = report_path
        self.test_files = files

    def get_iter_folder(self):
        return os.path.dirname(self.test_path)

    def get_topology_folder(self):
        return os.path.dirname(os.path.dirname(self.test_path))

    def get_run_folder(self):
        return os.path.dirname(os.path.dirname(os.path.dirname(self.test_path)))

    def get_stdout_path(self):
        return os.path.join(self.get_run_folder(), 'console_out.log')

    def get_stderr_path(self):
        return os.path.join(self.get_run_folder(), 'console_err.log')


    def debug(self):
        print >> sys.stderr, "=======================\nDebugging %s object" % self.__class__.__name__
        print >> sys.stderr, "mode \t\t%s" % self.mode
        print >> sys.stderr, "id \t\t%s" % self.id
        print >> sys.stderr, "topology \t%s" % self.topology
        print >> sys.stderr, "start_delay \t%s" % self.start_delay
        print >> sys.stderr, "iteration \t%s" % self.iteration
        print >> sys.stderr, "test_path \t\t%s" % self.test_path
        print >> sys.stderr, "iter_folder \t%s" % self.get_iter_folder()
        print >> sys.stderr, "topology_folder %s" % self.get_topology_folder()
        print >> sys.stderr, "run_folder \t%s" % self.get_run_folder()
        print >> sys.stderr, "test_files \t%s" % self.test_files

    def check_dependencies(self):
        error = []
        warning = []
        for mandatory_file in dep['all']:
            if mandatory_file not in self.test_files:
                print >> sys.stderr, "error: missing %s" % os.path.join(self.test_path,mandatory_file)
                error.append(mandatory_file)

        if not os.path.exists(self.get_stdout_path()):
            print >> sys.stderr, "warning: missing %s" % self.get_stdout_path()
            warning.append(self.get_stdout_path())
        if not os.path.exists(self.get_stderr_path()):
            print >> sys.stderr, "warning: missing %s" % self.get_stderr_path()
            warning.append(self.get_stderr_path())

        #TODO: check for testcase-specific files

        return error == []
