import config
import base
import time
import os
import sys
import urllib2

from base import skipUnlessTrue

class PerformanceSingleBrScenarios(base.TestScenarios):
    def S10xx_base(self, start_udp, wsn_udp, udp_echo):
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
            timereport.write("Mote detect start = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetectdone-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))

    def S11xx_base(self, start_udp, wsn_udp, udp_echo, global_repair = False):
        if not self.bridge_mode:
            print >> sys.stderr, "Not in bridge mode, skipping test"
            os.system("touch %s/SKIPPED" % config.test_report_path)
            return
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
        tping1 = self.support.platform.ping_run(self.support.test_mote.ip,1,config.test_report_path+'/ping1.log')
        tping2 = self.support.platform.ping_run(self.support.test_mote.ip.replace(config.wsn_prefix,config.wsn_second_prefix),1,config.test_report_path+'/ping2.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        timemotedetectdone = time.time()
        self.wait_dag_stabilisation()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        timemotepingdone = time.time()
        self.assertTrue( self.support.stop_ra(), "Could not stop RADVD")
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_stop())
            if wsn_udp:
                self.assertTrue(self.support.stop_udp_clients())
            else:
                self.assertTrue(self.support.stop_udp_client())
        self.assertTrue( self.support.platform.delete_address(self.support.backbone.itf,self.support.host.ip) )

        self.support.backbone.prefix=config.wsn_second_prefix
        self.support.host.ip = self.support.host.ip.replace(config.wsn_prefix,config.wsn_second_prefix)
        self.support.test_mote.ip = self.support.test_mote.ip.replace(config.wsn_prefix,config.wsn_second_prefix)
        self.support.brList[-1].ip=self.support.brList[-1].ip.replace(config.wsn_prefix,config.wsn_second_prefix)

        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf,self.support.host.ip) )
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            if wsn_udp:
                self.assertTrue(self.support.start_udp_clients(self.support.host.ip))
            else:
                self.assertTrue(self.support.start_udp_client(self.support.host.ip))
        self.assertTrue( self.support.start_ra(self.support.backbone), "Could not start RADVD")
        self.assertTrue( self.support.tcpdump.expect_ra(self.support.backbone.itf, 30), "")

        if global_repair:
            self.assertTrue(self.support.wait_ping_6lbr(config.ping_6lbr_timeout), "6LBR is not responding to new prefix")
            print >> sys.stderr, "RPL Global Repair... (by %s)" % self.support.host.ip
            os.environ['http_proxy']=''
            httpresponse = urllib2.urlopen("http://[%s]/rpl-gr?"%self.support.brList[-1].ip)
            self.assertTrue(httpresponse.getcode() == 200, "Fail to trigger the RPL global repair")
        timemoteping2 = time.time()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        timemoteping2done = time.time()        
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            if wsn_udp:
                self.assertTrue(self.support.stop_udp_clients())
            else:
                self.assertTrue(self.support.stop_udp_client())
            self.assertTrue(self.support.platform.udpsrv_stop())
        self.support.platform.ping_stop(tping1)
        self.support.platform.ping_stop(tping2)
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

    def S20xx_base(self, start_udp, wsn_udp, udp_echo):
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
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        timemotedetectdone = time.time()
        self.wait_dag_stabilisation()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_from_mote(config.ping_from_mote_timeout,self.support.host.ip), "Host is not responding")
        timemotepingdone = time.time()
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        timemotestopdone = time.time()
        if start_udp:
            if wsn_udp:
                self.assertTrue(self.support.stop_udp_clients())
            else:
                self.assertTrue(self.support.stop_udp_client())
            self.assertTrue(self.support.platform.udpsrv_stop())
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
            timereport.write("Mote detect start = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetectdone-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))

    @skipUnlessTrue("S1000")
    def test_S1000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(start_udp=False, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S1001")
    def test_S1001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(start_udp=True, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S1002")
    def test_S1002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(start_udp=True, wsn_udp=True, udp_echo=False)

    @skipUnlessTrue("S1003")
    def test_S1003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(start_udp=True, wsn_udp=True, udp_echo=True)

    @skipUnlessTrue("S1100")
    def test_S1100(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, with global repair.
        """
        self.S11xx_base(start_udp=False, wsn_udp=False, udp_echo=False, global_repair=True)

    @skipUnlessTrue("S1101")
    def test_S1101(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, with global repair.
        """
        self.S11xx_base(start_udp=True, wsn_udp=False, udp_echo=False, global_repair=True)

    @skipUnlessTrue("S1102")
    def test_S1102(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, with global repair.
        """
        self.S11xx_base(start_udp=True, wsn_udp=True, udp_echo=False, global_repair=True)

    @skipUnlessTrue("S1103")
    def test_S1103(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, with global repair.
        """
        self.S11xx_base(start_udp=True, wsn_udp=True, udp_echo=True, global_repair=True)
        
    @skipUnlessTrue("S1110")
    def test_S1110(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no global repair.
        """
        self.S11xx_base(start_udp=False, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S1111")
    def test_S1111(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no global repair.
        """
        self.S11xx_base(start_udp=True, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S1112")
    def test_S1112(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no global repair.
        """
        self.S11xx_base(start_udp=True, wsn_udp=True, udp_echo=False)

    @skipUnlessTrue("S1113")
    def test_S1113(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no global repair.
        """
        self.S11xx_base(start_udp=True, wsn_udp=True, udp_echo=True)

    @skipUnlessTrue("S2000")
    def test_S2000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(start_udp=False, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S2001")
    def test_S2001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(start_udp=True, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S2002")
    def test_S2002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(start_udp=True, wsn_udp=True, udp_echo=False)

    @skipUnlessTrue("S2003")
    def test_S2003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(start_udp=True, wsn_udp=True, udp_echo=True)
