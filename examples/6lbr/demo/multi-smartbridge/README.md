# Multi Smart-Bridge Demo

The Multi Smart-Bridge demo shows the configuration of two 6LBR in SmartBridge mode with multi-br activated.

## Configuration

### 6LBR configuration

The mode is enabled by setting in [6lbr.conf](6lbr/6lbr.conf):

    MODE=SMART-BRIDGE

In 6LBR version 1.4.x, by default, the BR requires a working IPv6 router on the Ethernet segment to provides the network prefix. In order to have the demo working without one, the prefix is preconfigured in the nvm.dat file using nvm_tool with the following option :

    --wsn-accept-ra 0

The multi BR is activated using nvm_tool with the following option or via the configuration page of the webserver :

    --smart-multi-br 1

### Node configuration

The nodes are using the standard 6lbr-demo firmware

## Demo

Once started, the first BR can be reached at [http://[fd00::201:1:1:1]/](http://[fd00::201:1:1:1]/), the second BR can be reached at [http://[fd00::202:2:2:2]/](http://[fd00::202:2:2:2]/)


