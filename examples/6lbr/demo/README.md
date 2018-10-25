# CETIC 6LBR demos

## Introduction

This directory contains various standalone demo showcasing the various features of CETIC 6LBR. All these demos are using the Cooja simulation tool connected to one or more local instances of 6LBR.

## Usage

### 1 - Setup the demo

To build the needed softwares and run the demo, one has to go to the actual demo directory and simply run the following command :

    make all
    
The `all` target will clean all the dependencies and rebuild the needed 6LBR software, the node and slip-radio firmwares and launch Cooja. A console window will be opened for each 6LBR instance and one for Cooja
Before launching the BRs, the tool will ask you the root password, this step is required to create the tap interfaces needed to run the BRs (and a virtual bridge if testing multi-br). The BRs themselves won't run as root.

### 2 - Run the simulation

When the demo environment is launched, you will have one console window for each BR, a console window for Cooja and the Cooja window itself. By default, the Cooja simulation is not running, to start the demo you have to click on the start button in the Cooja Window

### 3 - Interacting with the Demo

Once the BR is up and running (you can confirm that when the line '6LBR started' appears in the BR window), you can use a browser to reach the webserver of the BR or interact with one of the nodes. See the demo Readme for the actual IP addresses of the BR and nodes as it depends on the mode selected.

### 4 - Ending the demo

To end the demo, simply press 'Enter' in the console where you launched the demo, the script will stop Cooja and all the BR. You might have to again enter the root password in order to remove the tap interfaces.

## Demos

### Simple Router

The [Simple Router](simple-router/README.md) demo is the simplest configuration of 6LBR in Router mode.

### Simple Smart-bridge

The [Simple Smart-Bridge](simple-smartbridge/README.md) demo is the simplest configuration of 6LBR in SmartBridge mode.

### Non-Storing

The [Non-Storing](non-storing/README.md) demo creates a WSN network using the Non-Storing mode of RPL.

### NAT-64

The [NAT-64](nat64/README.md) demo activates the NAT-64 support in 6LBR, allowing to reach the BR and the nodes using IPv4.

### TSCH

The [TSCH](tsch/README.md) demo activates the TSCH MAC layer in the slip-radio and activates its support in 6LBR.

### LLSEC

The [LLSEC](llsec/README.md) demo enables Link-Layer Security in the WSN using NonCoresec engine.

### AdaptiveSec

The [AdaptiveSec](adaptivesec/README.md) demo enables Link-Layer Security in the WSN using AdaptiveSec engine.

### Multi-Radio

The [MultiRadio](multi-radio/README.md) demo shows how to run 6LBR with two slip-radios.

### 100 Nodes

The [100 nodes](100-nodes/README.md) demo show how 6LBR behaves in a high-density network.

### NDP-Router

The [NDP-Router](ndp-router/README.md) demo creates a WSN network without using RPL.

### Multi-SmartBridge

The [Multi-SmartBridge](multi-smartbridge/README.md) demo shows the configuration of two 6LBR in SmartBridge mode with multi-br activated.

### RPL-Root+RPL-Relay

The [RPL-Root+RPL-Relay](multi-smartbridge/README.md) demo shows the configuration of two 6LBR, one acting as a standalone RPL Root, the second being a simple packet relay.

### Plugins

The [Plugins](plugins/README.md) demo demonstrates loading of plugins in native 6LBR.

