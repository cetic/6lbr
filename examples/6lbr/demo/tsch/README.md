# TSCH Demo

The TSCH demo creates a WSN network using the TSCH MAC (Configured in minumal scheduling). This demo is based on the [Simple Router](../simple-router/README.md) demo, only the specific configuration is documented here.


## Configuration

### 6LBR configuration

The default MAC is CSMA, this cannot be switched directly to TSCH, instead the MAC layer must be in the slip-radio, the packed exchanged between the BR and the slip-radio are no longer 802.15.4 frames but vanilla IPv6 packets. *NullMAC*, a transparent MAC layer, is enabled in 6LBR in the configuration page of the webserver or in the nvm.dat file using nvm_tool with the following option :

    --mac-layer 2

Also, the slip-radio configuration must be set to slip-ip, so that the IPv6 packets are directly send to the slip-radio :

    [native]
    slip.ip=1

### Slip-Radio configuration

The TSCH MAC layer can be enabled in the slip-radio by setting the following flag in the [Makefile](../firmwares/non-storing/Makefile) :

    WITH_TSCH=1

The IP mode of the slip-radio can be enabled by setting the following flag in the [Makefile](../firmwares/non-storing/Makefile) :

    WITH_SLIP_IP=1

Both can also be enabled in the project-conf.h file.

### Node configuration

The TSCH MAC layer can be enabled in 6lbr-demo by setting the following flag in the [Makefile](../firmwares/non-storing/Makefile) :

    WITH_TSCH=1

It can also be enabled in the project-conf.h file.

## Demo

Once started, the BR can be reached at [http://[bbbb::100]/](http://[bbbb::100]/)

The nodes can be reached in the fd00:: subnet, for example : fd00::202:2:2:2

You can test the connectivity with the node using ping :

    ping6 fd00::202:2:2:2

You can reach the node using CoAP : [coap://[fd00::202:2:2:2/]/](coap://[fd00::202:2:2:2/]/)

Or via HTTP : [http://[fd00::202:2:2:2/]/](http://[fd00::202:2:2:2/]/)

