#!/usr/bin/python2.7

import unittest
import config
from time import sleep

class RouterLlsec(config.scenarios, unittest.TestCase):
    def modeSetUp(self):
        config.security_layer=1
        self.support.backbone.prefix=config.eth_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.br = self.support.add_6lbr()
        self.support.setUp()
        self.br.set_mode('ROUTER', config.channel, iid='100', ra_daemon=True, accept_ra=False, default_router=self.support.backbone.create_address('1'))
        
    def set_up_network(self):
        sleep(10)
        self.assertTrue(self.support.platform.accept_ra(self.support.backbone.itf), "Could not enable RA configuration support")
        if self.support.platform.support_rio():
            self.assertTrue(self.support.platform.accept_rio(self.support.backbone.itf), "Could not enable RIO support")
        self.assertTrue(self.support.tcpdump.expect_ra(self.support.backbone.itf, 30), "")
        self.assertTrue(self.support.platform.check_prefix(self.support.backbone.itf, config.eth_prefix+':'), "Interface %s not configured" % self.support.backbone.itf)
        self.support.host.ip=self.support.platform.get_address_with_prefix(self.support.backbone.itf, config.eth_prefix+':')
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.add_route(config.wsn_prefix+"::", gw=self.br.ip), "Could not add route")

    def tear_down_network(self):
        if not self.support.platform.support_rio():
            self.support.platform.rm_route(config.wsn_prefix+"::", gw=self.br.ip)

if __name__ == '__main__':
    unittest.main(exit=False, verbosity=1)
