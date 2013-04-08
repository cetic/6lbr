#!/usr/bin/python2.7
"""
# Example of a native 6lbr interacting with a real slip-radio (radio_dev) and a real 6lbr-demo client (mote_dev)
report_path='report/run-current/coojasim-current/iter-current/test_current'
slip_radio=[ {'dev': '/dev/tty.usbserial-XBTOKIQP', 'iid': '0212:7400:1466:5117'}, {'dev': 'tty.usbserial-003013FDB', 'iid': '205:c2a:8cee:1c6d'}]
motes = [ {'dev': '/dev/tty.usbserial-XBTO3BTS', 'iid': '0212:7400:146e:f407'}]
mote_baudrate=115200
channel=24

from support import *

backboneClass=NativeTapBB
wsnClass=LocalWsn
brClass=LocalNativeBR
moteClass=LocalTelosMote
#platform=MacOSX()
platform=Linux()
"""
""" 
This next configuration is an example of a COOJA-based test on Linux. A native 6lbr interacts with a simulated
slip-radio mote through a local socket and a simulated mote on a hardcoded pseudo terminal 
TODO: make /dev/pts/# dynamic, launch cooja simulation & socat automatically
"""
report_path='report/run-current/coojasim-current/iter-current/test_current'
backbone_dev='br0'
mote_baudrate=115200
channel=26
udp_port=3000

#Single BR Topologies
#simgen_config_path='./coojagen/examples/config_line_single_br.py' # Line: 2,4,6,8,10
#simgen_config_path='./coojagen/examples/config_grid_single_br.py' # Grid Ratio 1:2: 2x4, 3x6, 4x8

#2-BR Topologies
simgen_config_path='./coojagen/examples/config_preset_2dags_20nodes_disjoint.py' # Disjoint DAGs
#simgen_config_path='./coojagen/examples/config_preset_3slipradio_2dags_20nodes_disjoint.py' # Disjoint DAGs, 3 slip-radios
#simgen_config_path='./coojagen/examples/config_preset_2dags_20nodes_overlap.py' # TODO: Overlapping DAGs
#simgen_config_path='./coojagen/examples/config_grid_2br.py' # Grid Ratio 1:2: 2x4, 3x6, 4x8 with mote close to BR1

from support import *

backboneClass=NativeBridgeBB
brClass=LocalNativeBR
wsnClass=CoojaWsn
#platform=MacOSX()
platform=Linux()

"""
scenarios
define 1 to run the scenario, 0 to skip
"""
S0=0
S1=0

S1_move=0

S2=0
S3=0
S4=0
S5=0
S6=0
S7=0
S8=0
S9=0
S10=0
S11=0
S12=0

S1001=1
S1002=1
S1003=1
S1011=1
S1012=1
S1013=1

S1101=1
S1102=1
S1103=1
S1111=1
S1112=1
S1113=1

S2001=1
S2002=1
S2003=1
S2011=1
S2012=1
S2013=1

S5001=1
S5002=1
S5003=1

S101x_start_delay = 20

test_repeat=1

"""
modes
define 1 to run each selected scenario in that mode, 0 to skip the mode
"""
mode_SmartBridgeManual=0
mode_SmartBridgeAuto=1
mode_Router=1
mode_RouterNoRa=0

mode_RplRootTransparentBridge=1

mode_MultiBrSmartBridgeAuto=0
mode_RplRootMultiTransparentBridge=0

#Debug modes, do not use
mode_RplRoot=0
mode_RplRootNoRa=0
mode_TransparentBridgeManual=0
mode_TransparentBridgeAuto=0
