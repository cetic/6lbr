# Simple Smart-Bridge Demo

The Simple Smart-Bridge demo is the simplest configuration of 6LBR in SmartBridge mode.

## Configuration

### 6LBR configuration

The mode is enabled by setting in [6lbr.conf](6lbr/6lbr.conf):

    MODE=SMART-BRIDGE

In 6LBR version 1.4.x, by default, the BR requires a working IPv6 router on the Ethernet segment to provides the network prefix. In order to have the demo working without one, the prefix is preconfigured in the nvm.dat file using nvm_tool with the following option :

    --wsn-accept-ra 0

### Node configuration

The nodes are using the standard 6lbr-demo firmware

## Demo

Once started, the BR can be reached at [http://[fd00::201:1:1:1]/](http://[fd00::201:1:1:1]/)

The nodes can be reached in the same fd00:: subnet, for example : fd00::202:2:2:2

You can test the connectivity with the node using ping :

    ping6 fd00::202:2:2:2

You can reach the node using CoAP : [coap://[fd00::202:2:2:2/]/](coap://[fd00::202:2:2:2/]/)

Or via HTTP : [http://[fd00::202:2:2:2/]/](http://[fd00::202:2:2:2/]/)

