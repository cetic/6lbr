import base
import config
import time
import os
import sys

from base import skipUnlessTrue

class PerformanceMultiBrOverlapScenarios(base.TestScenarios):
    def S400x_base(self, start_udp, wsn_udp, udp_echo):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            if wsn_udp:
                self.assertTrue(self.support.start_udp_clients())
            else:
                self.assertTrue(self.support.start_udp_client())
        self.wait_mote_start()
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.test_report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        timemotedetectdone = time.time()
        self.wait_dag_stabilisation()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            if wsn_udp:
                self.assertTrue(self.support.stop_udp_clients())
            else:
                self.assertTrue(self.support.stop_udp_client())
            self.assertTrue(self.support.platform.udpsrv_stop())
        self.support.platform.ping_stop(tping)
        timenetunset = time.time()
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
        print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.test_report_path+'/time.log', "a") as timereport:
            timereport.write("Start Test= %f\n" % (timestart,))
            timereport.write("ms since start...\n")
            timereport.write("Network start = %f\n" % (1000*(timenetset-timestart),))
            timereport.write("Network started = %f\n" % (1000*(timenetsetdone-timestart),))
            timereport.write("Mote start = %f\n" % (1000*(timemoterun-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))

    def S502x_base(self, start_udp, wsn_udp, udp_echo):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            if wsn_udp:
                self.assertTrue(self.support.start_udp_clients())
            else:
                self.assertTrue(self.support.start_udp_client())
        self.wait_mote_start()
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.test_report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        timemotedetectdone = time.time()
        self.wait_dag_stabilisation()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        timemotepingdone = time.time()
        print >> sys.stderr, "Killing BR..."
        self.assertTrue(self.support.brList[0].stop_6lbr(), "Could not stop 6LBR")
        timemovedmoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(config.ping_switched_mote_timeout), "Mote is not responding")
        timemovedmotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            if wsn_udp:
                self.assertTrue(self.support.stop_udp_clients())
            else:
                self.assertTrue(self.support.stop_udp_client())
            self.assertTrue(self.support.platform.udpsrv_stop())
        self.support.platform.ping_stop(tping)
        timenetunset = time.time()
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
        print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.test_report_path+'/time.log', "a") as timereport:
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

    def S700x_base(self, start_udp, wsn_udp, udp_echo):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            if wsn_udp:
                self.assertTrue(self.support.start_udp_clients())
            else:
                self.assertTrue(self.support.start_udp_client())
        self.wait_mote_start()
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.test_report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        timemotedetectdone = time.time()
        self.wait_dag_stabilisation()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        timemotepingdone = time.time()
        print >> sys.stderr, "Killing BR..."
        self.assertTrue(self.support.brList[0].stop_6lbr(), "Could not stop 6LBR")
        timemovedmoteping = time.time()
        self.assertTrue(self.support.wait_ping_from_mote(config.ping_from_mote_timeout,self.support.host.ip), "Host is not responding")
        timemovedmotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            if wsn_udp:
                self.assertTrue(self.support.stop_udp_clients())
            else:
                self.assertTrue(self.support.stop_udp_client())
            self.assertTrue(self.support.platform.udpsrv_stop())
        self.support.platform.ping_stop(tping)
        timenetunset = time.time()
        self.tear_down_network()
        timenetunsetdone = time.time()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        timestop = time.time()
        print >> sys.stderr, "Test duration = %f s" % (timestop-timestart,)
        with open(config.test_report_path+'/time.log', "a") as timereport:
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

    @skipUnlessTrue("S4000")
    def test_S4000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(start_udp=False, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S4001")
    def test_S4001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(start_udp=True, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S4002")
    def test_S4002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(start_udp=True, wsn_udp=True, udp_echo=False)

    @skipUnlessTrue("S4003")
    def test_S4003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(start_udp=True, wsn_udp=True, udp_echo=True)

    @skipUnlessTrue("S5020")
    @skipUnlessTrue("start_delay")
    def test_S5020(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(start_udp=False, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S5021")
    @skipUnlessTrue("start_delay")
    def test_S5021(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(start_udp=True, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S5022")
    @skipUnlessTrue("start_delay")
    def test_S5022(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(start_udp=True, wsn_udp=True, udp_echo=False)

    @skipUnlessTrue("S5023")
    @skipUnlessTrue("start_delay")
    def test_S5023(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(start_udp=True, wsn_udp=True, udp_echo=True)

    @skipUnlessTrue("S7000")
    @skipUnlessTrue("start_delay")
    def test_S7000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S700x_base(start_udp=False, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S7001")
    @skipUnlessTrue("start_delay")
    def test_S7001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S700x_base(start_udp=True, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S7002")
    @skipUnlessTrue("start_delay")
    def test_S7002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S700x_base(start_udp=True, wsn_udp=True, udp_echo=False)

    @skipUnlessTrue("S7003")
    @skipUnlessTrue("start_delay")
    def test_S7003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S700x_base(start_udp=True, wsn_udp=True, udp_echo=True)

