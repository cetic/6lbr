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

Once started, the BR can be reached at [http://[aaaa::201:1:1:1]/](http://[aaaa::201:1:1:1]/)

The nodes can be reached in the same aaaa:: subnet, for example : aaaa::202:2:2:2

You can test the connectivity with the node using ping :

    ping6 aaaa::202:2:2:2

You can reach the node using CoAP : [coap://[aaaa::202:2:2:2/]/](coap://[aaaa::202:2:2:2/]/)

Or via HTTP : [http://[aaaa::202:2:2:2/]/](http://[aaaa::202:2:2:2/]/)

