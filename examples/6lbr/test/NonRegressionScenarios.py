import base
import config
from base import skipUnlessTrue
from time import sleep
import unittest
import sys

class NonRegressionScenarios(base.TestScenarios):
    @skipUnlessTrue("S0")
    def test_S0_ping_br(self):
        """
        Check 6LBR start-up and connectivity
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_6lbr(config.ping_6lbr_timeout), "6LBR is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S1")
    def test_S1_ping_mote(self):
        """
        Ping mote from host.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S2")
    def test_S2_ping_host_from_mote(self):
        """
        Ping host from mote.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        self.assertTrue(self.support.wait_ping_from_mote(config.ping_from_mote_timeout, self.support.host.ip), "Host is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S3")
    def test_S3_ping_external_host_from_mote(self):
        """
        Ping external host from mote, verify that peer/default router is used.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.support.platform.configure_if(self.support.backbone.itf, self.support.backbone.create_address('1'))
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        #if not self.host_is_router:
            #Host is the default router (thanks to the received RA)
            #So ping packet is directly forwarded by 6LBR to host
        #    self.assertTrue(self.support.tcpdump.expect_ns(self.support.backbone.itf, [int('0x'+self.support.backbone.prefix, base=16), 0, 0, 0, 0, 0, 0, 1], 30, bg=True), "")
        got_ping=False
        for i in range(10):
            self.assertTrue(self.support.tcpdump.expect_ping_request(self.support.backbone.itf, config.external_host, config.ping_from_mote_timeout, bg=True), "")
            self.assertTrue(self.support.ping_from_mote(config.external_host), "")
            sleep(2)
            if self.support.tcpdump.check_result():
                got_ping=True
                break
        self.assertTrue(got_ping, "Expected packet not received")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S4")
    def test_S4_prefixless_dag(self):
        """
        Build prefixless dag then ping host from mote.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @unittest.skip("Not working on coojamote (and not really useful test")
    def test_S5_disconnect_mote(self):
        """
        Ping disconnected mote.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.assertFalse(self.support.wait_ping_mote(config.no_ping_mote_timeout), "Mote is still responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S6")
    def test_S6_restart_br(self):
        """
        Ping mote, then restart BR and try to ping mote again.
        """
        self.assertTrue(self.support.start_6lbr('_1'), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        sleep(1)
        self.assertTrue(self.support.start_6lbr('_2', keep_nvm=True), "Could not start 6LBR")
        self.set_up_network()
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")


    @skipUnlessTrue("S7")
    def test_S7_switch_prefix(self):
        """
        Observe the propagation of the Prefix switching in the WSN side (when supported in the WPAN).
        """
        if not self.bridge_mode:
            print >> sys.stderr, "Not in bridge mode, skipping test"
            return
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        self.assertTrue( self.support.stop_ra(), "Could not stop RADVD")
        self.assertTrue( self.support.platform.delete_address(self.support.backbone.itf,self.support.host.ip) )

        self.support.backbone.prefix=config.wsn_second_prefix
        self.support.host.ip = self.support.host.ip.replace(config.wsn_prefix,config.wsn_second_prefix)
        self.support.test_mote.ip = self.support.test_mote.ip.replace(config.wsn_prefix,config.wsn_second_prefix)
        self.support.brList[-1].ip=self.support.brList[-1].ip.replace(config.wsn_prefix,config.wsn_second_prefix)

        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf,self.support.host.ip) )
        self.assertTrue( self.support.start_ra(self.support.backbone), "Could not start RADVD")
        self.assertTrue( self.support.tcpdump.expect_ra(self.support.backbone.itf, 30), "")

        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S8")
    @unittest.skip("Not implemented")
    def test_S8_incoming_rio(self):
        """
        Observe the propagation of the RIO in the WSN side (when supported in the WPAN).
        """
        pass

    @skipUnlessTrue("S9")
    @unittest.skip("Not implemented")
    def test_S9_udp_trafic(self):
        """
        Test UDP traffic towards host.
        """
        pass

    @skipUnlessTrue("S10")
    def test_S10_fragmentation(self):
        """
        Ping mote from host.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        payload=config.ping_payload
        while payload <= config.ping_max_payload:
            print >> sys.stderr, "Ping payload: %d" % payload
            self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout, payload=payload), "Mote is not responding")
            payload += config.ping_payload_step
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S11")
    def test_S11_ping_mote_from_subnet(self):
        """
        Ping mote from host.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        host_ip=self.support.backbone.create_address('2')
        self.support.platform.configure_if(self.support.backbone.itf, host_ip)
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout, source=host_ip), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
