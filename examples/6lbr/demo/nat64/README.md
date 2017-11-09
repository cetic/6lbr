# Non-Storing Demo

The NAT-64 demo activates the NAT-64 support in 6LBR, allowing to reach the BR and the nodes using IPv4.

## Configuration

### 6LBR configuration

By default NAT-64 is disabled in 6LBR, it can be enabled in the configuration page of the webserver or in the nvm.dat file using nvm_tool with the following option :

    --nat64-enable 1 --nat64-static-ports-enable 1
    
### Node configuration

The nodes are using the standard 6lbr-demo firmware

## Demo

Once started, the BR can be reached using its IPv4 address at [http://172.16.0.2/](http://172.16.0.2/) (The demo scripts assign 172.16.0.1 to the tap interface)

The nodes can be reached using the same IPv4 address as the BR, the port number is used to select the right node. For example, you can reach the node fd00::202:2:2:2 using port [25000](http://172.16.0.2:25000/) in TCP and [20000](coap://172.16.0.2:20000/) in CoAP


