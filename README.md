/!\ IN DEVELOPMENT

#6LowPan ND
Implementation of Neighbor Discovery Optimization for IPv6 over Low-Power 
Wireless Personal Area Networks on Contiki operating system (fork).

Integration is based on [RFC6775](http://tools.ietf.org/html/rfc6775) to optimize IPv6 ND on tiny OS.

This repository is a fork of Contiki with addition
* Simple interaction between 6LBR and Hosts	
	* Host configure the IP address with NDP	
	* Interaction RS, RA, NS and NA (described in [RFC6775](http://tools.ietf.org/html/rfc6775)) is okay and is maintained with periodic refresh. It can send UDP packet from host to 6LBR
	* Compression of IP address with context prefixes and CID flag in 6LoWPAN
* 

##Source
* contiki OS: [http://contiki-os.org](http://contiki-os.org)
* fork : [https://github.com/contiki-os/contiki](https://github.com/contiki-os/contiki)
