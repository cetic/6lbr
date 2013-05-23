import unittest
import sys
from os import system
from subprocess import Popen
from time import sleep
import config
from support import *
from tcpdump import TcpDump
import time
import shutil
import urllib2
import os

def skipUnlessTrue(descriptor):
    if not hasattr(config, descriptor):
        return unittest.skip("%s not defined in config.py, skipping" % descriptor)
    if getattr(config, descriptor) == 0:
        return unittest.skip("%s set to False in config.py, skipping" % descriptor)
    else:
        return lambda func: func

def skipUnlessFalse(descriptor):
    if not hasattr(config, descriptor):
        return unittest.skip("%s not defined in config.py, skipping" % descriptor)
    if getattr(config, descriptor) != 0:
        return unittest.skip("%s set to True in config.py, skipping" % descriptor)
    else:
        return lambda func: func

class TestSupport:
    def __init__(self, test_name):
        self.test_name=test_name
        self.initreport()
        self.backbone=config.backboneClass()
        self.wsn=config.wsnClass()
        self.platform=config.platform
        self.tcpdump=TcpDump()
        self.host=Host(self.backbone)
        self.brList=[]
        self.test_mote=None

    def setUp(self):
        self.platform.setUp()
        self.backbone.setUp()
        self.host.setUp()
        self.wsn.setUp()
        for i, _br in enumerate(self.brList):
            print >> sys.stderr, "Setup 6LBR #%i" % i
            _br.setUp()
        self.test_mote = self.wsn.get_test_mote()

    def tearDown(self):
        for _br in self.brList:
            _br.tearDown()
        self.wsn.tearDown()
        self.host.tearDown()
        self.backbone.tearDown()
        self.platform.tearDown()
        self.savereport()

    def start_ra(self, backbone):
        return self.platform.start_ra(backbone.itf,backbone.prefix)

    def stop_ra(self):
        return self.platform.stop_ra()

    def add_6lbr(self, radio=None):
        _br = config.brClass(self.backbone, self.wsn, radio)
        self.brList.append(_br)
        return _br

    def start_6lbr(self, log=""):
        ret = True
        for _br in self.brList:
            ret = ret and _br.start_6lbr(log)
        return ret

    def stop_6lbr(self):
        ret = True
        for _br in self.brList:
            ret = ret and _br.stop_6lbr()
        return ret

    def start_mote(self):
        return self.test_mote.start_mote(config.channel)

    def stop_mote(self):
        return self.test_mote.stop_mote()

    def ping(self, target):
        return self.platform.ping(target)

    def wait_mote_in_6lbr(self, count):
        return True

    def wait_ping(self, count, target):
        for n in range(count):
            if (self.ping(target)):
                return True
        return False

    def ping_6lbr(self, br=None):
        if not br:
           br=self.brList[-1]
        return self.ping( br.ip )

    def wait_ping_6lbr(self, count, br=None):
        if not br:
           br=self.brList[-1]
        print >> sys.stderr, "Pinging BR %s" % br.ip
        return self.wait_ping( count, br.ip )

    def ping_mote(self):
        return self.ping( self.test_mote.ip )

    def wait_ping_mote(self, count):
        print >> sys.stderr, "Pinging mote %s" % self.test_mote.ip
        return self.wait_ping( count, self.test_mote.ip )

    def ping_from_mote(self, address, expect_reply=False, count=0):
        return self.test_mote.ping( address, expect_reply, count )

    def wait_ping_from_mote(self, count, target):
        print >> sys.stderr, "Pinging from mote %s" % self.test_mote.ip
        for n in range(count):
            if (self.ping_from_mote(target, True)):
                return True
        return False

    def start_udp_client(self, host = None, port = None):
        if host is None:
            host = self.host.ip
        if port is None:
            port = config.udp_port
        print >> sys.stderr, "Enable UDP traffic on test mote"
        ok = self.test_mote.send_cmd("udp-dest %s" % host)
        ok = ok and self.test_mote.send_cmd("udp-port %d" % port)
        ok = ok and self.test_mote.send_cmd("udp start")
        return ok

    def stop_udp_client(self):
        print >> sys.stderr, "Disable UDP traffic on test mote"
        return self.test_mote.send_cmd("udp stop")

    def start_udp_clients(self, host = None, port = None):
        if host is None:
            host = self.host.ip
        if port is None:
            port = config.udp_port
        print >> sys.stderr, "Enable UDP traffic"
        ok = self.wsn.send_cmd_all("udp-dest %s" % host)
        ok = ok and self.wsn.send_cmd_all("udp-port %d" % port)
        ok = ok and self.wsn.send_cmd_all("udp start")
        return ok

    def stop_udp_clients(self):
        print >> sys.stderr, "Disable UDP traffic"
        return self.wsn.send_cmd_all("udp stop")

    def initreport(self):
        self.test_report_path=os.path.join(config.report_path, self.test_name)
        #Trick to propagate test report path to support instances
        config.test_report_path=self.test_report_path
        if os.path.exists(self.test_report_path):
            shutil.rmtree(self.test_report_path, True)
        os.makedirs(self.test_report_path)

    def savereport(self):
        try:
            shutil.move('COOJA.log',self.test_report_path)
        except IOError:
            pass
        try:
           shutil.move('COOJA.testlog',self.test_report_path)
        except IOError:
            pass
        for filename in os.listdir("."):
                if filename.startswith("radiolog-"):
                    try:
                        shutil.move(filename,self.test_report_path)
                    except IOError:
                        pass

class TestScenarios:
    def log_file(self, log_name):
        return "%s_%s.log" % (log_name, self.__class__.__name__)

    def print_test_name(self):
        print >> sys.stderr, "\n******** %s ********" % self.test_name

    def setUp(self):
        self.test_name=self.__class__.__name__ + '.' + self._testMethodName
        self.multi_br=False
        self.bridge_mode=False
        self.print_test_name()
        self.support=TestSupport(self.test_name)
        self.modeSetUp()

    def tearDown(self):
        self.support.platform.udpsrv_stop()
        self.tear_down_network()
        self.support.tearDown()
