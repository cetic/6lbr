import base

class PerformanceScenarios(base.TestScenario):
    def S10xx_base(self, start_udp, wsn_udp, udp_echo, mote_start_delay = 0):
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
            timereport.write("Mote detect start = %f\n" % (1000*(timemotedetect-timestart),))
            timereport.write("Mote detected = %f\n" % (1000*(timemotedetectdone-timestart),))
            timereport.write("Mote ping = %f\n" % (1000*(timemoteping-timestart),))
            timereport.write("Mote reached = %f\n" % (1000*(timemotepingdone-timestart),))
            timereport.write("Mote stopped = %f\n" % (1000*(timemotestopdone-timestart),))
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))

    def S11xx_base(self, start_udp, wsn_udp, udp_echo, mote_start_delay = 0, global_repair = False):
        if not self.bridge_mode:
            print >> sys.stderr, "Not in bridge mode, skipping test"
            os.system("touch %s/SKIPPED" % config.report_path)
            return
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
        tping1 = self.support.platform.ping_run(self.support.test_mote.ip,1,config.report_path+'/ping1.log')
        tping2 = self.support.platform.ping_run(self.support.test_mote.ip.replace(config.wsn_prefix,config.wsn_second_prefix),1,config.report_path+'/ping2.log')
        timemoterun = time.time()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        timemotedetect = time.time()
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        timemotedetectdone = time.time()
        timemoteping = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
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
            self.assertTrue(self.support.wait_ping_6lbr(40), "6LBR is not responding to new prefix")
            print >> sys.stderr, "RPL Global Repair... (by %s)" % self.support.host.ip
            os.environ['http_proxy']=''
            httpresponse = urllib2.urlopen("http://[%s]/rpl-gr"%self.support.brList[-1].ip)
            self.assertTrue(httpresponse.getcode() == 200, "Fail to trigger the RPL global repair")
        timemoteping2 = time.time()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
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

    def S20xx_base(self, start_udp, wsn_udp, udp_echo, mote_start_delay = 0):
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
        self.assertTrue(self.support.wait_ping_from_mote(60,self.support.host.ip), "Host is not responding")
        timemotepingdone = time.time()
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
            timereport.write("Network stop = %f\n" % (1000*(timenetunset-timestart),))
            timereport.write("Network stopped = %f\n" % (1000*(timenetunsetdone-timestart),))
            timereport.write("Stop Test = %f\n" % (1000*(timestop-timestart),))

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

    @skipUnlessTrue("S1000")
    @skipUnlessFalse("multi_br")
    def test_S1000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S1001")
    @skipUnlessFalse("multi_br")
    def test_S1001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S1002")
    @skipUnlessFalse("multi_br")
    def test_S1002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S1003")
    @skipUnlessFalse("multi_br")
    def test_S1003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S10xx_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S1100")
    @skipUnlessFalse("multi_br")
    def test_S1100(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, with global repair.
        """
        self.S11xx_base(False, False, False, config.start_delay, True)

    @skipUnlessTrue("S1101")
    @skipUnlessFalse("multi_br")
    def test_S1101(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, with global repair.
        """
        self.S11xx_base(True, False, False, config.start_delay, True)

    @skipUnlessTrue("S1102")
    @skipUnlessFalse("multi_br")
    def test_S1102(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, with global repair.
        """
        self.S11xx_base(True, True, False, config.start_delay, True)

    @skipUnlessTrue("S1103")
    @skipUnlessFalse("multi_br")
    def test_S1103(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, with global repair.
        """
        self.S11xx_base(True, True, True, config.start_delay, True)   
        
    @skipUnlessTrue("S1110")
    @skipUnlessFalse("multi_br")
    def test_S1110(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no global repair.
        """
        self.S11xx_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S1111")
    @skipUnlessFalse("multi_br")
    def test_S1111(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no global repair.
        """
        self.S11xx_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S1112")
    @skipUnlessFalse("multi_br")
    def test_S1112(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no global repair.
        """
        self.S11xx_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S1113")
    @skipUnlessFalse("multi_br")
    def test_S1113(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote. The prefix change once the mote is reachable, no global repair.
        """
        self.S11xx_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S2000")
    @skipUnlessFalse("multi_br")
    def test_S2000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S2001")
    @skipUnlessFalse("multi_br")
    def test_S2001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S2002")
    @skipUnlessFalse("multi_br")
    def test_S2002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S2003")
    @skipUnlessFalse("multi_br")
    def test_S2003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S20xx_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S4000")
    @skipUnlessTrue("multi_br")
    @skipUnlessFalse("disjoint_dag")
    def test_S4000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S4001")
    @skipUnlessTrue("multi_br")
    @skipUnlessFalse("disjoint_dag")
    def test_S4001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S4002")
    @skipUnlessTrue("multi_br")
    @skipUnlessFalse("disjoint_dag")
    def test_S4002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S4003")
    @skipUnlessTrue("multi_br")
    @skipUnlessFalse("disjoint_dag")
    def test_S4003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S400x_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S5000")
    @skipUnlessTrue("multi_br")
    @skipUnlessTrue("disjoint_dag")
    def test_S5000(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S5001")
    @skipUnlessTrue("multi_br")
    @skipUnlessTrue("disjoint_dag")
    def test_S5001(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S5002")
    @skipUnlessTrue("multi_br")
    @skipUnlessTrue("disjoint_dag")
    def test_S5002(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S5003")
    @skipUnlessTrue("multi_br")
    @skipUnlessTrue("disjoint_dag")
    def test_S5003(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S500x_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S5020")
    @skipUnlessTrue("multi_br")
    @skipUnlessFalse("disjoint_dag")
    @skipUnlessTrue("start_delay")
    def test_S5020(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S5021")
    @skipUnlessTrue("multi_br")
    @skipUnlessFalse("disjoint_dag")
    @skipUnlessTrue("start_delay")
    def test_S5021(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S5022")
    @skipUnlessTrue("multi_br")
    @skipUnlessFalse("disjoint_dag")
    @skipUnlessTrue("start_delay")
    def test_S5022(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S5023")
    @skipUnlessTrue("multi_br")
    @skipUnlessFalse("disjoint_dag")
    @skipUnlessTrue("start_delay")
    def test_S5023(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.S502x_base(True, True, True, config.start_delay)

    @skipUnlessTrue("S6000")
    @skipUnlessTrue("multi_br")
    @skipUnlessTrue("disjoint_dag")
    def test_S6000(self):
        """
        """
        self.S600x_base(False, False, False, config.start_delay)

    @skipUnlessTrue("S6001")
    @skipUnlessTrue("multi_br")
    @skipUnlessTrue("disjoint_dag")
    def test_S6001(self):
        """
        """
        self.S600x_base(True, False, False, config.start_delay)

    @skipUnlessTrue("S6002")
    @skipUnlessTrue("multi_br")
    @skipUnlessTrue("disjoint_dag")
    def test_S6002(self):
        """
        """
        self.S600x_base(True, True, False, config.start_delay)

    @skipUnlessTrue("S6003")
    @skipUnlessTrue("multi_br")
    @skipUnlessTrue("disjoint_dag")
    def test_S6003(self):
        """
        """
        self.S600x_base(True, True, True, config.start_delay)
