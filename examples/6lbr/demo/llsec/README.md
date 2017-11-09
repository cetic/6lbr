# LLSEC Demo

The LLSEC demo enables Link-Layer Security in the WSN using NonCoresec engine. This demo is based on the [Simple Router](../simple-router/README.md) demo, only the specific configuration is documented here.

## Configuration

### 6LBR configuration

The LLSEC mode can be enabled in the configuration page of the webserver or in the nvm.dat file using nvm_tool with the following option :

    --security-layer 1 --security-level 6
    
This correspond to Noncoresec engine with AES-CCM-64 and anti-replay deactivated. The default key is : 000102030405060708090A0B0C0D0E0F

### Node configuration

The LLSEC can be enabled in 6lbr-demo by setting the following flag in the [Makefile](../firmwares/llsec/Makefile) :

    WITH_LLSEC=1

It can also be enabled in the project-conf.h file.