import base
import config
from base import skipUnlessTrue
import sys
from time import sleep

class MultiBrDisjointScenarios(base.TestScenarios):
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
    def test_S2_move_mote_disjoint(self):
        """
        Ping from the computer to the mote when the PC knows the BR but the BR does not know the
        mote.
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.start_udp_client(), "Could not start udp traffic")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation()
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        print >> sys.stderr, "Moving mote..."
        self.support.wsn.move_mote(self.support.test_mote.mote_id, -1)
        sleep(5)
        self.assertTrue(self.support.wait_ping_mote(config.ping_moved_mote_timeout), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
