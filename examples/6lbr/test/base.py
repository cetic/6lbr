import unittest
import sys
from os import system
from subprocess import Popen
from time import sleep
from support import *
from tcpdump import TcpDump
import time
import shutil
import urllib2
import os

try:
    import config
except ImportError:
    print "Configuration file not found, using default..."

try:
    import gen_config
    print "Importing generated config"
except ImportError:
    pass

def skipUnlessTrue(descriptor):
    if hasattr(config, descriptor):
        if getattr(config, descriptor) == 0:
          return unittest.skip("%s set to False in config.py, skipping" % descriptor)
    #Not mentioned or true, run the test
    return lambda func: func

class TestSupport:
    def __init__(self, test_name):
        self.test_name=test_name
        self.initreport()
        self.platform=config.platformClass()
        self.backbone=config.backboneClass(self.platform)
        self.wsn=config.wsnClass()
        self.tcpdump=TcpDump()
        self.host=Host(self.backbone)
        self.brList=[]
        self.test_mote=None

    def setUp(self):
        self.platform.setUp()
        self.backbone.setUp()
        self.host.setUp()
        self.wsn.setUp()
        for i, _br in enumerate(self.brList):
            print >> sys.stderr, "Setup 6LBR #%i" % i
            _br.setUp()
        self.test_mote = self.wsn.get_test_mote()

    def tearDown(self):
        for _br in self.brList:
            _br.tearDown()
        self.wsn.tearDown()
        self.host.tearDown()
        self.backbone.tearDown()
        self.platform.tearDown()
        self.savereport()

    def start_ra(self, backbone):
        return self.platform.start_ra(backbone.itf,backbone.prefix)

    def stop_ra(self):
        return self.platform.stop_ra()

    def add_6lbr(self, device=None):
        _br = config.brClass(self.backbone, self.wsn, device)
        self.brList.append(_br)
        return _br

    def start_6lbr(self, log=""):
        ret = True
        for _br in self.brList:
            ret = ret and _br.start_6lbr(log)
        return ret

    def stop_6lbr(self):
        ret = True
        for _br in self.brList:
            ret = ret and _br.stop_6lbr()
        return ret

    def start_mote(self):
        return self.test_mote.start_mote(config.channel)

    def stop_mote(self):
        return self.test_mote.stop_mote()

    def ping(self, target):
        return self.platform.ping(target)

    def wait_mote_in_6lbr(self, count):
        return True

    def wait_ping(self, count, target):
        for n in range(count):
            if (self.ping(target)):
                return True
        return False

    def ping_6lbr(self, br=None):
        if not br:
           br=self.brList[-1]
        return self.ping( br.ip )

    def wait_ping_6lbr(self, count, br=None):
        if not br:
           br=self.brList[-1]
        print >> sys.stderr, "Pinging BR %s" % br.ip
        return self.wait_ping( count, br.ip )

    def ping_mote(self):
        return self.ping( self.test_mote.ip )

    def wait_ping_mote(self, count):
        print >> sys.stderr, "Pinging mote %s" % self.test_mote.ip
        return self.wait_ping( count, self.test_mote.ip )

    def ping_from_mote(self, address, expect_reply=False, count=0):
        return self.test_mote.ping( address, expect_reply, count )

    def wait_ping_from_mote(self, count, target):
        print >> sys.stderr, "Pinging from mote %s" % self.test_mote.ip
        for n in range(count):
            if (self.ping_from_mote(target, True)):
                return True
        return False

    def start_udp_client(self, host = None, port = None):
        if host is None:
            host = self.host.ip
        if port is None:
            port = config.udp_port
        print >> sys.stderr, "Enable UDP traffic on test mote"
        ok = self.test_mote.send_cmd("udp-dest %s" % host)
        ok = ok and self.test_mote.send_cmd("udp-port %d" % port)
        ok = ok and self.test_mote.send_cmd("udp start")
        return ok

    def stop_udp_client(self):
        print >> sys.stderr, "Disable UDP traffic on test mote"
        return self.test_mote.send_cmd("udp stop")

    def start_udp_clients(self, host = None, port = None):
        if host is None:
            host = self.host.ip
        if port is None:
            port = config.udp_port
        print >> sys.stderr, "Enable UDP traffic"
        ok = self.wsn.send_cmd_all("udp-dest %s" % host)
        ok = ok and self.wsn.send_cmd_all("udp-port %d" % port)
        ok = ok and self.wsn.send_cmd_all("udp start")
        return ok

    def stop_udp_clients(self):
        print >> sys.stderr, "Disable UDP traffic"
        return self.wsn.send_cmd_all("udp stop")

    def initreport(self):
        self.test_report_path=os.path.join(config.report_path, self.test_name)
        #Trick to propagate test report path to support instances
        config.test_report_path=self.test_report_path
        if os.path.exists(self.test_report_path):
            shutil.rmtree(self.test_report_path, True)
        os.makedirs(self.test_report_path)

    def savereport(self):
        try:
            shutil.move('COOJA.log',self.test_report_path)
        except IOError:
            pass
        try:
           shutil.move('COOJA.testlog',self.test_report_path)
        except IOError:
            pass
        for filename in os.listdir("."):
                if filename.startswith("radiolog-"):
                    try:
                        shutil.move(filename,self.test_report_path)
                    except IOError:
                        pass

class TestScenarios:
    def init_config(self):
        #Class definition
        config.backboneClass=getattr(config, 'backboneClass', NativeBridgeBB)
        config.brClass=getattr(config, 'brClass', LocalNativeBR)
        config.wsnClass=getattr(config, 'wsnClass', CoojaWsn)
        config.moteClass=getattr(config, 'moteClass', LocalTelosMote)
        config.platformClass=getattr(config, 'platformClass', Linux)
        
        #Common configuration
        config.report_path=getattr(config, 'report_path', 'report')
        config.backbone_dev=getattr(config, 'backbone_dev', 'br0')
        config.backbone_dev=getattr(config, 'ethernet_dev', 'eth0')
        config.channel=getattr(config, 'channel', 26)
        config.wsn_prefix=getattr(config, 'wsn_prefix', '8888')
        config.wsn_second_prefix=getattr(config, 'wsn_second_prefix', '9999')
        config.eth_prefix=getattr(config, 'eth_prefix', 'bbbb')
        config.ping_payload=getattr(config, 'ping_payload', 8) #Default is 54
        config.udp_port=getattr(config, 'udp_port', 3000)
        config.second_mote_ip=getattr(config, 'second_mote_ip', '0212:7416:0016:1616')
        config.ping_repeat=getattr(config, 'ping_repeat', 100)

        #Cooja configuration
        config.topology_file=getattr(config, 'topology_file', 'coojagen/output/LASTFILE')
        config.simulation_path=getattr(config, 'simulation_path', None)

        #Nativr configuration
        config.slip_radio=getattr(config, 'slip_radio', [])
        config.motes = getattr(config, 'motes', [])
        config.mote_baudrate=getattr(config, 'config_baudrate', 115200)
        
        #Remote configuration
        config.ethernet_dev=getattr(config, 'ethernet_dev', 'eth0')

        #Econotag configuration
        config.econotag_br=getattr(config, 'econotag_br', [])
        config.econotag_loader=getattr(config, 'econotag_loader', '../../../cpu/mc1322x/tools/mc1322x-load.pl')
        config.econotag_bbmc=getattr(config, 'econotag_bbmc', '../../../cpu/mc1322x/tools/ftditools/bbmc')
        config.econotag_nvm_flasher=getattr(config, 'econotag_nvm_flasher', '../tools/econotag/flash.sh')
        config.econotag_flasher_delay=getattr(config, 'econotag_flasher_delay', 50000)

        #Test configuration 
        config.ping_6lbr_timeout=getattr(config,'ping_6lbr_timeout',40)
        config.mote_in_6lbr_timeout=getattr(config,'mote_in_6lbr_timeout',30)
        config.ping_mote_timeout=getattr(config,'ping_mote_timeout',60)
        config.no_ping_mote_timeout=getattr(config,'no_ping_mote_timeout',10)
        config.ping_from_mote_timeout=getattr(config,'ping_from_mote_timeout',60)
        config.external_host=getattr(config,'external_host','cccc::1')
        config.ping_moved_mote_timeout=getattr(config,'ping_moved_mote_timeout',240)
        config.ping_switched_mote_timeout=getattr(config,'ping_switched_mote_timeout',600)
        config.mote_start_delay=getattr(config,'mote_start_delay',0)
        config.dag_stabilisation_delay=getattr(config,'dag_stabilisation_delay',0)
        config.long_dag_stabilisation_delay=getattr(config,'long_dag_stabilisation_delay',60)
    
    def wait_mote_start(self):
        if config.mote_start_delay > 0:
            print >> sys.stderr, "Wait %d s before starting up mote" % config.mote_start_delay
            sleep(config.mote_start_delay)

    def wait_dag_stabilisation(self, long=False):
        delay = config.long_dag_stabilisation_delay if long else config.dag_stabilisation_delay
        if delay > 0:
            print >> sys.stderr, "Wait %d s for DAG stabilisation" % delay
            sleep(delay)

    def log_file(self, log_name):
        return "%s_%s.log" % (log_name, self.__class__.__name__)

    def print_test_name(self):
        print >> sys.stderr, "\n******** %s ********" % self.test_name

    def setUp(self):
        self.init_config()
        self.test_name=self.__class__.__name__ + '.' + self._testMethodName
        self.multi_br=False
        self.bridge_mode=False
        self.host_is_router=False
        self.print_test_name()
        self.support=TestSupport(self.test_name)
        self.modeSetUp()

    def tearDown(self):
        self.support.platform.udpsrv_stop()
        self.tear_down_network()
        self.support.tearDown()
