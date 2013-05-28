import base
from base import skipUnlessTrue, skipUnlessFalse
from time import sleep

class NonRegressionScenarios(base.TestScenarios):
    @skipUnlessTrue("S0")
    def test_S0_ping_br(self):
        """
        Check 6LBR start-up and connectivity
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_6lbr(40), "6LBR is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S1")
    def test_S1_ping_mote(self):
        """
        Ping mote from host.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
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
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.assertTrue(self.support.ping_from_mote(self.support.host.ip, True, 60), "Host is not responding")
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
        self.support.platform.configure_if(self.support.backbone.itf, self.support.backbone.create_address('1'))
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        #if not self.host_is_router:
            #Host is the default router (thanks to the received RA)
            #So ping packet is directly forwarded by 6LBR to host
        #    self.assertTrue(self.support.tcpdump.expect_ns(self.support.backbone.itf, [int('0x'+self.support.backbone.prefix, base=16), 0, 0, 0, 0, 0, 0, 1], 30, bg=True), "")
        self.assertTrue(self.support.tcpdump.expect_ping_request(self.support.backbone.itf, "cccc::1", 30, bg=True), "")
        self.assertTrue(self.support.ping_from_mote("cccc::1"), "")
        self.assertTrue(self.support.tcpdump.check_result(), "Expected packet not received")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S4")
    def test_S4_prefixless_dag(self):
        """
        Build prefixless dag then ping host from mote.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S5")
    def test_S5_disconnect_mote(self):
        """
        Ping disconnected mote.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.assertFalse(self.support.wait_ping_mote(10), "Mote is still responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    @skipUnlessTrue("S6")
    def test_S6_restart_br(self):
        """
        Ping mote, then restart BR and try to ping mote again.
        """
        self.assertTrue(self.support.start_6lbr('_1'), "Could not start 6LBR")
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
        sleep(1)
        self.assertTrue(self.support.start_6lbr('_2'), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")


    @skipUnlessTrue("S7")
    def test_S7_switch_prefix(self):
        """
        Observe the propagation of the Prefix switching in the WSN side (when supported in the WPAN).
        """
        pass

    @skipUnlessTrue("S8")
    def test_S8_incoming_rio(self):
        """
        Observe the propagation of the RIO in the WSN side (when supported in the WPAN).
        """
        pass

    @skipUnlessTrue("S8")
    def test_S9_udp_trafic(self):
        """
        Test UDP traffic towards host.
        """
        pass
