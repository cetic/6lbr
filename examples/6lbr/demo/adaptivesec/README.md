# Adaptivesec Demo

The Adaptivesec demo enables Link-Layer Security in the WSN using Adaptivesec AKES-noncore engine. This demo is based on the [Simple Router](../simple-router/README.md) demo, only the specific configuration is documented here.

## Configuration

### 6LBR configuration

The LLSEC mode can be enabled in the configuration page of the webserver or in the nvm.dat file using nvm_tool with the following option :

    --security-layer 2 --security-level 6
 
 This correspond to AKES noncore engine with AES-CCM-64. The default key is : 000102030405060708090A0B0C0D0E0F
    
### Node configuration

The LLSEC can be enabled in 6lbr-demo by setting the following flag in the [Makefile](../firmwares/adaptivesec/Makefile) :

    WITH_LLSEC=1
    WITH_ADAPTIVESEC=1

It can also be enabled in the project-conf.h file.