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
    def __init__(self, platform):
        self.itf=None
        self.prefix=None
        self.tcap=None
        self.platform=platform
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def create_address(self, iid):
        return self.prefix + '::' + iid
    def isBridge(self):
        pass
    def isBrCreated(self):
        pass
    def if_up(self):
        self.tcap = self.platform.pcap_start(self.itf,os.path.join(config.test_report_path,'%s.pcap'%self.itf))
    def if_down(self):
        if self.tcap:
            self.platform.pcap_stop(self.tcap)
            self.tcap=None

class EthernetBB(Backbone):
    def setUp(self):
        self.itf = config.ethernet_dev
        #result = self.platform.configure_bridge(self.itf)
        self.if_up()
        return True

    def tearDown(self):
        self.if_down()
        return True #self.platform.unconfigure_bridge(self.itf)

    def isBridge(self):
        return True

    def isBrCreated(self):
        return False

class NativeBB(Backbone):
    def __init__(self, platform):
        Backbone.__init__(self, platform)
        self.tapCount=0
        self.tapStem='tap%d'
    def allocate_tap(self):
        tap = self.tapStem % self.tapCount
        self.tapCount += 1
        return tap

class NativeBridgeBB(NativeBB):
    def setUp(self):
        self.itf = config.backbone_dev
        result = self.platform.configure_bridge(self.itf)
        self.if_up()
        return result

    def tearDown(self):
        self.if_down()
        return self.platform.unconfigure_bridge(self.itf)

    def isBridge(self):
        return True
    def isBrCreated(self):
        return False

class NativeTapBB(NativeBB):
    def setUp(self):
        self.itf = 'tap0'
    def tearDown(self):
        pass
    def isBridge(self):
        return False
    def isBrCreated(self):
        return True

class BRProxy:
    def __init__(self, backbone, wsn, device):
        self.backbone=backbone
        self.wsn=wsn
        self.device=device
        self.ip=None
        self.log=None
        self.err=None

    def setUp(self):
        if not self.device:
            self.device=self.wsn.allocate_br_dev()
        else:
            self.device['used']=1
        self.stop_6lbr_forced()

    def tearDown(self):
        if ( self.is_running() ):
            self.stop_6lbr()
        self.wsn.release_br_dev(self.device)

    def is_running(self):
        pass
    
    def set_mode(self, mode, channel, iid=None, ra_daemon=False, accept_ra=False, ra_router_lifetime=0, addr_rewrite=True, smart_multi_br=False, default_router='::'):
        pass

    def start_6lbr(self, log_stem, keep_nvm=False):
        pass

    def stop_6lbr(self):
        pass

    def stop_6lbr_forced(self):
        pass

econotag_index=0

class LocalEconotagBR(BRProxy):
    def __init__(self, backbone, wsn, device):
        BRProxy.__init__(self, backbone, wsn, device)
        global econotag_index
        self.process=None
        econotag_index += 1
        self.index=econotag_index

    def is_running(self):
        return self.process != None

    def set_mode(self, mode, channel, iid=None, ra_daemon=False, accept_ra=False, ra_router_lifetime=0, addr_rewrite=True, smart_multi_br=False, default_router='::', security_layer=0):
        if mode=='ROUTER':
            self.bin='%s/cetic_6lbr_router' % config.econotag_bin
        elif mode=='SMART-BRIDGE':
            self.bin='%s/cetic_6lbr_smart_bridge' % config.econotag_bin
        else:
            raise Exception("Unsupported mode '%s'" % mode)
        if iid:
            self.ip=self.backbone.create_address(iid)
        else:
            self.ip=self.backbone.create_address(self.device['iid'])
        self.cfg_path=os.path.join(config.test_report_path, "br_%d" % self.index)
        self.nvm_file=os.path.join(self.cfg_path, "test.dat")
        if not os.path.exists(self.cfg_path):
            os.makedirs(self.cfg_path)
        net_config = "--wsn-prefix %s:: --wsn-ip %s::100 --eth-prefix %s:: --eth-ip %s::100 --security-layer=%d" % (config.wsn_prefix, config.wsn_prefix, config.eth_prefix, config.eth_prefix, config.security_layer)
        rpl_config = "--rpl-dio-int-doubling %d" % (config.dio_int_doubling)
        test_config="--channel=%d --wsn-accept-ra=%d --ra-daemon-en=%d --ra-router-lifetime=%d --addr-rewrite=%d --smart-multi-br=%d --dft-router=%s" % (channel, accept_ra, ra_daemon, ra_router_lifetime, addr_rewrite, smart_multi_br, default_router)
        params="--new %s %s %s %s" % (net_config, rpl_config, test_config, self.nvm_file)
        if iid:
            params += " --eth-ip=%s" % self.ip
        subprocess.check_output("../tools/nvm_tool " + params, shell=True)

    def start_6lbr(self, log_stem, keep_nvm=False):
        print >> sys.stderr, "Starting 6LBR %d (id %s)..." % (self.index, self.device['iid'])
        if keep_nvm:
            print >> sys.stderr, "Using existing nvm"
        elif config.econotag_nvm_flasher:
            if config.econotag_bbmc:
                subprocess.call(args=[config.econotag_nvm_flasher, '--nvm', self.nvm_file, '--board', config.econotag_board, '--dev', self.device['dev'], '--loader', config.econotag_loader, '--bbmc', config.econotag_bbmc, '--', '-b', str(config.econotag_flasher_delay), '-e'])
                sleep(2)
            else:
                print >> sys.stderr, "Press the reset button"
                subprocess.call(args=[config.econotag_nvm_flasher, '--nvm', self.nvm_file, '--board', config.econotag_board, '--dev', self.device['dev'], '--loader', config.econotag_loader, '--bbmc', 'None', '--', '-b', str(config.econotag_flasher_delay), '-e' ])
                sleep(2)
        else:
            print >> sys.stderr, "No flasher tool, using existing nvm"
        self.log=open(os.path.join(self.cfg_path, '6lbr%s.log' % log_stem), "w")
        self.err=open(os.path.join(self.cfg_path, '6lbr%s.err' % log_stem), "w")
        if config.econotag_bbmc:
            self.process = subprocess.Popen(args=[config.econotag_loader,  '-t', self.device['dev'], '-f', self.bin, '-c', "%s -l redbee-econotag reset" % config.econotag_bbmc])
            sleep(os.path.getsize(self.bin)/14400*4)
        else:
            self.process = subprocess.Popen(args=[config.econotag_loader,  '-t', self.device['dev'], '-f', self.bin], stdout=self.log, stderr=self.err)
            print >> sys.stderr, "Press the reset button and then press Enter"
            dummy = raw_input()
        sleep(1)
        return self.process != None

    def stop_6lbr(self):
        if self.process:
            print >> sys.stderr, "Stopping 6LBR %s (id %s)..." % (self.index, self.device['iid'])
            if config.econotag_bbmc:
                pass
            else:
                print >> sys.stderr, "Press the reset button and then press Enter"
                dummy = raw_input()
            self.process.terminate()
            sleep(1)
            self.log.close()
            self.err.close()
            self.process = None
        return True

class LocalNativeBR(BRProxy):
    def __init__(self, backbone, wsn, device):
        BRProxy.__init__(self, backbone, wsn, device)
        self.process=None
        self.itf = backbone.allocate_tap()

    def is_running(self):
        return self.process != None

    def set_mode(self, mode, channel, iid=None, ra_daemon=False, accept_ra=False, ra_router_lifetime=0, addr_rewrite=True, smart_multi_br=False, default_router='::', security_layer=0):
        self.mode=mode
        if iid:
            self.ip=self.backbone.create_address(iid)
        else:
            self.ip=self.backbone.create_address(self.device['iid'])
        self.cfg_path=os.path.join(config.test_report_path, "br", self.itf)
        self.cfg_file=os.path.join(self.cfg_path, "test.conf")
        self.nvm_file=os.path.join(self.cfg_path, "test.dat")
        if not os.path.exists(self.cfg_path):
            os.makedirs(self.cfg_path)
        conf = open(self.cfg_file, 'w')
        print >>conf, "MODE=%s" % mode
        print >>conf, "DEV_ETH=eth0"
        print >>conf, "DEV_TAP=%s" % self.itf
        print >>conf, "RAW_ETH=0"

        print >>conf, "ETH_JOIN_BRIDGE=0"

        if self.backbone.isBridge():
            print >>conf, "BRIDGE=1"
            print >>conf, "CREATE_BRIDGE=0"
            print >>conf, "DEV_BRIDGE=%s" % self.backbone.itf
        else:
            print >>conf, "BRIDGE=0"

        if 'dev' in self.device:
            print >>conf, "DEV_RADIO=%s" % self.device['dev']
        else:
            print >>conf, "SOCK_RADIO=%s" % self.device['socket']
            print >>conf, "SOCK_PORT=%s" % self.device['port']

        print >>conf, "NVM=%s" % self.nvm_file
        print >>conf, "LIB_6LBR=../package/usr/lib/6lbr"
        print >>conf, "ETC_6LBR=../package/etc/6lbr"
        print >>conf, "BIN_6LBR=../bin"
        print >>conf, "IFUP=../package/usr/lib/6lbr/6lbr-ifup"
        print >>conf, "IFDOWN=../package/usr/lib/6lbr/6lbr-ifdown"
        print >>conf, "LOG_LEVEL=12" #All level
        print >>conf, "LOG_SERVICES=ffffffff" #All services
        print >>conf, "LOG_6LBR_OUT=-"
        print >>conf, "LOG_6LBR_ERR=-"
        conf.close()
        net_config = "--wsn-prefix %s:: --wsn-ip %s::100 --eth-prefix %s:: --eth-ip %s::100  --security-layer=%d" % (config.wsn_prefix, config.wsn_prefix, config.eth_prefix, config.eth_prefix, config.security_layer)
        rpl_config = "--rpl-dio-int-doubling %d" % (config.dio_int_doubling)
        test_config="--channel=%d --wsn-accept-ra=%d --ra-daemon-en=%d --ra-router-lifetime=%d --addr-rewrite=%d --smart-multi-br=%d --dft-router=%s" % (channel, accept_ra, ra_daemon, ra_router_lifetime, addr_rewrite, smart_multi_br, default_router)
        params="--new %s %s %s %s" % (net_config, rpl_config, test_config, self.nvm_file)
        if iid:
            params += " --eth-ip=%s" % self.ip
        subprocess.check_output("../tools/nvm_tool " + params, shell=True)

    def start_6lbr(self, log_stem="", keep_nvm=False):
        print >> sys.stderr, "Starting 6LBR %s (id %s)..." % (self.itf, self.device['iid'])
        self.log=open(os.path.join(self.cfg_path, '6lbr%s.log' % log_stem), "w")
        self.err=open(os.path.join(self.cfg_path, '6lbr%s.err' % log_stem), "w")
        self.process = subprocess.Popen(args=["../package/usr/bin/6lbr",  self.cfg_file], stdout=self.log, stderr=self.err)
        sleep(1)
        if self.backbone.isBrCreated():
            self.backbone.if_up()
        return self.process != None

    def stop_6lbr(self):
        if self.process:
            if self.backbone.isBrCreated():
                #Give some time to write packets to file
                sleep(1)
                self.backbone.if_down()
            print >> sys.stderr, "Stopping 6LBR %s (id %s)..." % (self.itf, self.device['iid'])
            self.process.terminate()
            sleep(1)
            self.log.close()
            self.err.close()
            self.process = None
        return True

remote_br_index=0

class RemoteNativeBR(BRProxy):    
    def __init__(self, backbone, wsn, device):
        BRProxy.__init__(self, backbone, wsn, device)
        global remote_br_index
        remote_br_index += 1
        self.index=remote_br_index
        self.running=False

    def is_running(self):
        return self.running

    def set_mode(self, mode, channel, iid=None, ra_daemon=False, accept_ra=False, ra_router_lifetime=0, addr_rewrite=True, smart_multi_br=False, default_router='::', security_layer=0):
        self.mode=mode
        if iid:
            self.ip=self.backbone.create_address(iid)
        else:
            self.ip=self.backbone.create_address(self.device['iid'])
        self.cfg_path=os.path.join(config.test_report_path, "br_%d" % self.index)
        self.cfg_file=os.path.join(self.cfg_path, "test.conf")
        self.nvm_file=os.path.join(self.cfg_path, "test.dat")
        if not os.path.exists(self.cfg_path):
            os.makedirs(self.cfg_path)
        conf = open(self.cfg_file, 'w')
        print >>conf, "MODE=%s" % mode
        print >>conf, "DEV_ETH=eth0"
        print >>conf, "DEV_TAP=tap0"
        print >>conf, "RAW_ETH=0"
        print >>conf, "BRIDGE=1"
        print >>conf, "CREATE_BRIDGE=0"
        print >>conf, "DEV_BRIDGE=br0"
        print >>conf, "DEV_RADIO=%s" % self.device['dev']
        print >>conf, "BAUDRATE=%s" % self.device['baudrate']
        print >>conf, "LOG_LEVEL=3" #INFO and above only
        print >>conf, "LOG_SERVICES=ffffffff" #All services
        conf.close()
        net_config = "--wsn-prefix %s:: --wsn-ip %s::100 --eth-prefix %s:: --eth-ip %s::100  --security-layer=%d" % (config.wsn_prefix, config.wsn_prefix, config.eth_prefix, config.eth_prefix, config.security_layer)
        rpl_config = "--rpl-dio-int-doubling %d" % (config.dio_int_doubling)
        test_config="--channel=%d --wsn-accept-ra=%d --ra-daemon-en=%d --ra-router-lifetime=%d --addr-rewrite=%d --smart-multi-br=%d --dft-router=%s" % (channel, accept_ra, ra_daemon, ra_router_lifetime, addr_rewrite, smart_multi_br, default_router)
        params="--new %s %s %s %s" % (net_config, rpl_config, test_config, self.nvm_file)
        if iid:
            params += " --eth-ip=%s" % self.ip
        subprocess.check_output("../tools/nvm_tool " + params, shell=True)

    def remote_cmd(self, cmd):
        result = system("ssh -i %s -o NumberOfPasswordPrompts=0 %s@%s sudo '%s'" % (config.id_file, config.remote_user, self.device['ctrl_ip'], cmd))
        return result == 0

    def send_file(self, local_file, remote_file):
        result = system("scp -i %s -o NumberOfPasswordPrompts=0 %s %s@%s:%s" % (config.id_file, local_file, config.remote_user, self.device['ctrl_ip'], remote_file))
        return result == 0

    def retrieve_file(self, remote_file, local_file):
        result = system("scp -i %s -o NumberOfPasswordPrompts=0 %s@%s:%s %s" % (config.id_file, config.remote_user, self.device['ctrl_ip'], remote_file, local_file))
        return result == 0

    def start_6lbr(self, log_stem="", keep_nvm=False):
        self.remote_cmd("/etc/init.d/6lbr stop")
        self.remote_cmd("rm /var/log/6lbr*")
        print >> sys.stderr, "Starting 6LBR..."
        ret = self.send_file(self.cfg_file, '/etc/6lbr/6lbr.conf')
        if not keep_nvm:
            ret = ret and self.send_file(self.nvm_file, '/etc/6lbr/nvm.dat')
        ret = ret and self.remote_cmd("/etc/init.d/6lbr start")
        self.running=ret
        return ret

    def stop_6lbr(self):
        print >> sys.stderr, "Stopping 6LBR..."
        ret = self.remote_cmd("/etc/init.d/6lbr stop")
        print "Retrieve log file..."
        ret = ret and self.retrieve_file('/var/log/6lbr.log', os.path.join(self.cfg_path, '6lbr.log'))
        print "Retrieve err file..."
        ret = ret and self.retrieve_file('/var/log/6lbr.err', os.path.join(self.cfg_path, '6lbr.err'))
        self.running=False
        return ret

    def stop_6lbr_forced(self):
        print >> sys.stderr, "Stopping 6LBR..."
        ret = self.remote_cmd("/etc/init.d/6lbr stop")
        return ret

class Wsn:
    def __init__(self):
        self.prefix=None

    def setUp(self):
        pass

    def tearDown(self):
	    pass

    def reset(self):
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
        self.err=None

    def setUp(self):
        if config.simulation_path:
            simulation_path=config.simulation_path
        else:
            try:
                topology_file = open(config.topology_file, 'r')
                simulation_path = topology_file.readline().rstrip()
                topology_file.close()
            except IOError:
                print "Could not open %s topology file" % config.topology_file
                raise

        print >> sys.stderr, "Setting up Cooja, compiling node firmwares... %s" % simulation_path
        nogui = '-nogui=%s' % simulation_path
        self.err=open(os.path.join(config.test_report_path, 'COOJA.err'), "w")
        self.cooja = subprocess.Popen(['java', '-jar', '../../../tools/cooja/dist/cooja.jar', 
                                       nogui], stdout=subprocess.PIPE, stderr=self.err)
        line = self.cooja.stdout.readline()
        while 'Opened pcap file' not in line: # Wait for simulation to start
            if 'serialpty;open;' in line:
                elems = line.split(";")
                newmote = VirtualTelosMote(self)
                newmote.setId(elems[-1].rstrip(), int(elems[-2]))
                self.motelist.append(newmote)
            line = self.cooja.stdout.readline()
        print >> sys.stderr, "Cooja simulation started"
        self.cooja.stdout.close()

        sleep(2)
	
        for mote in self.motelist:
            mote.setUp()

        try:
            self.motelist_file = open(simulation_path[:-4] + '.motes')
            for line in self.motelist_file:
                line = line.rstrip()
                parts = line.split(';')
                nodeid = parts[0]
                mote=self.get_mote(nodeid)
                if parts[1] == 'slipradio':
                    self.add_slip_mote(nodeid, parts[2])
                if parts[1] == 'node_delay':
                    self.add_test_mote(nodeid)
                if mote is not None:
                    mote.setTarget(parts[2])
                if len(parts) > 3:
                    #The node has some mobility data attached, parse it
                    for xy in parts[3:]:
                        xy = xy.rstrip().split(',')
                        self.get_mote(nodeid).add_mobility_point(float(xy[0]), float(xy[1]))
                        print >> sys.stderr, "Adding mobility, point %f,%f to node %s" % (float(xy[0]), float(xy[1]), nodeid)
        except IOError:
            pass #TODO
        if self.get_test_mote() == None:
            print >> sys.stderr, "Can not find test mote"
            raise Exception()

    def tearDown(self):
        if self.cooja:
            print >> sys.stderr, "Killing Cooja"
            try:
                self.get_test_mote().serialport.open()
                self.get_test_mote().serialport.write("\r\nkillcooja\r\n")
                self.get_test_mote().serialport.close()
                for mote in self.motelist:
                    mote.tearDown()
                self.cooja.wait()
                time.sleep(1)
                print >> sys.stderr, "Cooja Thread Killed"
            except serial.SerialException:
                print >> sys.stderr, "Serial error, Cooja Thread already killed ?"
        self.cooja=None
        self.err.close()
        self.motelist = []

    def add_slip_mote(self, nodeid, target):
        hex_mote_id = "%02x" % int(nodeid)
        if target == 'sky':
            iid = '0212:74' + hex_mote_id + ':' + '00' + hex_mote_id + ':' + hex_mote_id + hex_mote_id
        else:
            iid = '02' + hex_mote_id +':' + hex_mote_id + ':' + hex_mote_id + ':' + hex_mote_id
        self.slip_motes.append({'nodeid':nodeid, 'socket': 'localhost', 'port': 60000 + int(nodeid), 'iid': iid})

    def add_test_mote(self, nodeid):
        self.test_motes.append(nodeid)
    
    def allocate_br_dev(self):
        for slip_mote in self.slip_motes:
            if 'used' not in slip_mote:
                slip_mote['used'] = 1
                return slip_mote
        raise Exception()

    def release_br_dev(self, slip_mote):
        del slip_mote['used']

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
        self.brDevList=[]
        #TODO: add device type
        self.brDevList+=deepcopy(config.slip_radio)
        self.brDevList+=deepcopy(config.econotag_br)
        self.brDevList+=deepcopy(config.remote_br)
        self.moteDevList=deepcopy(config.motes)

    def setUp(self):
        mote = config.moteClass(self)
        mote.setUp()
        self.motelist.append(mote)
	
    def tearDown(self):
        for mote in self.motelist:
            mote.tearDown()
        self.motelist = []

    def allocate_br_dev(self):
        for dev in self.brDevList:
            if 'used' not in dev:
                dev['used']=1
                return dev
        raise Exception()

    def release_br_dev(self, dev):
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
        import fabric.api
        self.motelist = []
        self.supernode_hostname = config.supernode
        self.brDevList=[]
        self.brDevList+=deepcopy(config.slip_radio)
        self.brDevList+=deepcopy(config.econotag_br)
        self.brDevList+=deepcopy(config.remote_br)
        self.testmote_label=config.testmote['label']

    def setUp(self):
        print >> sys.stderr, "Discovering Testbed Motes..."
        motelist = self.tb_list(mote_type='sky', verbose=True)
        motelist += '\r\n' + self.tb_list(mote_type='z1', verbose=True)
        for elem in motelist.split('\n'):
            (label, serial, dev, name) = elem.split('\t')
            type=""
            if label[0] == "T":
                type='sky'
            elif label[0] == "Z":
                type='z1'
            if label == self.testmote_label:
                print >> sys.stderr, "Found testmote %s %s" % (label, dev)
                mote = config.moteClass(self, label, dev, type, config.testmote['iid'])
            else:
                mote = config.moteClass(self, label, dev, type)
            self.motelist.append(mote)
            mote.setUp()

        #We could re-use this if we implement a verbose mode:
        #for mote in self.motelist:
        #    print >> sys.stderr, ("mote", mote.dev)

    def reset(self):
        #reset the whole testbed, twice, to avoid errors. 
        #for simplicity, we do not use prog for now, it is pre-flashed
        print >> sys.stderr, "Resetting the Testbed..."
        self.tb_reset('sky')
        self.tb_reset('sky')
        self.tb_reset('z1')
        self.tb_reset('z1')
        #In testbed mode we use the 6lbr-demo-delay mode for ALL motes, because we want to avoid the first wave of reset motes to
        #Join the old DODAG still running on the other motes. So now we need to startup all the motes except the real delay motes

        for mote in self.motelist:
            if mote.label != self.testmote_label:
                mote.start_mote(config.channel, False)

    def tearDown(self):
        for mote in self.motelist:
            mote.tearDown()
        self.motelist = []

    def allocate_br_dev(self):
        for dev in self.brDevList:
            if 'used' not in dev:
                dev['used']=1
                return dev
        raise Exception()

    def release_br_dev(self, dev):
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
        for mote in self.motelist:
            if mote.label == self.testmote_label:
                return mote
        return None

    def supernode_cmd(self, cmd):
        with fabric.api.settings(host_string=self.supernode_hostname, user='root', use_ssh_config=True, hide=True), fabric.api.hide('output'), fabric.api.hide('running'):
            output = fabric.api.run(cmd)
            #print >> sys.stderr, output
            return output

    def tb_prog(self, binfile, mote_devs, mote_type):
        #TODO: Handle error: mote id does not exist
        #TODO: Handle prog errors
        system('scp -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no /home/sd/git/sixlbr/examples/6lbr-demo/%s root@%s:/root/bin' % (binfile, self.supernode_hostname))
        self.supernode_cmd('tb_action -a prog -t %s -f /root/bin/%s -d %s' % (mote_type, binfile, ','.join(mote_devs)))

    def tb_reset(self, mote_type=None, mote_devs=None):
        if mote_type == None:
            self.supernode_cmd('tb_action -a reset')
        else:
            if mote_devs == None:
                self.supernode_cmd('tb_action -a reset -t %s' % (mote_type))
            else:
                self.supernode_cmd('tb_action -a reset -t %s -d %s' % (mote_type, ','.join(mote_devs)))

    def tb_erase(self, mote_type, mote_devs=None):
        if mote_devs == None:
            mote_devs = 'all'
        self.supernode_cmd('tb_action -a erase -t %s -d %s' % (mote_type, mote_devs))

    def tb_list(self, mote_type='sky', verbose=False):
        if verbose:
            return self.supernode_cmd('tb_list -t %s' % (mote_type))
        else:
            return self.supernode_cmd('tb_list -t %s -s' % (mote_type))

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

    def start_mote(self, channel, wait_confirm):
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
    def __init__(self, wsn, label, dev, type, iid=None):
        MoteProxy.__init__(self)
        self.wsn=wsn
        self.label=label
        self.dev=dev
        self.type=type
        self.baudrate=115200 #TODO hardcoded
        if iid != None:
            self.ip=self.wsn.create_address(iid)

    def wait_until(self, text, count):
        pass

    def reset_mote(self):
        self.wsn.tb_reset(self.type, [self.dev,])
    
    def start_mote(self, channel, wait_confirm=True):
        print >> sys.stderr, "Set channel %d on %s mote, device %s" % (channel, self.type, self.dev)
        self.send_cmd('rfchannel %d' % channel)
        print >> sys.stderr, "Start %s mote, device %s" % (self.type, self.dev)
        if wait_confirm:
            return self.send_cmd('start6lbr', 'done', 15)       
        else:
            return self.send_cmd('start6lbr')

    def send_cmd(self, cmd, expect=None, expect_time=0):
        if expect == None:
            self.wsn.supernode_cmd('tb_serial_cmd -c "%s" -b %s -d %s' % (cmd, self.baudrate, self.dev))
        else:
            self.wsn.supernode_cmd('tb_serial_cmd -c "%s" -b %s -d %s -r %s -t %d' % (cmd, self.baudrate, self.dev, expect, int(expect_time)))
        return True #TODO: check if supernode_cmd functions and return True/False. Supernode cmd returns the output string, not True/False

    def stop_mote(self):
        print >> sys.stderr, "Stopping mote..."
        self.wsn.tb_reset('sky', [self.dev,])
        #return self.send_cmd("reboot", "Starting", 5)
        return True

    def ping(self, address, expect_reply=False, count=0):
        print "Ping %s..." % address
        return self.send_cmd('"ping %s"' % address, '"Received an icmp6 echo reply\n"', 50)

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
        self.ip=self.wsn.create_address(self.config['iid'])

    def tearDown(self):
        MoteProxy.tearDown(self)

    def wait_until(self, text, count):
        start_time = time.time()
        elapsed = 0
        while elapsed < count :
            elapsed = time.time() - start_time
            lines = self.serialport.readlines()
            print >> sys.stderr, lines
            if text in lines:
                return True
        return False

    def reset_mote(self):
        print >> sys.stderr, "Resetting mote..."
        system("../../../tools/sky/msp430-bsl-linux --telosb -c %s -r" % self.config['dev'])
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        result=self.wait_until("Starting '6LBR Demo'\n", 15)
        self.serialport.close()
        return result

    def start_mote(self, channel, wait_confirm=True):
        print >> sys.stderr, "Starting mote..."
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        self.serialport.write("\r\nrfchannel %d\r\n" % channel)
        self.serialport.write("\r\nstart6lbr\r\n")
        result=self.wait_until("done\r\n", 15)
        self.serialport.close()
        return result

    def stop_mote(self):
        print >> sys.stderr, "Stopping mote..."
        if(self.serialport.isOpen()):
            self.serialport.close()
        system("../../../tools/sky/msp430-bsl-linux --telosb -c %s -r" % self.config['dev'])
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        result=self.wait_until("Starting '6LBR Demo'\n", 15)
        self.serialport.close()
        return result

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

    def ping(self, address, expect_reply=False, count=0):
        #TODO: should use send_cmd()
        print "Ping %s..." % address
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        self.serialport.write("\r\nping %s\r\n" % address)
        if expect_reply:
            ret = self.wait_until("Received an icmp6 echo reply\n", 10)
        else:
            ret = True
        self.serialport.close()
        return ret

class VirtualTelosMote(MoteProxy):
    def __init__(self, wsn):
        MoteProxy.__init__(self)
        self.wsn=wsn
        self.mote_dev = None
        self.mote_id = None
        self.mobility_data = []

    def setUp(self):
        #print >> sys.stderr, "Mote setup %s %d" % (self.mote_dev, self.mote_id)
        self.serialport = serial.Serial(
            port=self.mote_dev,
	        baudrate=config.mote_baudrate,
	        parity = serial.PARITY_NONE,
	        timeout = 1)
        self.serialport.close()
        #self.reset_mote()
    
    def tearDown(self):
        MoteProxy.tearDown(self)
        self.serialport=None

    def setId(self, mote_dev, mote_id):
        self.mote_dev = mote_dev
        self.mote_id = mote_id

    def setTarget(self, target):
        self.target = target
        hex_mote_id = "%02x" % int(self.mote_id)
        if self.target == 'sky':
            self.ip = self.wsn.create_address( '0212:74' + hex_mote_id + ':' + '00' + hex_mote_id + ':' + hex_mote_id + hex_mote_id )
        else:
            self.ip = self.wsn.create_address( '02' + hex_mote_id +':' + hex_mote_id + ':' + hex_mote_id + ':' + hex_mote_id )

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
        print >> sys.stderr, "Resetting mote %d..." % self.mote_id
        return self.send_cmd("reboot", "Starting '6LBR Demo'\n", 5)

    def start_mote(self, channel, wait_confirm=True):
        #TODO check if send_cmd and the receiveing mote can handle this in 1 command: \r\nrfchannel %d\r\nstart6lbr\r\n
        print >> sys.stderr, "Starting mote %d..." % self.mote_id
        self.serialport.open()
        self.serialport.flushInput()
        self.serialport.flushOutput()
        self.serialport.write("\r\nrtimestamp %f\r\n" % time.time())
        self.serialport.write("\r\nrfchannel %d\r\n" % channel)
        self.serialport.write("\r\nstart6lbr\r\n")
        ret = self.wait_until("done\r\n", 5)
        self.serialport.close()
        return ret

    def stop_mote(self):
        #print >> sys.stderr, "Stopping mote %d..." % self.mote_id
        #return self.send_cmd("reboot", "Starting '6LBR Demo'\n", 5)
        return True

    def ping(self, address, expect_reply=False, count=0):
        print "Ping %s..." % address
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

    def start_mote(self,channel, wait_confirm=True):
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

    def configure_bridge(self, itf):
        pass
    
    def unconfigure_bridge(self, itf):
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
            system("ifconfig %s inet6 %s/64 delete" % (itf, address))
            return True
        else:
            return True

    def delete_address(self, itf, addr):
        result = system("ifconfig %s inet6 %s/64 delete" % (itf, addr))
        return result == 0

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

    def start_ra(self, itf, prefix):
        print >> sys.stderr, "Start RA daemon (%s)..." % prefix
        system("sysctl -w net.inet6.ip6.forwarding=1")
        system("sysctl -w net.inet6.ip6.accept_rtadv=0")
        self.rtadvd = subprocess.Popen(args="rtadvd  -f -c rtadvd/rtadvd.%s.%s.conf %s" % (itf, prefix, itf), shell=True)
        return self.rtadvd != None

    def stop_ra(self):
        if self.rtadvd:
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

    def ping(self, target, payload=None, source=None):
        #print >> sys.stderr, "ping..."
        if payload is None: payload=config.ping_payload
        result = system("ping6 -s %d -c 1 %s > /dev/null 2>/dev/null" % (payload, target))
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
        result = system("touch %s" % out)
        proc = subprocess.Popen(args="tcpdump -i %s -w %s > /dev/null 2>&1" % (itf, out), shell=True, preexec_fn=os.setsid)
        return proc

    def pcap_stop(self, proc):
        if proc:
            proc.terminate()

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
        self.local_host = None

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
        if self.local_host:
            self.remove_host()
        print >> sys.stderr, "platform teardown"
    
    def configure_if(self, itf, address):
        result = system("ip addr add %s/64 dev %s" % (address, itf))
        return result == 0

    def unconfigure_if(self, itf, address):
        if itf and self.check_prefix(itf, address):
            result = system("ip addr del %s/64 dev %s" % (address, itf)) == 0
            return result
        return True

    def configure_bridge(self, itf):
        result = system("brctl addbr %s" % itf)
        if result != 0:
            return False
        result = system("brctl setfd %s 0" % itf)
        if result != 0:
            return False
        result = system("ifconfig %s up" % itf)
        return result == 0
    
    def unconfigure_bridge(self, itf):
        result = system("ifconfig %s down" % itf)
        if result != 0:
            return False
        result = system("brctl delbr %s" % itf)
        return result == 0
    
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

    def add_host(self, address, itf='tap1'):
        result = system("tunctl -t %s" % itf)
        if result != 0:
            return False
        self.local_host=itf
        result = system("ip link set dev %s up" % itf)
        if result != 0:
            return False
        return self.configure_if(itf, address)

    def remove_host(self, itf='tap1'):
        result = system("tunctl -d %s" % itf)
        self.local_host=None
        return result == 0

    def start_ra(self, itf, prefix):
        print >> sys.stderr, "Start RA daemon (%s)..." % prefix
        system("sysctl -q -w net.ipv6.conf.%s.forwarding=1" % itf)
        #self.radvd = subprocess.Popen(args=["radvd", "-d", "1", "-C", "radvd/radvd.%s.%s.conf" % (itf,prefix)], shell=True, preexec_fn=os.setsid)
        self.radvd = subprocess.Popen(args=("radvd -d 1 -C radvd/radvd.%s.%s.conf" % (itf,prefix)).split())
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
        result = system("ifconfig %s | grep '%s' > /dev/null" % (itf, prefix))
        return result == 0

    def get_address_with_prefix(self, itf, prefix):
        return subprocess.check_output("ifconfig %s | egrep -o '(%s[:0-9a-f]+)'" % (itf, prefix), shell=True)

    def accept_ra(self, itf):
        system("sysctl -q -w net.ipv6.conf.%s.forwarding=0" % itf)
        system("sysctl -q -w net.ipv6.conf.%s.accept_ra=1" % itf)
        return True

    def support_rio(self):
        return True

    def accept_rio(self, itf):
        system("sysctl -q -w net.ipv6.conf.%s.accept_ra_rt_info_max_plen=64" % itf)
        return True

    def ping(self, target, payload=None, source=None):
        #print "ping %s" % target
        options=''
        if payload is None: payload=config.ping_payload
        if source:
            options+='-I %s' % source
        result = system("ping6 %s -s %d -W %d -c 1 %s > /dev/null" % (options, payload, config.ping_timeout, target))
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
            result = system("ping6 -D -s %d -W %d -c 1 %s 2>&1 >> %s" % (config.ping_payload, config.ping_timeout, target, out))
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
