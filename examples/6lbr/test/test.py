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

    def start_ra(self, itf, variant=None):
        return self.platform.start_ra(itf,variant)

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

    def wait_ping_from_mote(self, count, target):
        for n in range(count):
            if (self.ping_from_mote(target)):
                return True
        return False

    def start_udp_clients(self, host = None, port = None):
        if host is None:
            host = self.host.ip
        if port is None:
            port = config.udp_port
        print "Enable UDP traffic"
        return self.wsn.send_cmd_all("udp %s %d" % (host,port))

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

    def print_test_name(self):
        print >> sys.stderr, "\n******** %s ********" % self.testname

    def setUp(self):
        self.testname=self.__class__.__name__ + '.' + self._testMethodName
        self.multi_br=False
	self.print_test_name()
        self.support=TestSupport()
	self.support.initreport()
	self.modeSetUp()

    def tearDown(self):
	self.support.savereport(self.testname)
        self.support.platform.udpsrv_stop()
        self.tear_down_network()
        self.support.tearDown()
        #self.support.savereportmode('mode_'+self.__class__.__name__)

    @skipUnlessTrue("S0")
    def test_S0(self):
        """
        Check 6LBR start-up and connectivity
        """
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_6lbr(40), "6LBR is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
	print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)

    @skipUnlessTrue("S1")
    def test_S1(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
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

    @skipUnlessTrue("S1_move")
    def test_S1_move(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        if not self.multi_br: return
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

    @skipUnlessTrue("S2")
    def test_S2(self):
        """
        Ping from the computer to the mote when the PC does not know the BR and the BR knows
        the mote.
        """
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
        pass

    @skipUnlessTrue("S6")
    def test_S6(self):
        """
        Observe the NUDs between the computer and the BR.
        """
        pass

    @skipUnlessTrue("S7")
    def test_S7(self):
        """
        Test the Auconfiguration process of the BR in bridge mode and observe its ability to take a
        router prefix (by using the computer as a router), and deal with new RA once configured.
        """
        pass

    @skipUnlessTrue("S8")
    def test_S8(self):
        """
        Observe the propagation of the RIO in the WSN side (when supported in the WPAN).
        """
        pass

    @skipUnlessTrue("S9")
    def test_S9(self):
        """
        Test the using of the default router.
        """
        pass

    @skipUnlessTrue("S10")
    def test_S10(self):
        """
        Ping from the sensor to the computer when the sensor does not know the CBR.
        """
        pass

    @skipUnlessTrue("S11")
    def test_S11(self):
        """
        Ping from the sensor to the computer when the CBR does not know the computer.
        """
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

    def S10xx_base(self, start_udp, udp_echo, mote_start_delay = 0):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            self.assertTrue(self.support.start_udp_clients())
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for the DAG" % mote_start_delay
            time.sleep(mote_start_delay)
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_stop())
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
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
            timereport.write("Mote detect start = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetectdone-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
            
    def S20xx_base(self, start_udp, udp_echo, mote_start_delay = 0):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            self.assertTrue(self.support.start_udp_clients())
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for the DAG" % mote_start_delay
            time.sleep(mote_start_delay)
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_from_mote(60,self.support.host.ip), "Host is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_stop())
        self.support.platform.pcap_stop(tcap)
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
            timereport.write("Mote detect start = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetectdone-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))            

    @skipUnlessTrue("S501x")
    def S500x_base(self, start_udp, udp_echo, mote_start_delay = 0):
        if not self.multi_br: return
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            self.assertTrue(self.support.start_udp_clients())
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for the DAG" % mote_start_delay
            time.sleep(mote_start_delay)
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        print >> sys.stderr, "Moving mote..."
        self.support.wsn.move_mote(self.support.test_mote.mote_id, -1)
        sleep(5)
        timemovedmoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemovedmotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_stop())
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
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
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Moved mote ping = %f\n" % (1000*(timemovedmoteping-timestart),))
            timereport.write("Moved mote reached = %f\n" % (1000*(timemovedmotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))

    @skipUnlessTrue("S1001")
    def test_S1001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(False, False)

    @skipUnlessTrue("S1002")
    def test_S1002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(True, False)

    @skipUnlessTrue("S1003")
    def test_S1003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(True, True)

    @skipUnlessTrue("S1011")
    def test_S1011(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(False, False, config.S101x_start_delay)

    @skipUnlessTrue("S1012")
    def test_S1012(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(True, False, config.S101x_start_delay)

    @skipUnlessTrue("S1013")
    def test_S1013(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(True, True, config.S101x_start_delay)

    @skipUnlessTrue("S2001")
    def test_S2001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(False, False)

    @skipUnlessTrue("S2002")
    def test_S2002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(True, False)

    @skipUnlessTrue("S2003")
    def test_S2003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(True, True)

    @skipUnlessTrue("S2011")
    def test_S2011(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(False, False, config.S101x_start_delay)

    @skipUnlessTrue("S2012")
    def test_S2012(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(True, False, config.S101x_start_delay)

    @skipUnlessTrue("S2013")
    def test_S2013(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(True, True, config.S101x_start_delay)

    @skipUnlessTrue("S5001")
    def test_S5001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(False, False)

    @skipUnlessTrue("S5002")
    def test_S5002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(True, False)

    @skipUnlessTrue("S5003")
    def test_S5003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(True, True)

@skipUnlessTrue("mode_SmartBridgeManual")
class SmartBridgeManual(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('SMART-BRIDGE', config.channel, accept_ra=False)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")

    def tear_down_network(self):
        self.assertTrue( self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip), "")

@skipUnlessTrue("mode_SmartBridgeAuto")
class SmartBridgeAuto(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('SMART-BRIDGE', config.channel, accept_ra=True)

    def set_up_network(self):
        sleep(2)
        #self.support.platform.accept_ra(self.support.brList.itf)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.start_ra(self.support.backbone.itf), "Could not start RADVD")

    def tear_down_network(self):
        self.assertTrue( self.support.stop_ra(), "Could not stop RADVD")
        
    def S11xx_base(self, start_udp, udp_echo, mote_start_delay = 0):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            self.assertTrue(self.support.start_udp_clients())
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for the DAG" % mote_start_delay
            time.sleep(mote_start_delay)
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue( self.support.stop_ra(), "Could not stop RADVD")
        self.assertTrue( self.support.start_ra(self.support.backbone.itf,"alt"), "Could not start RADVD")
        timemoteping2 = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemoteping2done = time.time()        
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_stop())
        self.support.platform.ping_stop(tping)
        self.support.platform.pcap_stop(tcap)
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
            timereport.write("Mote detect start = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetectdone-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote ping2 = %f\n" % (1000*(timemoteping2-timestart),))
            timereport.write("Mote reached2 = %f\n" % (1000*(timemoteping2done-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))

    @skipUnlessTrue("S1111")
    def test_S1111(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no glabl repair.
        """
        self.S11xx_base(False, False, config.S101x_start_delay)

    @skipUnlessTrue("S1112")
    def test_S1112(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no glabl repair.
        """
        self.S11xx_base(True, False, config.S101x_start_delay)

    @skipUnlessTrue("S1113")
    def test_S1113(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no glabl repair.
        """
        self.S11xx_base(True, True, config.S101x_start_delay)

@skipUnlessTrue("mode_Router")
class Router(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.setUp()
        self.br.set_mode('ROUTER', config.channel, iid='100', ra_daemon=True, accept_ra=False)
        
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
class RouterNoRa(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('ROUTER', config.channel, iid='100', ra_daemon=False)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.platform.add_route("aaaa::", gw=self.br.ip), "")

    def tear_down_network(self):
        self.assertTrue( self.support.platform.rm_route("aaaa::", gw=self.br.ip), "")
        self.assertTrue( self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip), "")

@skipUnlessTrue("mode_TransparentBridgeManual")
class TransparentBridgeManual(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=False)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")

    def tear_down_network(self):
        self.assertTrue( self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip), "")

@skipUnlessTrue("mode_TransparentBridgeAuto")
class TransparentBridgeAuto(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=True)

    def set_up_network(self):
        sleep(2)
        #self.support.platform.accept_ra(self.support.brList.itf)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.start_ra(self.support.backbone.itf), "")

    def tear_down_network(self):
        self.assertTrue( self.support.stop_ra(), "")

@skipUnlessTrue("mode_RplRoot")
class RplRoot(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.setUp()
        self.br.set_mode('RPL-ROOT', config.channel, iid='100', ra_daemon=True, addr_rewrite=False, filter_rpl=False)

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
class RplRootNoRa(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('RPL-ROOT', config.channel, iid="100", ra_daemon=False, addr_rewrite=False, filter_rpl=False)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.platform.add_route("aaaa::", gw=self.br.ip), "")

    def tear_down_network(self):
        self.assertTrue( self.support.platform.rm_route("aaaa::", gw=self.br.ip), "")
        self.assertTrue( self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip), "")

@skipUnlessTrue("mode_RplRootTransparentBridge")
class RplRootTransparentBridge(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.tb = self.support.add_6lbr()
        self.rpl_root = self.support.add_6lbr()
        self.support.setUp()
        self.tb.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=False, filter_rpl=False)
        self.rpl_root.set_mode('RPL-ROOT', config.channel, iid='100', ra_daemon=True, addr_rewrite=False, filter_rpl=False)

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
class MultiBrSmartBridgeAuto(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.multi_br=True
        self.support.backbone.prefix='aaaa'
        self.support.wsn.prefix='aaaa'
        self.br1 = self.support.add_6lbr()
        self.br2 = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br1.set_mode('SMART-BRIDGE', config.channel, accept_ra=True)
        self.br2.set_mode('SMART-BRIDGE', config.channel, accept_ra=True)
        
    @skipUnlessTrue("S0")
    def test_S0(self):
        """
        Check 6LBR start-up and connectivity
        """
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_6lbr(40, self.br1), "6LBR-1 is not responding")
        self.assertTrue(self.support.wait_ping_6lbr(40, self.br2), "6LBR-2 is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
        print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)

    def set_up_network(self):
        sleep(2)
        #self.support.platform.accept_ra(self.support.brList.itf)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.start_ra(self.support.backbone.itf), "")

    def tear_down_network(self):
        self.assertTrue( self.support.stop_ra(), "")

@skipUnlessTrue("mode_RplRootMultiTransparentBridge")
class RplRootMultiTransparentBridge(TestScenarios, unittest.TestCase):
    def modeSetUp(self):
        self.multi_br=True
        self.support.backbone.prefix='bbbb'
        self.support.wsn.prefix='aaaa'
        self.tb1 = self.support.add_6lbr()
        self.tb2 = self.support.add_6lbr()
        self.rpl_root = self.support.add_6lbr()
        self.support.setUp()
        self.tb1.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=False, filter_rpl=False)
        self.tb2.set_mode('TRANSPARENT-BRIDGE', config.channel, accept_ra=False, filter_rpl=False)
        self.rpl_root.set_mode('RPL-ROOT', config.channel, iid='100', ra_daemon=True, addr_rewrite=False, filter_rpl=False)

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

def main():
    for i in range(1,config.test_repeat+1):
        print >> sys.stderr, " ============="
        print >> sys.stderr, " == ITER %02d ==" % i
        unittest.main(exit=False, verbosity=1)
	srcdir = os.path.dirname(config.report_path)
	destdir = os.path.join(os.path.dirname(srcdir),'iter-%02d'%i)
        if os.path.exists(destdir):
            if os.path.isdir(destdir):
                rmtree(destdir)
            else:
                os.unlink(destdir)
        os.rename(srcdir,destdir)

if __name__ == '__main__':
    main()
