# RPL-Root+RPL-Relay Demo

The RPL-Root+RPL-Relay demo shows the configuration of two 6LBR, one acting as a standalone RPL Root, the second being a simple packet relay.

## Configuration

### 6LBR configuration

The RPL-Root mode is enabled by setting in [6lbr.conf](6lbr-2/6lbr.conf):

    MODE=RPL-ROOT

The RPL-Root does not communicate directly with the WSN, the packets are forwarded to the RPL-Relay. Therefore there is no radio configuration in the file.

The RPL-Relay mode is enabled by setting in [6lbr.conf](6lbr-1/6lbr.conf):

    MODE=RPL-RELAY

### Node configuration

The nodes are using the standard 6lbr-demo firmware

## Demo

Once started, the RPL-Root BR can be reached at [http://[fd00::201:1:1:1]/](http://[fd00::201:1:1:1]/).

