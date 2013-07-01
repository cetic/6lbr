import base
import config
import time
import os
import sys

from time import sleep
from base import skipUnlessTrue

class PerformanceMultiBrDisjointScenarios(base.TestScenarios):

    def S500x_base(self, start_udp, wsn_udp, udp_echo):
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
        print >> sys.stderr, "Moving mote..."
        self.support.wsn.move_mote(self.support.test_mote.mote_id, -1)
        sleep(5)
        timemovedmoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(config.ping_moved_mote_timeout), "Mote is not responding")
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

    def S600x_base(self, start_udp, wsn_udp, udp_echo):
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
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        timemotepingdone = time.time()
        timemote2ping = time.time()
        second_mote_ip=self.support.wsn.create_address(config.second_mote_ip)
        self.assertTrue(self.support.wait_ping_from_mote(config.ping_from_mote_timeout,second_mote_ip), "Second mote is not responding")
        timemote2pingdone = time.time()

        ping_stat=[]
        for i in range(0, config.ping_repeat):
            time_start = time.time()
            if self.support.wait_ping_from_mote(2,second_mote_ip):
                time_stop = time.time()
                delta=time_stop-time_start-1 #serial.readline() timeout is 1 sec
                if delta < 0:
                    delta += 1
                ping_stat.append(delta)
            else:
                ping_stat.append(0)
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
            timereport.write("Mote2 ping = %f\n" % (1000*(timemote2ping-timestart),))
            timereport.write("Mote2 reached = %f\n" % (1000*(timemote2pingdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
            timereport.write("Ping stat = " + str(ping_stat) + "\n")


    @skipUnlessTrue("S5000")
    def test_S5000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(start_udp=False, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S5001")
    def test_S5001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(start_udp=True, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S5002")
    def test_S5002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(start_udp=True, wsn_udp=True, udp_echo=False)

    @skipUnlessTrue("S5003")
    def test_S5003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(start_udp=True, wsn_udp=True, udp_echo=True)

    @skipUnlessTrue("S6000")
    def test_S6000(self):
        """
        """
        self.S600x_base(start_udp=False, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S6001")
    def test_S6001(self):
        """
        """
        self.S600x_base(start_udp=True, wsn_udp=False, udp_echo=False)

    @skipUnlessTrue("S6002")
    def test_S6002(self):
        """
        """
        self.S600x_base(start_udp=True, wsn_udp=True, udp_echo=False)

    @skipUnlessTrue("S6003")
    def test_S6003(self):
        """
        """
        self.S600x_base(start_udp=True, wsn_udp=True, udp_echo=True)
