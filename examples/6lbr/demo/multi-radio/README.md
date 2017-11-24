# MultiRadio Demo

The MultiRadio demo shows how to run 6LBR with two slip-radios. This demo is based on the [Simple Router](../simple-router/README.md) demo, only the specific configuration is documented here.

## Configuration

### 6LBR configuration

To support more than one slip-radio, the configuration must be made in the [nvm.conf](6lbr/nvm.conf) file instead of [6lbr.conf](6lbr/6lbr.conf).

For each slip-radio, a slip-radio entry must be created in the nvm.conf file :

    [slip-radio]
    device=/dev/ttyUSB0
   
### Node configuration

The demo uses the default firmware configuration.