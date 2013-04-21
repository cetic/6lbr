#!/usr/bin/python2.7
import re
import os
import sys
import pp_time
import pp_pings
import pp_plots

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

#Due to various regex considerations, dots in filename must be expressed with a '~', it obviously means that no '~' are allowed in the original filename
#'?' will identify one (and only one) variable character
#'*' will identify a variable substring
# dep = { "test" : {'all' : ['COOJA~testlog', 'COOJA~log', 'br0~pcap', 'br0~pkl', 'time~log', 'radiolog~pcap', '6lbr_tap0~log'], 'S10??' : ['ping~log'], 'S11??' : ['ping1~log', 'ping2~log'], 'S40??' : ['ping~log'], 'S5???' : ['ping~log'], '*Multi*' : ['6lbr_tap1~log'] , 'RplRootMulti*' : ['6lbr_tap2~log']},
dep = { "test" : {'all' : ['COOJA~testlog', 'COOJA~log', 'br0~pkl', 'time~log', '6lbr_tap0~log'], 'S10??' : ['ping~log'], 'S11??' : ['ping1~log', 'ping2~log'], 'S40??' : ['ping~log'], 'S5???' : ['ping~log'], '*Multi*' : ['6lbr_tap1~log'] , 'RplRootMulti*' : ['6lbr_tap2~log']},
        "iter" : {'all' : ['gen_config~py'] },
        "topo" : {'all' : ['coojasim-*~csc', 'coojasim-*~motes'] },
        "run" : {'all' : ['console_out~log', 'console_err~log'] },
    }

class Result:
    def __init__(self, report_path, files = None):
        match_test_folder = re_test_folder.match(os.path.basename(report_path))
        self.mode = match_test_folder.group(1)
        self.id = 'S'+match_test_folder.group(2)
        pathparts = report_path.split(os.path.sep)
        for part in pathparts:
            if 'coojasim-' in part:
                self.topology = part
            elif 'iter-' in part:
                info = part.split('-')
                self.start_delay = info[-2]
                self.iteration = info[-1]
            elif 'run-' in part:
                self.timestamp = part.split('-')[-1]
        self.test_path = report_path
        self.test_files = files
        self.file_index = {}

    def get_folder_test(self):
        return self.test_path

    def get_folder_iter(self):
        return os.path.dirname(self.test_path)

    def get_folder_topo(self):
        return os.path.dirname(os.path.dirname(self.test_path))

    def get_folder_run(self):
        return os.path.dirname(os.path.dirname(os.path.dirname(self.test_path)))

    def set_ping_info(self, info):
        self.ping_info = info

    def set_time_info(self, info):
        self.time_info = info

    def get_files(self, level, groups):
        outfiles = {}
        if dep.has_key(level):
            levelbasepath = getattr(self, "get_folder_%s" % level)()
            levelfiles = os.listdir(levelbasepath)
            for group in groups:
                if dep[level].has_key(group):
                    for expectedfile in dep[level][group]:
                        matched = False
                        for realfile in levelfiles:
                            if os.path.isdir(os.path.join(levelbasepath,realfile)):
                                continue
                            regx = re.compile(expectedfile.replace("~","\.").replace("?",".?").replace("*",".*"))
                            match = regx.match(realfile)
                            if match is not None:
                                matched = True;
                                outfiles[expectedfile.replace("?","").replace("*","").replace("~","").replace("_","").replace("-","")] = os.path.join(levelbasepath,realfile)
                                break
                        if not matched:
                            outfiles[expectedfile.replace("?","").replace("*","").replace("~","").replace("_","").replace("-","")] = None
            return outfiles                   
        else:
            return None

    def get_file_run_stdout(self):
        if "consoleoutlog" in self.file_index:
            return self.file_index["consoleoutlog"]

    def get_file_run_stderr(self):
        if "consoleerrlog" in self.file_index:
            return self.file_index["consoleerrlog"]

    def get_file_topo_config(self):
        if "coojasimcsc" in self.file_index:
            return self.file_index["coojasimcsc"]
        
    def get_file_topo_motes(self):
        if "coojasimmotes" in self.file_index:
            return self.file_index["coojasimmotes"]

    def get_file_topo_genconfig(self):
        if "genconfigpy" in self.file_index:
            return self.file_index["genconfigpy"]

    def get_file_timelog(self):
        if "timelog" in self.file_index:
            return self.file_index['timelog']

    def get_ping_logs(self):
        pinglogs = {}
        if "pinglog" in self.file_index:
            pinglogs["ping1"] = self.file_index["pinglog"]
        elif "ping1log" in self.file_index:
            pinglogs["ping1"] = self.file_index["ping1log"]
        if "ping2log" in self.file_index:
            pinglogs["ping2"] = self.file_index["ping2log"]

        return pinglogs

    def debug(self):
        print >> sys.stderr, "=======================\nDebugging %s object" % self.__class__.__name__
        print >> sys.stderr, "mode \t\t%s" % self.mode
        print >> sys.stderr, "id \t\t%s" % self.id
        print >> sys.stderr, "topology \t%s" % self.topology
        print >> sys.stderr, "start_delay \t%s" % self.start_delay
        print >> sys.stderr, "iteration \t%s" % self.iteration
        print >> sys.stderr, "test_path \t\t%s" % self.test_path
        print >> sys.stderr, "iter_folder \t%s" % self.get_folder_iter()
        print >> sys.stderr, "topology_folder %s" % self.get_folder_topo()
        print >> sys.stderr, "run_folder \t%s" % self.get_folder_run()
        print >> sys.stderr, "test_files \t%s" % self.test_files

    def match_dependencies_groups(self,level):
        matched_dep_groups = []
        matched_dep_groups.append("all")
        for dep_name in dep[level]:
            if dep_name == "all":
                continue
            dep_name_regx = dep_name.replace("?",".?").replace("*",".*")
            redep = re.compile(dep_name_regx)
            rematchid = redep.match(self.id)
            rematchmode = redep.match(self.mode)
            if rematchid is not None:
                matched_dep_groups.append(dep_name)
            elif rematchmode is not None:
                matched_dep_groups.append(dep_name)
        return matched_dep_groups

    def check_dependencies(self, with_print=True):
        error = []
        #warning = []
        #for dep_group in self.match_dependencies_groups("test"):
        #    for mandatory_file in dep_group:
        #        mandatory_file_fix = mandatory_file.replace('~','.')
        #        if mandatory_file_fix not in self.test_files:
        #            print >> sys.stderr, "error: missing %s" % os.path.join(self.test_path,mandatory_file_fix)
        #            error.append(mandatory_file_fix)

        #if not os.path.exists(self.get_file_stdout()):
        #    print >> sys.stderr, "warning: missing %s" % self.get_file_stdout()
        #    warning.append(self.get_file_stdout())
        #if not os.path.exists(self.get_file_stderr()):
        #    print >> sys.stderr, "warning: missing %s" % self.get_file_stderr()
        #    warning.append(self.get_file_stderr())
        self.file_index.clear()
        for level in dep:
            self.file_index.update(self.get_files(level,self.match_dependencies_groups(level)))

        for k,v in self.file_index.items():
            if v is None:
                if len(error) == 0:
                    if(with_print):
                        print "---"
                if(with_print):
                    print >> sys.stderr, "error: missing '%s' for test '%s'" % (k,self.test_path)
                error.append(k)

        return error == []


def prune_failed(results):
    res = []
    for result in results:
        if result.get_file_timelog()!=None and os.path.exists(result.get_file_timelog()):
            res.append(result)

    return res

def extract_ping_info(result):
    pingfiles = result.get_ping_logs()
    ping_info = {}

    donothing = True
    for elem in pingfiles:
        if pingfiles[elem] != None:
            donothing = False
            ping_info[elem] = pp_pings.parse_ping(pingfiles[elem])
        result.set_ping_info(ping_info)
    if donothing:
        ping_info['ping1'] = pp_pings.parse_pingmote([result.file_index['COOJAtestlog'], result.file_index['br0pkl']])
        result.set_ping_info(ping_info)

def extract_time_info(result):
    result.set_time_info(pp_time.parse_times(result.get_file_timelog()))

def plot_all(results):
    #pp_plots.plot1(results)
    pp_plots.plot_all(results)
