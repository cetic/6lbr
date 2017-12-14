# Simple Router Demo

The Simple Router demo is the simplest configuration of 6LBR in Router mode.

## Configuration

### 6LBR Configuration

The mode is enabled by setting in [6lbr.conf](6lbr/6lbr.conf):

    MODE=ROUTER

### Node configuration

The nodes are using the standard 6lbr-demo firmware

## Demo

Once started, the BR can be reached at [http://[bbbb::100]/](http://[bbbb::100]/)

The nodes can be reached in the fd00:: subnet, for example : fd00::202:2:2:2

You can test the connectivity with the node using ping :

    ping6 fd00::202:2:2:2

You can reach the node using CoAP : [coap://[fd00::202:2:2:2/]/](coap://[fd00::202:2:2:2/]/)

Or via HTTP : [http://[fd00::202:2:2:2/]/](http://[fd00::202:2:2:2/]/)

