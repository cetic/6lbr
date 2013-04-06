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

def skipUnlessTrue(descriptor):
    if not hasattr(config, descriptor):
        return unittest.skip("%s not defined in config.py, skipping" % descriptor)
    if getattr(config, descriptor) == 0:
        return unittest.skip("%s set to False in config.py, skipping" % descriptor)
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
        print >> sys.stderr, "\n---\n"
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

    def start_ra(self, itf):
        return self.platform.start_ra(itf)

    def stop_ra(self):
        return self.platform.stop_ra()

    def add_6lbr(self):
        _br = config.brClass(self.backbone, self.wsn)
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
        return self.wait_ping( count, br.ip )

    def ping_mote(self):
        return self.ping( self.test_mote.ip )

    def wait_ping_mote(self, count):
        print("Pinging mote %s" % self.test_mote.ip)
        return self.wait_ping( count, self.test_mote.ip )

    def ping_from_mote(self, address, expect_reply=False, count=0):
        return self.test_mote.ping( address, expect_reply, count )

    def initreport(self):
        if not os.path.exists(config.report_path):
            os.makedirs(config.report_path)

    def savereport(self,testname):
        srcdir = config.report_path
        os.rename('COOJA.log',os.path.join(srcdir,'COOJA.log'))
        os.rename('COOJA.testlog',os.path.join(srcdir,'COOJA.testlog'))
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

class TestScenarios:
    def log_file(self, log_name):
        return "%s_%s.log" % (log_name, self.__class__.__name__)

    @skipUnlessTrue("S0")
    def test_S0(self):
        """
        Check 6LBR start-up and connectivity
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S00 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_6lbr(40), "6LBR is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
	self.support.savereport(testname)

    @skipUnlessTrue("S1")
    def test_S1(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S01 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        ping_thread = self.support.platform.ping_run(self.support.test_mote.ip,0.5,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        self.support.platform.ping_stop(ping_thread)
        timenetunset = time.time()
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
	self.support.savereport(testname)

    @skipUnlessTrue("S2")
    def test_S2(self):
        """
        Ping from the computer to the mote when the PC does not know the BR and the BR knows
        the mote.
        """
        print >> sys.stderr, "******** Test S02 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)

    @skipUnlessTrue("S3")
    def test_S3(self):
        """
        Ping from the computer to the mote when everyone is known but the mote has been disconnected.
        """
        print >> sys.stderr, "******** Test S03 ********"
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.assertFalse(self.support.wait_ping_mote(10), "Mote is still responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S4")
    def test_S4(self):
        """
        Starting from a stable RPL topology, restart the border router and observe how it attaches
        to the RPL DODAG.
        """
        print >> sys.stderr, "******** Test S04 ********"
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S5")
    def test_S5(self):
        """
        Wait for a DAD between the computer and the BR, then disconnect and reconnect the com-
        puter and observe the reaction of the BR to a computer's DAD.
        """
        print >> sys.stderr, "******** Test S05 ********"
        pass

    @skipUnlessTrue("S6")
    def test_S6(self):
        """
        Observe the NUDs between the computer and the BR.
        """
        print >> sys.stderr, "******** Test S06 ********"
        pass

    @skipUnlessTrue("S7")
    def test_S7(self):
        """
        Test the Auconfiguration process of the BR in bridge mode and observe its ability to take a
        router prefix (by using the computer as a router), and deal with new RA once configured.
        """
        print >> sys.stderr, "******** Test S07 ********"
        pass

    @skipUnlessTrue("S8")
    def test_S8(self):
        """
        Observe the propagation of the RIO in the WSN side (when supported in the WPAN).
        """
        print >> sys.stderr, "******** Test S08 ********"
        pass

    @skipUnlessTrue("S9")
    def test_S9(self):
        """
        Test the using of the default router.
        """
        print >> sys.stderr, "******** Test S09 ********"
        pass

    @skipUnlessTrue("S10")
    def test_S10(self):
        """
        Ping from the sensor to the computer when the sensor does not know the CBR.
        """
        print >> sys.stderr, "******** Test S10 ********"
        pass

    @skipUnlessTrue("S11")
    def test_S11(self):
        """
        Ping from the sensor to the computer when the CBR does not know the computer.
        """
        print >> sys.stderr, "******** Test S11 ********"
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.assertTrue(self.support.ping_from_mote(self.support.host.ip, True, 60), "Host is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S12")
    def test_S12(self):
        """
        Ping from the sensor to an external domain (as the inet address of google.com) and
        observe all the sending process.
        """
        print >> sys.stderr, "******** Test S12 ********"
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        if self.__class__.__name__ == 'SmartBridgeAuto':
            self.assertTrue(self.support.tcpdump.expect_ping_request(self.support.backbone.itf, "cccc::1", 30, bg=True), "")
        else:
            self.assertTrue(self.support.tcpdump.expect_ns(self.support.backbone.itf, [0xbbbb, 0, 0, 0, 0, 0, 0, 1], 30, bg=True), "")
        self.assertTrue(self.support.ping_from_mote("cccc::1"), "")
        self.assertTrue(self.support.tcpdump.check_result(), "")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S1001")
    def test_S1001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S1001 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timenetsetdone = time.time()
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        timenetunset = time.time()
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
	self.support.savereport(testname)

    @skipUnlessTrue("S1002")
    def test_S1002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S1002 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        self.assertTrue(self.support.platform.udpsrv_start(1234))
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timenetsetdone = time.time()
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        timenetunset = time.time()
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
        self.assertTrue(self.support.platform.udpsrv_stop())
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
	self.support.savereport(testname)

    @skipUnlessTrue("S1003")
    def test_S1003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S1003 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        self.assertTrue(self.support.platform.udpsrv_start_echo(1234))
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timenetsetdone = time.time()
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        timenetunset = time.time()
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
        self.assertTrue(self.support.platform.udpsrv_stop())
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
	self.support.savereport(testname)

    @skipUnlessTrue("S1011")
    def test_S1011(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S1011 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        timenetsetdone = time.time()
        time.sleep(900)
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        timenetunset = time.time()
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
	self.support.savereport(testname)

    @skipUnlessTrue("S1012")
    def test_S1012(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S1012 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        self.assertTrue(self.support.platform.udpsrv_start(1234))
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        timenetsetdone = time.time()
        time.sleep(900)
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        timenetunset = time.time()
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
        self.assertTrue(self.support.platform.udpsrv_stop())
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
	self.support.savereport(testname)

    @skipUnlessTrue("S1013")
    def test_S1013(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S1013 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        self.assertTrue(self.support.platform.udpsrv_start_echo(1234))
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        timenetsetdone = time.time()
        time.sleep(900)
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        timenetunset = time.time()
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
        self.assertTrue(self.support.platform.udpsrv_stop())
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
	self.support.savereport(testname)
    
@skipUnlessTrue("mode_SmartBridgeManual")
class SmartBridgeManual(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('SMART-BRIDGE', config.channel, accept_ra=False)

    def tearDown(self):
        self.tear_down_network()
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")

    def tear_down_network(self):
        self.assertTrue( self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip), "")

@skipUnlessTrue("mode_SmartBridgeAuto")
class SmartBridgeAuto(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('SMART-BRIDGE', config.channel, accept_ra=True)

    def tearDown(self):
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)

    def set_up_network(self):
        sleep(2)
        #self.support.platform.accept_ra(self.support.brList.itf)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.start_ra(self.support.backbone.itf), "")

    def tear_down_network(self):
        self.assertTrue( self.support.stop_ra(), "")

@skipUnlessTrue("mode_Router")
class Router(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.setUp()
        self.br.set_mode('ROUTER', config.channel, iid='100', ra_daemon=True, accept_ra=False)

    def tearDown(self):
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)
        
    def set_up_network(self):
        sleep(10)
        self.assertTrue(self.support.platform.accept_ra(self.support.backbone.itf), "Could not enable RA configuration support")
        if self.support.platform.support_rio():
            self.assertTrue(self.support.platform.accept_rio(self.support.backbone.itf), "Could not enable RIO support")
        self.assertTrue(self.support.tcpdump.expect_ra(self.support.backbone.itf, 30), "")
        self.assertTrue(self.support.platform.check_prefix(self.support.backbone.itf, 'bbbb:'), "Interface not configured")
        self.support.host.ip=self.support.platform.get_address_with_prefix(self.support.backbone.itf, 'bbbb:')
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.add_route("aaaa::", gw=self.br.ip), "Could not add route")

    def tear_down_network(self):
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.rm_route("aaaa::", gw=self.br.ip), "Could not remove route")

@skipUnlessTrue("mode_RouterNoRa")
class RouterNoRa(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('ROUTER', config.channel, iid='100', ra_daemon=False)

    def tearDown(self):
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.platform.add_route("aaaa::", gw=self.br.ip), "")

    def tear_down_network(self):
        self.assertTrue( self.support.platform.rm_route("aaaa::", gw=self.br.ip), "")
        self.assertTrue( self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip), "")

@skipUnlessTrue("mode_TransparentBridgeManual")
class TransparentBridgeManual(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=False)

    def tearDown(self):
        self.tear_down_network()
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")

    def tear_down_network(self):
        self.assertTrue( self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip), "")

@skipUnlessTrue("mode_TransparentBridgeAuto")
class TransparentBridgeAuto(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=True)

    def tearDown(self):
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)

    def set_up_network(self):
        sleep(2)
        #self.support.platform.accept_ra(self.support.brList.itf)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.start_ra(self.support.backbone.itf), "")

    def tear_down_network(self):
        self.assertTrue( self.support.stop_ra(), "")

@skipUnlessTrue("mode_RplRoot")
class RplRoot(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.setUp()
        self.br.set_mode('RPL-ROOT', config.channel, iid='100', ra_daemon=True, addr_rewrite=False, filter_rpl=False)

    def tearDown(self):
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)
        
    def set_up_network(self):
        sleep(10)
        self.assertTrue(self.support.platform.accept_ra(self.support.backbone.itf), "Could not enable RA configuration support")
        if self.support.platform.support_rio():
            self.assertTrue(self.support.platform.accept_rio(self.support.backbone.itf), "Could not enable RIO support")
        self.assertTrue(self.support.tcpdump.expect_ra(self.support.backbone.itf, 30), "")
        self.assertTrue(self.support.platform.check_prefix(self.support.backbone.itf, 'bbbb:'), "Interface not configured")
        self.support.host.ip=self.support.platform.get_address_with_prefix(self.support.backbone.itf, 'bbbb:')
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.add_route("aaaa::", gw=self.br.ip), "Could not add route")

    def tear_down_network(self):
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.rm_route("aaaa::", gw=self.br.ip), "Could not remove route")

@skipUnlessTrue("mode_RplRootNoRa")
class RplRootNoRa(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('RPL-ROOT', config.channel, iid="100", ra_daemon=False, addr_rewrite=False, filter_rpl=False)

    def tearDown(self):
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.platform.add_route("aaaa::", gw=self.br.ip), "")

    def tear_down_network(self):
        self.assertTrue( self.support.platform.rm_route("aaaa::", gw=self.br.ip), "")
        self.assertTrue( self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip), "")

@skipUnlessTrue("mode_RplRootTransparentBridge")
class RplRootTransparentBridge(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.tb = self.support.add_6lbr()
        self.rpl_root = self.support.add_6lbr()
        self.support.setUp()
        self.tb.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=False, filter_rpl=False)
        self.rpl_root.set_mode('RPL-ROOT', config.channel, iid='100', ra_daemon=True, addr_rewrite=False, filter_rpl=False)

    def tearDown(self):
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)
   
    def set_up_network(self):
        sleep(10)
        self.assertTrue(self.support.platform.accept_ra(self.support.backbone.itf), "Could not enable RA configuration support")
        if self.support.platform.support_rio():
            self.assertTrue(self.support.platform.accept_rio(self.support.backbone.itf), "Could not enable RIO support")
        self.assertTrue(self.support.tcpdump.expect_ra(self.support.backbone.itf, 30), "")
        self.assertTrue(self.support.platform.check_prefix(self.support.backbone.itf, 'bbbb:'), "Interface not configured")
        self.support.host.ip=self.support.platform.get_address_with_prefix(self.support.backbone.itf, 'bbbb:')
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.add_route("aaaa::", gw=self.rpl_root.ip), "Could not add route")

    def tear_down_network(self):
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.rm_route("aaaa::", gw=self.rpl_root.ip), "Could not remove route")
        

@skipUnlessTrue("mode_MultiBrSmartBridgeAuto")
class MultiBrSmartBridgeAuto(unittest.TestCase,TestScenarios):
    def setUp(self):
        self.support=TestSupport()
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br1 = self.support.add_6lbr()
        self.br2 = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br1.set_mode('SMART-BRIDGE', config.channel, accept_ra=True)
        self.br2.set_mode('SMART-BRIDGE', config.channel, accept_ra=True)

    def tearDown(self):
        self.support.tearDown()
        self.support.savereportmode('mode_'+type(self).__name__)
        
    @skipUnlessTrue("S0")
    def test_S0(self):
        """
        Check 6LBR start-up and connectivity
        """
        testname = sys._getframe().f_code.co_name
        self.support.initreport()
        print >> sys.stderr, "******** Test S00 ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_6lbr(40, self.br1), "6LBR-1 is not responding")
        self.assertTrue(self.support.wait_ping_6lbr(40, self.br2), "6LBR-2 is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
        print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        self.support.savereport(testname)

    @skipUnlessTrue("S1_move")
    def test_S1_move(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        testname = sys._getframe().f_code.co_name
	self.support.initreport()
        print >> sys.stderr, "******** Test S01_move ********"
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(self.log_file('test_S1_move')), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        ping_thread = self.support.platform.ping_run(self.support.test_mote.ip,0.5,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        print >> sys.stderr, "Moving mote..."
	self.support.wsn.move_mote(self.support.test_mote.mote_id, -1)
	sleep(5)
        self.assertTrue(self.support.wait_ping_mote(240), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        self.support.platform.ping_stop(ping_thread)
        timenetunset = time.time()
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
	self.support.savereport(testname)
    def set_up_network(self):
        sleep(2)
        #self.support.platform.accept_ra(self.support.brList.itf)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.start_ra(self.support.backbone.itf), "")

    def tear_down_network(self):
        self.assertTrue( self.support.stop_ra(), "")

def main():
    for i in range(1,config.test_repeat+1):
        print >> sys.stderr, " ============="
        print >> sys.stderr, " == ITER %02d ==" % i
        unittest.main(exit=False, verbosity=1)
	srcdir = os.path.dirname(os.path.dirname(config.report_path))
	destdir = os.path.join(os.path.dirname(srcdir),'iter-%02d'%i)
        if os.path.exists(destdir):
            if os.path.isdir(destdir):
                rmtree(destdir)
            else:
                os.unlink(destdir)
        os.rename(srcdir,destdir)

if __name__ == '__main__':
    main()
