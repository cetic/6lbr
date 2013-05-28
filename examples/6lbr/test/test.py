#!/usr/bin/python2.7

import unittest
import sys
from os import system
from subprocess import Popen
from time import sleep
import config
from support import *
from tcpdump import TcpDump
import time
from shutil import rmtree
import urllib2
import os

if os.path.exists('gen_config.py'):
    import gen_config
else:
    print "No generated config file found"

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
def repeat(times):
    def repeatHelper(f):
        def callHelper(*args):
            for i in range(0, times):
                f(*args)
        return callHelper
    return repeatHelper

class TestSupport:
    def __init__(self):
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
        try:
            topologyfile = open('.NEXT_TOPOLOGY', 'r')
            next_topology = topologyfile.readline().rstrip()
            topologyfile.close()
            self.wsn.setUp(next_topology)
        except IOError:
            print "Could not open .NEXT_TOPOLOGY topology file"
            self.wsn.setUp("TODO") #TODO: no default argument anymore here
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

    def start_ra(self, backbone):
        return self.platform.start_ra(backbone.itf,backbone.prefix)

    def stop_ra(self):
        return self.platform.stop_ra()

    def add_6lbr(self, radio=None):
        _br = config.brClass(self.backbone, self.wsn, radio)
        self.brList.append(_br)
        return _br

    def start_6lbr(self, log):
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
        if not os.path.exists(config.report_path):
            os.makedirs(config.report_path)

    def savereport(self,testname):
        srcdir = config.report_path
        try:
            os.rename('COOJA.log',os.path.join(srcdir,'COOJA.log'))
            os.rename('COOJA.testlog',os.path.join(srcdir,'COOJA.testlog'))
            for filename in os.listdir("."):
                if filename.startswith("radiolog-"):
                    os.rename(filename,os.path.join(srcdir,'radiolog.pcap'))            
        except OSError:
            pass
        destdir = os.path.join(os.path.dirname(srcdir),testname)
        if os.path.exists(destdir):
            if os.path.isdir(destdir):
                rmtree(destdir)
            else:
                os.unlink(destdir)
        os.rename(srcdir,destdir)

    def savereportmode(self,testname):
        srcdir = os.path.dirname(config.report_path)
        destdir = os.path.join(os.path.dirname(srcdir),testname)
        if os.path.exists(destdir):
            if os.path.isdir(destdir):
                dirs = os.listdir(srcdir)
                for node in dirs:
                    if os.path.exists(os.path.join(destdir,node)):
                        if os.path.isdir(os.path.join(destdir,node)):
                            rmtree(os.path.join(destdir,node))
                        else:
                            os.unlink(os.path.join(destdir,node))
                    os.rename(os.path.join(srcdir,node),os.path.join(destdir,node))
                os.rmdir(srcdir)
            else:
                os.unlink(destdir)
                os.rename(srcdir,destdir)
        else:
                os.rename(srcdir,destdir)

@skipUnlessTrue("mode_SmartBridgeManual")
@skipUnlessFalse("multi_br")
class SmartBridgeManual(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.bridge_mode=True
        self.support.backbone.prefix=config.wsn_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('SMART-BRIDGE', config.channel, accept_ra=False)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")

    def tear_down_network(self):
        self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip)

@skipUnlessTrue("mode_RouterNoRa")
@skipUnlessFalse("multi_br")
class RouterNoRa(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix=config.eth_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('ROUTER', config.channel, iid='100', ra_daemon=False)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.platform.add_route(config.wsn_prefix+"::", gw=self.br.ip), "")

    def tear_down_network(self):
        self.support.platform.rm_route(config.wsn_prefix+"::", gw=self.br.ip)
        self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip)

@skipUnlessTrue("mode_TransparentBridgeManual")
@skipUnlessFalse("multi_br")
class TransparentBridgeManual(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix=config.wsn_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=False)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")

    def tear_down_network(self):
        self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip)

@skipUnlessTrue("mode_TransparentBridgeAuto")
@skipUnlessFalse("multi_br")
class TransparentBridgeAuto(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix=config.wsn_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=True)

    def set_up_network(self):
        sleep(2)
        #self.support.platform.accept_ra(self.support.brList.itf)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.start_ra(self.support.backbone), "")

    def tear_down_network(self):
        self.support.stop_ra()

@skipUnlessTrue("mode_RplRoot")
@skipUnlessFalse("multi_br")
class RplRoot(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix=config.eth_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.setUp()
        self.br.set_mode('RPL-ROOT', config.channel, iid='100', ra_daemon=True, addr_rewrite=False)

    def set_up_network(self):
        sleep(10)
        self.assertTrue(self.support.platform.accept_ra(self.support.backbone.itf), "Could not enable RA configuration support")
        if self.support.platform.support_rio():
            self.assertTrue(self.support.platform.accept_rio(self.support.backbone.itf), "Could not enable RIO support")
        self.assertTrue(self.support.tcpdump.expect_ra(self.support.backbone.itf, 30), "")
        self.assertTrue(self.support.platform.check_prefix(self.support.backbone.itf, config.eth_prefix+':'), "Interface not configured")
        self.support.host.ip=self.support.platform.get_address_with_prefix(self.support.backbone.itf, config.eth_prefix+':')
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.add_route(config.wsn_prefix+"::", gw=self.br.ip), "Could not add route")

    def tear_down_network(self):
        if not self.support.platform.support_rio():
            self.support.platform.rm_route(config.wsn_prefix+"::", gw=self.br.ip)

@skipUnlessTrue("mode_RplRootNoRa")
@skipUnlessFalse("multi_br")
class RplRootNoRa(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix=config.eth_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('RPL-ROOT', config.channel, iid="100", ra_daemon=False, addr_rewrite=False)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.platform.add_route(config.wsn_prefix+"::", gw=self.br.ip), "")

    def tear_down_network(self):
        self.support.platform.rm_route(config.wsn_prefix+"::", gw=self.br.ip)
        self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip)
        
def main():
    if not os.path.exists(config.report_path):
        os.makedirs(config.report_path)
    unittest.main(exit=False, verbosity=1)

if __name__ == '__main__':
    main()
