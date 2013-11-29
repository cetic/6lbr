#!/usr/bin/python2.7

import unittest
import config
from time import sleep

class NdpRouter(config.scenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix=config.eth_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.host.iid='200'
        self.support.setUp()
        self.br.set_mode('NDP-ROUTER', config.channel, iid='100', ra_daemon=True, ra_router_lifetime=1800, default_router=self.support.backbone.create_address('1'))

    def set_up_network(self):
        sleep(2)
        self.assertTrue( self.support.platform.configure_if(self.support.backbone.itf, self.support.host.ip), "")
        self.assertTrue( self.support.platform.add_route(config.wsn_prefix+"::", gw=self.br.ip), "")

    def tear_down_network(self):
        self.support.platform.rm_route(config.wsn_prefix+"::", gw=self.br.ip)
        self.support.platform.unconfigure_if(self.support.backbone.itf, self.support.host.ip)

if __name__ == '__main__':
    unittest.main(exit=False, verbosity=1)
