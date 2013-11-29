#!/usr/bin/python2.7

import unittest
import config
from time import sleep

class RplRoot6lr(config.scenarios, unittest.TestCase):
    def modeSetUp(self):
        self.support.backbone.prefix=config.eth_prefix
        self.support.wsn.prefix=config.wsn_prefix
        self.tb = self.support.add_6lbr()
        self.rpl_root = self.support.add_6lbr(device={'dev': '/dev/null', 'iid': '-'})
        self.support.setUp()
        self.tb.set_mode('6LR', config.channel, accept_ra=False)
        self.rpl_root.set_mode('RPL-ROOT', config.channel, iid='100', ra_daemon=True, addr_rewrite=False, default_router=self.support.backbone.create_address('1'))

    def set_up_network(self):
        sleep(10)
        self.assertTrue(self.support.backbone.isBridge(), "Test require full backbone")
        self.assertTrue(self.support.platform.accept_ra(self.support.backbone.itf), "Could not enable RA configuration support")
        if self.support.platform.support_rio():
            self.assertTrue(self.support.platform.accept_rio(self.support.backbone.itf), "Could not enable RIO support")
        self.assertTrue(self.support.tcpdump.expect_ra(self.support.backbone.itf, 30), "RA not received")
        self.assertTrue(self.support.platform.check_prefix(self.support.backbone.itf, config.eth_prefix+':'), "Interface not configured")
        self.support.host.ip=self.support.platform.get_address_with_prefix(self.support.backbone.itf, config.eth_prefix+':')
        if not self.support.platform.support_rio():
            self.assertTrue(self.support.platform.add_route(config.wsn_prefix+"::", gw=self.rpl_root.ip), "Could not add route")

    def tear_down_network(self):
        if not self.support.platform.support_rio():
            self.support.platform.rm_route(config.wsn_prefix+"::", gw=self.rpl_root.ip)


if __name__ == '__main__':
    unittest.main(exit=False, verbosity=1)
