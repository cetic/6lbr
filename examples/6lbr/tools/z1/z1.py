#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, CETIC.
# Based on z1-bsl-nopic and msp430.bsl.target.telosb
# All Rights Reserved.
# Simplified BSD License (see LICENSE.txt for full text)

import time
from optparse import OptionGroup
from msp430.bsl.target import SerialBSLTarget


class Z1Target(SerialBSLTarget):
    """
    Z1 target
    
    The Z1 wireless sensor mote has an onboard PIC 12F519 to control RST and TCK signals.
           _________     __________
          | PIC12   |   | MSP430
          |         |   | ..
    DTR-->| CLK     |   | ..
    RTS-->| GP4     |   | ..
          |         |   | ..
          |         |   | .. 
          |         |   | ..
          |     GP5 |---| TCK
          |     GP2 |---| RST
          |_________|   |__________
    
    The  Z1 schematic can be downloaded from
    http://zolertia.sourceforge.net/wiki/images/8/86/Z1_RevC_pubrel.pdf
    """
    
    def __init__(self):
        SerialBSLTarget.__init__(self)

    def add_extra_options(self):
        SerialBSLTarget.add_extra_options(self)

        # by default, use 38400 baud
        if self.parser.has_option("--speed"):
            option = self.parser.get_option("--speed")
            option.default = 38400
            option.help = "change baud rate (default %s)" % option.default
            group = self.parser.get_option_group("--speed")
            self.parser.remove_option("--speed")
            group.add_option(option)
        
    def parse_extra_options(self):
        SerialBSLTarget.parse_extra_options(self)
        
    def writepicROM(self, address, data):
        ''' Writes data to @address'''
        for i in range(7,-1,-1):
            self.picROMclock((address >> i) & 0x01)
        self.picROMclock(0)
        recbuf = 0
        for i in range(7,-1,-1):
            s = ((data >> i) & 0x01)
            #print s
            if i < 1:
                r = not self.picROMclock(s, True)
            else:
                r = not self.picROMclock(s)
            recbuf = (recbuf << 1) + r

        self.picROMclock(0, True)
        #k = 1
        #while not self.serial.getCTS():
        #    pass 
        #time.sleep(0.1)
        return recbuf

    def readpicROM(self, address):
        ''' reads a byte from @address'''
        for i in range(7,-1,-1):
            self.picROMclock((address >> i) & 0x01)
        self.picROMclock(1)
        recbuf = 0
        r = 0
        for i in range(7,-1,-1):
            r = self.picROMclock(0)
            recbuf = (recbuf << 1) + r
        self.picROMclock(r)
        #time.sleep(0.1)
        return recbuf
        
    def picROMclock(self, masterout, slow = False):
        #print "setting masterout to "+str(masterout)
        self.serial.setRTS(masterout)
        self.serial.setDTR(1)
        #time.sleep(0.02)
        self.serial.setDTR(0)
        if slow:
            time.sleep(0.02)
        return self.serial.getCTS()

    def picROMfastclock(self, masterout):
        #print "setting masterout to "+str(masterout)
        self.serial.setRTS(masterout)
        self.serial.setDTR(1)
        self.serial.setDTR(0)
        time.sleep(0.02)
        return self.serial.getCTS()

    def bslResetZ1(self, invokeBSL=0):
        '''
        Applies BSL entry sequence on RST/NMI and TEST/VPP pins
        Parameters:
            invokeBSL = 1: complete sequence
            invokeBSL = 0: only RST/NMI pin accessed
            
        By now only BSL mode is accessed
        '''
        
        self.logger.info("* bslReset(invokeBSL=%s)\n" % invokeBSL)
        if invokeBSL:
            self.logger.debug("in Z1 bsl reset...\n")
            time.sleep(0.1)
            self.writepicROM(0xFF, 0xFF)
            time.sleep(0.1)
            self.logger.debug("z1 bsl reset done...\n")
        else:
            self.logger.debug("in Z1 reset...\n")
            time.sleep(0.1)
            self.writepicROM(0xFF, 0xFE)
            time.sleep(0.1)
            self.logger.debug("z1 reset done...\n")

    def start_bsl(self, prompt_for_release=False):
        """\
        Start the ROM-BSL.
        """
        self.bslResetZ1(invokeBSL=1)
        time.sleep(0.250)        # give MSP430's oscillator time to stabilize
        self.serial.flushInput() # clear buffers

    def reset(self):
        """Reset the device."""
        self.bslResetZ1(invokeBSL=0)
        self.logger.info('Reset device')


def main():
    # run the main application
    bsl_target = Z1Target()
    bsl_target.main()

if __name__ == '__main__':
    main()
