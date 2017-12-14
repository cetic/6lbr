# Non-Storing Demo

The Non-Storing demo creates a WSN network using the Non-Storing mode of RPL. This demo is based on the [Simple Router](../simple-router/README.md) demo, only the specific configuration is documented here.

## Configuration

### 6LBR configuration

The default RPL mode is Storing Mode, this can be switched to Non-Storing Mode in the configuration page of the webserver or in the nvm.dat file using nvm_tool with the following option :

    --rpl-non-storing 1
    
### Node configuration

The Non-Storing mode can be enabled in 6lbr-demo by setting the following flag in the [Makefile](../firmwares/non-storing/Makefile) :

    RPL_NON_STORING=1

It can also be enabled in the project-conf.h file.