#!/usr/bin/python2.7

import sys
from os import system
import subprocess
from multiprocessing import Process
from copy import deepcopy
import signal
from time import sleep
import time
import config
import re
import os
import serial

class Backbone:
    def __init__(self):
        self.itf=None
        self.prefix=None
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def create_address(self, iid):
        return self.prefix + '::' + iid
    def isBridge(self):
        pass

class NativeBB(Backbone):
    def __init__(self):
        Backbone.__init__(self)
        self.tapCount=0
        self.tapStem='tap%d'
    def allocate_tap(self):
        tap = self.tapStem % self.tapCount
        self.tapCount += 1
        return tap

class NativeBridgeBB(NativeBB):
    def setUp(self):
        self.itf = config.backbone_dev
        result = system("brctl addbr %s" % self.itf)
        if result != 0:
            return False
        result = system("brctl setfd %s 0" % self.itf)
        if result != 0:
            return False
        result = system("ifconfig %s up" % self.itf)
        return result == 0

    def tearDown(self):
        result = system("ifconfig %s down" % self.itf)
        if result != 0:
            return False
        result = system("brctl delbr %s" % self.itf)
        return result == 0

    def isBridge(self):
        return True

class NativeTapBB(NativeBB):
    def setUp(self):
        self.itf = 'tap0'
    def tearDown(self):
        pass
    def isBridge(self):
        return False

class BRProxy:
    def __init__(self):
        self.itf=None
        self.ip=None

    def setUp(self):
        pass
    def tearDown(self):
        pass

    def set_mode(self, mode, channel, ra_daemon=False, accept_ra=False, addr_rewrite=True, filter_rpl=True):
        pass

    def start_6lbr(self, log):
        pass

    def stop_6lbr(self):
        pass

class LocalNativeBR(BRProxy):
    def __init__(self,backbone, wsn, radio):
        BRProxy.__init__(self)
        self.process=None
        self.backbone=backbone
        self.wsn=wsn
        self.itf = backbone.allocate_tap()
        self.radio=radio

    def setUp(self):
        self.log=None
        if not self.radio:
            self.radio=self.wsn.allocate_radio_dev()
        else:
            self.radio['used']=1

    def tearDown(self):
        if ( self.process ):
            self.stop_6lbr()
        self.wsn.release_radio_dev(self.radio)

    def set_mode(self, mode, channel, iid=None, ra_daemon=False, accept_ra=False, addr_rewrite=True, filter_rpl=True):
        self.mode=mode
        if iid:
            self.ip=self.backbone.create_address(iid)
        else:
            self.ip=self.backbone.create_address(self.radio['iid'])
        if not os.path.exists("br/%s" % self.itf):
            os.makedirs("br/%s" % self.itf)
        conf = open("br/%s/test.conf" % self.itf, 'w')
        print >>conf, "MODE=%s" % mode
        print >>conf, "DEV_ETH=eth0"
        print >>conf, "DEV_TAP=%s" % self.itf
        print >>conf, "RAW_ETH=0"

        if self.backbone.isBridge():
            print >>conf, "BRIDGE=1"
            print >>conf, "CREATE_BRIDGE=0"
            print >>conf, "DEV_BRIDGE=%s" % self.backbone.itf
        else:
            print >>conf, "BRIDGE=0"

        if 'dev' in self.radio:
            print >>conf, "DEV_RADIO=%s" % self.radio['dev']
        else:
            print >>conf, "SOCK_RADIO=%s" % self.radio['socket']
            print >>conf, "SOCK_PORT=%s" % self.radio['port']

        print >>conf, "NVM=br/%s/test.dat" % self.itf
        print >>conf, "LIB_6LBR=../package/usr/lib/6lbr"
        print >>conf, "BIN_6LBR=../bin"
        print >>conf, "IFUP=../package/usr/lib/6lbr/6lbr-ifup"
        print >>conf, "IFDOWN=../package/usr/lib/6lbr/6lbr-ifdown"
        conf.close()
        params="--new --channel=%d --wsn-accept-ra=%d --eth-ra-daemon=%d --addr-rewrite=%d --filter-rpl=%d br/%s/test.dat" % (channel, accept_ra, ra_daemon, addr_rewrite, filter_rpl, self.itf)
        if iid:
            params += " --eth-ip=%s" % self.ip
        subprocess.check_output("../tools/nvm_tool " + params, shell=True)

    def start_6lbr(self, log_file):
        print >> sys.stderr, "Starting 6LBR %s (id %s)..." % (self.itf, self.radio['nodeid'])
        #self.process = Popen(args="./start_br %s -s %s -R -t %s -c %s" % (self.mode, config.radio_dev, self.itf, self.nvm_file), shell=True)
        self.log=open(log_file+'_%s.log'%self.itf, "w")
        self.process = subprocess.Popen(args=["../package/usr/bin/6lbr",  "./br/%s/test.conf"%self.itf], stdout=self.log)
        sleep(1)
        return self.process != None

    def stop_6lbr(self):
        if self.process:
            print >> sys.stderr, "Stopping 6LBR %s (id %s)..." % (self.itf, self.radio['nodeid'])
            self.process.send_signal(signal.SIGTERM)
            sleep(1)
            self.log.close()
            self.process = None
        return True

class RemoteNativeBR(BRProxy):    
    def setUp(self):
        self.itf="eth0"

    def tearDown(self):
        if ( self.process ):
            self.stop_6lbr()

    def set_mode(self, mode, channel, ra_daemon=False, accept_ra=False, addr_rewrite=True, filter_rpl=True):
        pass

    def start_6lbr(self, log):
        print >> sys.stderr, "Starting 6LBR..."
        return False

    def stop_6lbr(self):
        print >> sys.stderr, "Stopping 6LBR..."
        return False

class Wsn:
    def __init__(self):
        self.prefix=None

    def setUp(self):
        pass

    def tearDown(self):
	    pass

    def create_address(self, iid):
        return self.prefix + '::' + iid

    def get_test_mote(self):
        pass

    def send_cmd_all(self, cmd):
        ret = True
        for mote in self.motelist:
            ret = ret and mote.send_cmd(cmd)
        return ret

class CoojaWsn(Wsn):
    def __init__(self):
        Wsn.__init__(self)
        self.motelist = []
        self.slip_motes=[]
        self.test_motes=[]

    def setUp(self, simulation_path):
        print("Setting up Cooja, compiling node firmwares... %s" % simulation_path)
        nogui = '-nogui=%s' % simulation_path
	self.cooja = subprocess.Popen(['java', '-jar', '../../../tools/cooja/dist/cooja.jar', 
                                       nogui], stdout=subprocess.PIPE)
        line = self.cooja.stdout.readline()
        while 'Simulation main loop started' not in line: # Wait for simulation to start 
	    if 'serialpty;open;' in line:
                elems = line.split(";")
                newmote = VirtualTelosMote(self)
                newmote.setInfo(elems[-1].rstrip(), int(elems[-2]))
                self.motelist.append(newmote)
            line = self.cooja.stdout.readline()
        print("Cooja simulation started")

        sleep(2)
	
        for mote in self.motelist:
            mote.setUp()

        try:
            self.motelist_file = open(simulation_path[:-4] + '.motes')
            for line in self.motelist_file:
                line = line.rstrip()
                parts = line.split(';')
                nodeid = parts[0]
                if parts[1] == 'slipradio':
                    self.add_slip_mote(nodeid)
                if parts[1] == 'node_delay':
                    self.add_test_mote(nodeid)
                if len(parts) > 2:
                    #The node has some mobility data attached, parse it
                    for xy in parts[2:]:
                        xy = xy.rstrip().split(',')
                        self.get_mote(nodeid).add_mobility_point(float(xy[0]), float(xy[1]))
                        print >> sys.stderr, "Adding mobility, point %f,%f to node %s" % (float(xy[0]), float(xy[1]), nodeid)
        except IOError:
            pass #TODO

    def add_slip_mote(self, nodeid):
        hex_mote_id = "%02x" % int(nodeid)
        iid = '0212:74' + hex_mote_id + ':' + '00' + hex_mote_id + ':' + hex_mote_id + hex_mote_id
        self.slip_motes.append({'nodeid':nodeid, 'socket': 'localhost', 'port': 60000 + int(nodeid), 'iid': iid})

    def add_test_mote(self, nodeid):
        self.test_motes.append(nodeid)
    
    def allocate_radio_dev(self):
        for slip_mote in self.slip_motes:
            if 'used' not in slip_mote:
                slip_mote['used'] = 1
                return slip_mote
        raise Exception()

    def release_radio_dev(self, slip_mote):
        del slip_mote['used']

    def tearDown(self):
        print("Killing Cooja")

        try:
            self.get_test_mote().serialport.open()
            self.get_test_mote().serialport.write("\r\nkillcooja\r\n")
            self.get_test_mote().serialport.close()
            self.cooja.wait()
            time.sleep(1)
            print >> sys.stderr, "Cooja Thread Killed"
        except serial.SerialException:
            print >> sys.stderr, "Serial error, Cooja Thread already killed ?"
        system("pkill -9 java")
        time.sleep(1)
        for mote in self.motelist:
            mote.tearDown()
        self.motelist = []

    def get_test_mote(self):
        return self.get_mote(self.test_motes[-1])

    def get_mote(self, nodeid):
        for mote in self.motelist:
            if mote.mote_id == int(nodeid):
                return mote

    def move_mote_xy(self, nodeid, xpos, ypos):
        self.get_test_mote().serialport.open()
        self.get_test_mote().serialport.write("\r\nmovemote,%d,%f,%f\r\n" %(nodeid, xpos, ypos))
        self.get_test_mote().serialport.close()

    def move_mote(self, nodeid, position):
        try:
            self.move_mote_xy(nodeid, self.get_mote(nodeid).get_mobility_point(position)[0], self.get_mote(nodeid).get_mobility_point(position)[1])
        except IndexError:
            print >> sys.stderr, "Attempt to access non-existant position. Verify the interactive_mobility array in config.py"

class LocalWsn(Wsn):
    def __init__(self):
        Wsn.__init__(self)
        self.motelist = []
        self.radioDevList=deepcopy(config.slip_radio)
        self.moteDevList=deepcopy(config.motes)

    def setUp(self, simulation_path):
        mote = config.moteClass(self)
        mote.setUp()
        self.motelist.append(mote)
	
    def tearDown(self):
        for mote in self.motelist:
            mote.tearDown()
        self.motelist = []

    def allocate_radio_dev(self):
        for dev in self.radioDevList:
            if 'used' not in dev:
                dev['used']=1
                return dev
        raise Exception()

    def release_radio_dev(self, dev):
        del dev['used']

    def allocate_mote_dev(self):
        for dev in self.moteDevList:
            if 'used' not in dev:
                dev['used']=1
                return dev
        raise Exception()

    def release_mote_dev(self, dev):
        del dev['used']

    def get_test_mote(self):
        return self.motelist[-1]

class TestbedWsn(Wsn):
    def __init__(self):
        Wsn.__init__(self)
        self.motelist = []
        self.hypernode_ip = ''
    def setUp(self, hypernode_ip):
        self.hypernode_ip = hypernode_ip
        # TODO: Open connection to Hypernode
        # TODO: Import testbed configuration file
        # TODO: Create a new TestbedMote for each mote on the testbed
	
    def tearDown(self):
        for mote in self.motelist:
            mote.tearDown()
        self.motelist = []
        # TODO: Close connection to Hypernode

class MoteProxy:
    def __init__(self):
        self.ip=None

    def setUp(self):
        pass

    def tearDown(self):
        if self.is_mote_started():
            self.stop_mote()
    
    def reset_mote(self):
        pass

    def start_mote(self, channel):
        pass

    def stop_mote(self):
        pass

    def ping(self, address, expect_reply=False, count=0):
        pass

    def is_mote_started(self):
        return False

    def send_cmd(self, cmd):
        pass

class TestbedMote(MoteProxy):
    def __init__(self, wsn):
        MoteProxy.__init__(self)
        self.wsn=wsn

    def wait_until(self, text, count):
        pass
    
    def reset_mote(self):
        pass
        # TODO: Call reset_mote on the specified mote instance (testbed-reset + start6lbrapps)

    def start_mote(self, channel):
        pass
        # TODO: TestbedMote will implement start mote through a generic write mechanism to a mote's serial port

    def stop_mote(self):
        pass
        # TODO: Call stop_mote on the specified mote instance (testbed-reset)

    def ping(self, address, expect_reply=False, count=0):
        pass
        # TODO: Call ping on the specified mote instance

class LocalTelosMote(MoteProxy):
    def __init__(self, wsn):
        MoteProxy.__init__(self)
        self.wsn=wsn

    def setUp(self):
        self.config=self.wsn.allocate_mote_dev()
        self.serialport = serial.Serial(
            port=self.config['dev'],
            baudrate=config.mote_baudrate,
            parity = serial.PARITY_NONE,
            timeout = 1
        )
        self.reset_mote()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        self.ip=self.wsn.create_address(self.config['iid'])

    def tearDown(self):
        MoteProxy.tearDown(self)
        self.serialport.close()

    def wait_until(self, text, count):
        start_time = time.time()
        elapsed = 0
        while elapsed < count :
            elapsed = time.time() - start_time
            lines = self.serialport.readlines()
            #print >> sys.stderr, line
            if text in lines:
                return True
        return False

    def reset_mote(self):
        print >> sys.stderr, "Resetting mote..."
        if(self.serialport.isOpen()):
            self.serialport.close()
        system("../../../tools/sky/msp430-bsl-linux --telosb -c %s -r" % self.config['dev'])
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        return self.wait_until("Starting '6LBR Demo'\n", 8)

    def start_mote(self, channel):
        print >> sys.stderr, "Starting mote..."
        self.serialport.flushInput()
        self.serialport.flushOutput()
        self.serialport.write("\r\nrfchannel %d\r\n" % channel)
        self.serialport.write("\r\nstart6lbr\r\n")
        return self.wait_until("done\r\n", 5)

    def stop_mote(self):
        print >> sys.stderr, "Stopping mote..."
        if(self.serialport.isOpen()):
            self.serialport.close()
        system("../../../tools/sky/msp430-bsl-linux --telosb -c %s -r" % self.config['dev'])
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        return self.wait_until("Starting '6LBR Demo'\n", 8)

    def ping(self, address, expect_reply=False, count=0):
        print >> sys.stderr, "Ping %s..." % address
        self.serialport.write("\r\nping %s\r\n" % address)
        if expect_reply:
            return self.wait_until("Received an icmp6 echo reply\n", 10)
        else:
            return True

class VirtualTelosMote(MoteProxy):
    def __init__(self, wsn):
        MoteProxy.__init__(self)
        self.wsn=wsn
        self.mote_dev = None
        self.mote_id = None
        self.mobility_data = []

    def setUp(self):
        print("Mote setup %s %d" % (self.mote_dev, self.mote_id))
        self.serialport = serial.Serial(
	port=self.mote_dev,
	baudrate=config.mote_baudrate,
	parity = serial.PARITY_NONE,
	timeout = 1
	)
        self.serialport.close()
        self.reset_mote()
    
    def tearDown(self):
        MoteProxy.tearDown(self)

    def setInfo(self, mote_dev, mote_id):
        self.mote_dev = mote_dev
        self.mote_id = mote_id
        hex_mote_id = "%02x" % int(mote_id)
        self.ip = self.wsn.create_address( '0212:74' + hex_mote_id + ':' + '00' + hex_mote_id + ':' + hex_mote_id + hex_mote_id )

    def wait_until(self, text, count):
        start_time = time.time()
        elapsed = 0
        while elapsed < count :
            elapsed = time.time() - start_time
            line = self.serialport.readline()
            #print >> sys.stderr, line
            if text in line:
                return True
        return False

    def reset_mote(self):
        print >> sys.stderr, "Resetting mote..."
        return self.send_cmd("reboot", "Starting '6LBR Demo'\n", 5)

    def start_mote(self, channel):
        #TODO check if send_cmd and the receiveing mote can handle this in 1 command: \r\nrfchannel %d\r\nstart6lbr\r\n
        print >> sys.stderr, "Starting mote..."
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        self.serialport.write("\r\nrfchannel %d\r\n" % channel)
        self.serialport.write("\r\nstart6lbr\r\n")
        ret = self.wait_until("done\r\n", 5)
        self.serialport.close()
        return ret

    def stop_mote(self):
        print >> sys.stderr, "Stopping mote..."
        return self.send_cmd("reboot", "Starting '6LBR Demo'\n", 5)

    def ping(self, address, expect_reply=False, count=0):
        print >> sys.stderr, "Ping %s..." % address
        if expect_reply:
            ret = self.send_cmd("ping %s" % address, "Received an icmp6 echo reply\n", 10)
        else:
            ret = self.send_cmd("ping %s" % address)
        return ret

    def add_mobility_point(self, x, y):
        self.mobility_data.append([x,y])

    def get_mobility_point(self, index):
        return self.mobility_data[index]

    def send_cmd(self, cmd, expect=None, expect_time=0):
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        self.serialport.write("\r\n"+cmd+"\r\n")
        if expect != None:
            ret = self.wait_until(expect, expect_time)
        else:
            ret = True
        self.serialport.close()
        return ret

class InteractiveMote(MoteProxy):
    def __init__(self):
        MoteProxy.__init__(self)
        self.mote_started=False

    def setUp(self):
        self.ip="aaaa::" + config.iid_mote

    def start_mote(self,channel):
        print >> sys.stderr, "*** Press enter when mote is powered on"
        dummy = raw_input()
        self.mote_started=True
        return True

    def stop_mote(self):
        print >> sys.stderr, "*** Press enter when mote is powered off"
        dummy = raw_input()
        self.mote_started=False
        return True

    def ping(self, address, expect_reply=False, count=0):
        print >> sys.stderr, "*** Ping from mote not implemented"
        return False

    def is_mote_started(self):
        return self.mote_started
    
class Platform:
    def setUp(self):
        pass
    
    def tearDown(self):
        pass

    def configure_if(self, itf, address):
        pass

    def unconfigure_if(self, itf, address):
        pass

    def add_route(self, dest, gw=None, itf=None):
        pass

    def rm_route(self, dest, gw=None, itf=None):
        pass

    def start_ra(self, itf):
        pass

    def stop_ra(self):
        pass

    def check_prefix(self, itf, prefix):
        pass

    def get_address_with_prefix(self, itf, prefix):
        pass
#    def check_prefix(self, itf, prefix, count):
#        for n in range(count):
#            if (self.check_prefix(itf, prefix)):
#                return True
#            sleep(1)
#        return False

    def accept_ra(self, itf):
        pass

    def accept_rio(self, itf):
        pass

    def ping(self, target):
        pass

    def delete_address(self, itf, addr):
        pass

class MacOSX(Platform):
    def __init__(self):
        self.rtadvd=None

    def tearDown(self):
        if self.rtadvd:
            self.stop_ra()

    def configure_if(self, itf, address):
        result = system("ifconfig %s inet6 %s/64 add" % (itf, address))
        return result == 0

    def unconfigure_if(self, itf, address):
        if itf:
            #return system("ifconfig %s down" % itf) == 0
            system("ifconfig %s inet6 %s/64 delete" % (itf, address))
            system("ifconfig %s down" % itf)
            return True
        else:
            return True

    def add_route(self, dest, gw=None, itf=None):
        if itf:
            result = system("route add -inet6 -prefixlen 64 %s -interface %s" % (dest, itf))
        else:
            result = system("route add -inet6 -prefixlen 64 %s %s" % (dest, gw))
        return result == 0

    def rm_route(self, dest, gw=None, itf=None):
        if itf:
            result = system("route delete -inet6 -prefixlen 64 %s -interface %s" % (dest, itf))
        else:
            result = system("route delete -inet6 -prefixlen 64 %s %s" % (dest, gw))
        return result == 0

    def start_ra(self, itf):
        print >> sys.stderr, "Start RA daemon..."
        system("sysctl -w net.inet6.ip6.forwarding=1")
        system("sysctl -w net.inet6.ip6.accept_rtadv=0")
        self.rtadvd = subprocess.Popen(args="rtadvd  -f -c rtadvd.%s.conf %s" % (itf, itf), shell=True)
        return self.rtadvd != None

    def stop_ra(self):
        print >> sys.stderr, "Stop RA daemon..."
        self.rtadvd.send_signal(signal.SIGTERM)
        self.rtadvd = None
        return True

    def check_prefix(self, itf, prefix):
        result = system("ifconfig %s | grep '%s'" % (itf, prefix))
        return result == 0

    def get_address_with_prefix(self, itf, prefix):
        return subprocess.check_output("ifconfig %s | egrep -o '(%s[:0-9a-f]+)'" % (itf, prefix), shell=True)

    def accept_ra(self, itf):
        system("sysctl -w net.inet6.ip6.forwarding=0")
        system("sysctl -w net.inet6.ip6.accept_rtadv=1")
        return True

    def support_rio(self):
        return False

    def accept_rio(self, itf):
        return False

    def ping(self, target):
        print >> sys.stderr, "ping..."
        result = system("ping6 -s %d -c 1 %s > /dev/null 2>/dev/null" % (config.ping_payload, target))
        if result != 0:
            sleep(1)
        return result == 0

    def ping_run(self, target, interval, out):
        result = system("touch %s" % out)
        proc = Process(target=self.ping_loop, args=(target,interval,out))
        proc.start()
        while True:
            id = proc.pid
            if not self.threads.has_key(id):
                break;
        self.threads[id] = proc        
        return id

    def ping_stop(self, tid):
        if self.threads.has_key(tid):
            self.threads[tid].terminate()
            self.threads[tid].join(10)
            del self.threads[tid]
            return True
        else:
            return False

    def ping_loop(self, target, interval, out):
        while True:
            result = system("echo '***' >> %s" % out)
            result = system("ping6 -s %d -c 1 %s 2>&1 >> %s" % (config.ping_payload, target, out))
            time.sleep(interval)

    def pcap_start(self, itf, out):
        pass

    def pcap_stop(self, tid):
        pass

    def udpsrv_start(self, port, udp_echo):
        pass

    def udpsrv_stop(self):
        pass

class Linux(Platform):
    def __init__(self):
        self.radvd = None
        self.udpsrv = None
        self.sp_ping = None
        self.threads = {}

    def tearDown(self):
        if self.radvd:
            self.stop_ra()
        if self.udpsrv:
            self.udpsrv_stop()
        for t in self.threads:
            self.threads[t].terminate()
            if isinstance(self.threads[t],Process):
                self.threads[t].join(10)
            if isinstance(self.threads[t],subprocess.Popen):
                self.threads[t].wait()
        for t in self.threads:
            try:
                os.kill(t, signal.SIGKILL)
            except OSError, err:
                pass
        self.threads.clear()
        print >> sys.stderr, "platform teardown"
    
    def configure_if(self, itf, address):
        result = system("ip addr add %s/64 dev %s" % (address, itf))
        return result == 0

    def unconfigure_if(self, itf, address):
        if itf:
            return system("ifconfig %s down" % itf) == 0
        else:
            return True

    def add_route(self, dest, gw=None, itf=None):
        if gw:
            result = system("route -A inet6 add %s/64 gw %s" % (dest, gw))
        else:
            result = system("route -A inet6 add %s/64 %s" % (dest, itf))            
        return result == 0

    def rm_route(self, dest, gw=None, itf=None):
        if gw:
            result = system("route -A inet6 del %s/64 gw %s" % (dest, gw))
        else:
            result = system("route -A inet6 del %s/64 %s" % (dest, itf))
        return result == 0

    def start_ra(self, itf, variant=None):
        if variant is None:
            print >> sys.stderr, "Start RA daemon..."
        else:
            print >> sys.stderr, "Start RA daemon (variant=%s)..."%variant
        system("sysctl -w net.ipv6.conf.%s.forwarding=1" % itf)
        if variant is None:
            #self.radvd = subprocess.Popen(args=["radvd", "-d", "1", "-C", "radvd.%s.conf" % itf], shell=True, preexec_fn=os.setsid)
            self.radvd = subprocess.Popen(args=("radvd -d 1 -C radvd.%s.conf" % itf).split())
        else:
            #self.radvd = subprocess.Popen(args=["radvd", "-d", "1", "-C", "radvd.%s.%s.conf" % (itf,variant)], shell=True, preexec_fn=os.setsid)
            self.radvd = subprocess.Popen(args=("radvd -d 1 -C radvd.%s.%s.conf" % (itf,variant)).split())
        return self.radvd != None

    def stop_ra(self):
        if self.radvd:
            print >> sys.stderr, "Stop RA daemon..."
            try:
                #os.killpg(self.radvd.pid,signal.SIGTERM)
                self.radvd.terminate()
                self.radvd.wait()
            except OSError:
                pass
            self.radvd = None
        return True

    def check_prefix(self, itf, prefix):
        result = system("ifconfig %s | grep '%s'" % (itf, prefix))
        return result == 0

    def get_address_with_prefix(self, itf, prefix):
        return subprocess.check_output("ifconfig %s | egrep -o '(%s[:0-9a-f]+)'" % (itf, prefix), shell=True)

    def accept_ra(self, itf):
        system("sysctl -w net.ipv6.conf.%s.forwarding=0" % itf)
        system("sysctl -w net.ipv6.conf.%s.accept_ra=1" % itf)
        return True

    def support_rio(self):
        return True

    def accept_rio(self, itf):
        system("sysctl -w net.ipv6.conf.%s.accept_ra_rt_info_max_plen=64" % itf)
        return True

    def ping(self, target):
        print "ping %s" % target
        result = system("ping6 -s %d -W 1 -c 1 %s > /dev/null" % (config.ping_payload, target))
        #if result >> 8 == 2:
        sleep(1)
        return result == 0

    def ping_run(self, target, interval, out):
        result = system("touch %s" % out)
        proc = Process(target=self.ping_loop, args=(target,interval,out))
        proc.start()
        while True:
            id = proc.pid
            if not self.threads.has_key(id):
                break;
        self.threads[id] = proc        
        return id

    def ping_stop(self, tid):
        if self.threads.has_key(tid):
            self.threads[tid].terminate()
            self.threads[tid].join(10)
            del self.threads[tid]
            return True
        else:
            return False

    def ping_loop(self, target, interval, out):
        while True:
            result = system("echo '***' >> %s" % out)
            result = system("ping6 -D -s %d -W 1 -c 1 %s 2>&1 >> %s" % (config.ping_payload, target, out))
            time.sleep(interval)

    def pcap_start(self, itf, out):
        result = system("touch %s" % out)
        proc = subprocess.Popen(args="tcpdump -i %s -w %s > /dev/null 2>&1" % (itf, out), shell=True, preexec_fn=os.setsid)
        while True:
            id = proc.pid
            if not self.threads.has_key(id):
                break;
        self.threads[id] = proc        
        return id

    def pcap_stop(self, tid):
        if self.threads.has_key(tid):
            os.killpg(tid, signal.SIGTERM)
            self.threads[tid].wait()
            del self.threads[tid]
            return True
        else:
            return False

    def udpsrv_start(self, port, udp_echo):
        if udp_echo:
            print >> sys.stderr, "Start UDP echo server..."
            self.udpsrv = subprocess.Popen(args="ncat -6 -u -l -k -e /bin/cat -p %s > /dev/null 2>&1" % port, shell=True, preexec_fn=os.setsid)
        else:
            print >> sys.stderr, "Start UDP server..."
            self.udpsrv = subprocess.Popen(args="ncat -6 -u -l -k -e /bin/false -p %s > /dev/null 2>&1" % port, shell=True, preexec_fn=os.setsid)
        return self.udpsrv != None

    def udpsrv_stop(self):
        if self.udpsrv is not None:
            print >> sys.stderr, "Stop UDP server (pid=%d)..."%self.udpsrv.pid
            os.killpg(self.udpsrv.pid, signal.SIGKILL)
            self.udpsrv = None
        return True

    def delete_address(self, itf, addr):
        result = system("ip -6 addr del %s/64 dev %s" % (addr,itf))
        return result == 0

class Host:
    def __init__(self, backbone):
        self.backbone=backbone
        self.iid=None
        self.ip=None

    def setUp(self):
        if self.iid:
            self.ip = self.backbone.create_address(self.iid)
    
    def tearDown(self):
        pass

if __name__ == '__main__':
    mote=LocalTelosMote()
    mote.setUp()
    mote.start_mote()
