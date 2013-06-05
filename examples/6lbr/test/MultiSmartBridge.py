#!/usr/bin/python2.7

import unittest
import config
from time import sleep
from base import skipUnlessTrue

class MultiSmartBridge(config.scenarios, unittest.TestCase):
    def modeSetUp(self):
        self.bridge_mode=True
        self.host_is_router=True
        self.support.backbone.prefix=config.wsn_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br1 = self.support.add_6lbr()
        self.br2 = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br1.set_mode('SMART-BRIDGE', config.channel, accept_ra=True, smart_multi_br=True)
        self.br2.set_mode('SMART-BRIDGE', config.channel, accept_ra=True, smart_multi_br=True)
        
    @skipUnlessTrue("S0")
    def test_S0_ping_br(self):
        """
        Check 6LBR start-up and connectivity
        """
        self.assertTrue(self.support.start_6lbr(), "Could not start 6LBR")
        self.set_up_network()
        self.assertTrue(self.support.wait_ping_6lbr(40, self.br1), "6LBR-1 is not responding")
        self.assertTrue(self.support.wait_ping_6lbr(40, self.br2), "6LBR-2 is not responding")
        self.tear_down_network()
        self.assertTrue(self.support.stop_6lbr(), "Could not stop 6LBR")

    def set_up_network(self):
        self.assertTrue(self.support.backbone.isBridge(), "Test require full backbone")
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "Interface not configured")
        self.assertTrue( self.support.start_ra(self.support.backbone), "Could not start RADVD")

    def tear_down_network(self):
        self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip)
        self.support.stop_ra()

if __name__ == '__main__':
    unittest.main(exit=False, verbosity=1)
