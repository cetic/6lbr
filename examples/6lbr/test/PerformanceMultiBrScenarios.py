import base

from base import skipUnlessTrue

class PerformanceMultiBrScenarios(base.TestScenarios):
    def S400x_base(self, start_udp, wsn_udp, udp_echo, mote_start_delay = 0):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            if wsn_udp:
                self.assertTrue(self.support.start_udp_clients())
            else:
                self.assertTrue(self.support.start_udp_client())
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for DAG stabilisation" % mote_start_delay
            time.sleep(mote_start_delay)
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
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
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))

    def S500x_base(self, start_udp, wsn_udp, udp_echo, mote_start_delay = 0):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            if wsn_udp:
                self.assertTrue(self.support.start_udp_clients())
            else:
                self.assertTrue(self.support.start_udp_client())
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for DAG stabilisation" % mote_start_delay
            time.sleep(mote_start_delay)
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
            if wsn_udp:
                self.assertTrue(self.support.stop_udp_clients())
            else:
                self.assertTrue(self.support.stop_udp_client())
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

    def S502x_base(self, start_udp, wsn_udp, udp_echo, mote_start_delay = 0):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            if wsn_udp:
                self.assertTrue(self.support.start_udp_clients())
            else:
                self.assertTrue(self.support.start_udp_client())
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        tping = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for DAG stabilisation" % mote_start_delay
            time.sleep(mote_start_delay)
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        print >> sys.stderr, "Killing BR..."
        self.assertTrue(self.support.brList[0].stop_6lbr(), "Could not stop 6LBR")
        timemovedmoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(600), "Mote is not responding")
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

    def S600x_base(self, start_udp, wsn_udp, udp_echo, mote_start_delay = 0):
        timestart = time.time()
        self.assertTrue(self.support.start_6lbr(config.report_path+'/6lbr'), "Could not start 6LBR")
        timenetset = time.time()
        self.set_up_network()
        timenetsetdone = time.time()
        if start_udp:
            self.assertTrue(self.support.platform.udpsrv_start(config.udp_port,udp_echo))
            if wsn_udp:
                self.assertTrue(self.support.start_udp_clients())
            else:
                self.assertTrue(self.support.start_udp_client())
        tcap = self.support.platform.pcap_start(config.backbone_dev,os.path.join(config.report_path,'%s.pcap'%config.backbone_dev))
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for DAG stabilisation" % mote_start_delay
            time.sleep(mote_start_delay)
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        timemotepingdone = time.time()
        timemote2ping = time.time()
        second_mote_ip=self.support.wsn.create_address(config.second_mote_ip)
        self.assertTrue(self.support.wait_ping_from_mote(60,second_mote_ip), "Second mote is not responding")
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
            timereport.write("Mote2 ping = %f\n" % (1000*(timemote2ping-timestart),))
            timereport.write("Mote2 reached = %f\n" % (1000*(timemote2pingdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))
            timereport.write("Ping stat = " + str(ping_stat) + "\n")

    @skipUnlessTrue("S4000")
    @skipUnlessFalse("disjoint_dag")
    def test_S4000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S4001")
    @skipUnlessFalse("disjoint_dag")
    def test_S4001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S4002")
    @skipUnlessFalse("disjoint_dag")
    def test_S4002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S4003")
    @skipUnlessFalse("disjoint_dag")
    def test_S4003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S5000")
    @skipUnlessTrue("disjoint_dag")
    def test_S5000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S5001")
    @skipUnlessTrue("disjoint_dag")
    def test_S5001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S5002")
    @skipUnlessTrue("disjoint_dag")
    def test_S5002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S5003")
    @skipUnlessTrue("disjoint_dag")
    def test_S5003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S5020")
    @skipUnlessFalse("disjoint_dag")
    @skipUnlessTrue("start_delay")
    def test_S5020(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S5021")
    @skipUnlessFalse("disjoint_dag")
    @skipUnlessTrue("start_delay")
    def test_S5021(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S5022")
    @skipUnlessFalse("disjoint_dag")
    @skipUnlessTrue("start_delay")
    def test_S5022(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S5023")
    @skipUnlessFalse("disjoint_dag")
    @skipUnlessTrue("start_delay")
    def test_S5023(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S6000")
    @skipUnlessTrue("disjoint_dag")
    def test_S6000(self):
        """
        """
        self.S600x_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S6001")
    @skipUnlessTrue("disjoint_dag")
    def test_S6001(self):
        """
        """
        self.S600x_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S6002")
    @skipUnlessTrue("disjoint_dag")
    def test_S6002(self):
        """
        """
        self.S600x_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S6003")
    @skipUnlessTrue("disjoint_dag")
    def test_S6003(self):
        """
        """
        self.S600x_base(True, True, True, config.start_delay)
