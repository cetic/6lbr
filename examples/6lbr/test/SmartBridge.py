#!/usr/bin/python2.7

import unittest
import config
from time import sleep

class SmartBridge(config.scenarios, unittest.TestCase):
    def modeSetUp(self):
        self.bridge_mode=True
        self.host_is_router=True
        self.support.backbone.prefix=config.wsn_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('SMART-BRIDGE', config.channel, accept_ra=True)

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "Interface not configured")
        self.assertTrue( self.support.start_ra(self.support.backbone), "Could not start RADVD")

    def tear_down_network(self):
        self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip)
        self.support.stop_ra()

if __name__ == '__main__':
    unittest.main(exit=False, verbosity=1)
