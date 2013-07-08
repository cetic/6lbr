import base
import config
from base import skipUnlessTrue
import sys
from time import sleep
import unittest

class MultiBrOverlapScenarios(base.TestScenarios):
    @skipUnlessTrue("S1")
    @unittest.skip("Not implemented")
    def test_S1_move_mote_overlap(self):
        pass
    
    @skipUnlessTrue("S2")
    def test_S2_br_failure(self):
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.wait_mote_start()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.start_udp_client(), "Could not start udp traffic")
        self.assertTrue(self.support.wait_mote_in_6lbr(config.mote_in_6lbr_timeout), "Mote not detected")
        self.wait_dag_stabilisation(long=True)
        self.assertTrue(self.support.wait_ping_mote(config.ping_mote_timeout), "Mote is not responding")
        print >> sys.stderr, "Killing BR..."
        self.assertTrue(self.support.brList[0].stop_6lbr(), "Could not stop 6LBR")
        self.assertTrue(self.support.wait_ping_mote(config.ping_switched_mote_timeout), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
