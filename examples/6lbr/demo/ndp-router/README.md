# NDP-Router Demo

The NDP-Router demo creates a WSN network using pure IPv6 network control protocol, NDP. This demo is based on the [Simple Router](../simple-router/README.md) demo, only the specific configuration is documented here.

## Configuration

### 6LBR configuration

The mode is enabled by setting in [6lbr.conf](6lbr/6lbr.conf):

    MODE=NDP-ROUTER

The default Router Lifetime is 0, for a working network, it must be set to a value greater than 0. This can be modified in the configuration page of the webserver or in the nvm.dat file using nvm_tool with the following option :

    --ra-router-lifetime 1800

### Node configuration

The NDP mode can be enabled in 6lbr-demo by setting the following flag in the [Makefile](../firmwares/non-storing/Makefile) :

    WITH_RPL=0

It can also be enabled in the project-conf.h file.
