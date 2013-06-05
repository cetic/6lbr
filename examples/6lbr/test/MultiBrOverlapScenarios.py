import base
from base import skipUnlessTrue
import sys
from time import sleep

class MultiBrOverlapScenarios(base.TestScenarios):
    @skipUnlessTrue("S1")
    def test_S1_move_mote_overlap(self):
        pass
    
    @skipUnlessTrue("S2")
    def test_S2_br_failure(self):
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.start_mote(), "Could not start up mote")
        self.assertTrue(self.support.start_udp_client(), "Could not start udp traffic")
        self.assertTrue(self.support.wait_mote_in_6lbr(30), "Mote not detected")
        mote_start_delay=60
        if mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s for DAG stabilisation" % mote_start_delay
            sleep(mote_start_delay)
        self.assertTrue(self.support.wait_ping_mote(60), "Mote is not responding")
        print >> sys.stderr, "Killing BR..."
        self.assertTrue(self.support.brList[0].stop_6lbr(), "Could not stop 6LBR")
        self.assertTrue(self.support.wait_ping_mote(600), "Mote is not responding")
        self.assertTrue(self.support.stop_mote(), "Could not stop mote")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")
