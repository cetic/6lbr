#The 6LoWPAN-ND implementation on Contiki

[![Build Status](https://travis-ci.org/sdefauw/contiki.svg?branch=dev_RPL)](https://travis-ci.org/sdefauw/contiki)

Implementation of Neighbor Discovery Optimization for IPv6 over Low-Power Wireless Personal Area Networks on Contiki operating system (fork).

Integration is based on [RFC6775](http://tools.ietf.org/html/rfc6775) to optimize IPv6 NDP this tiny Operating system.

Through this fork, we implement 6LoWPAN-ND on Contiki as accurately as possible. Some issues and optimization are discussed in the master's thesis report (section source)

##How to use

###User
Add 6LoWPAN-ND entity macro in the Makefile:

* UIP_CONF_6LN: 6LoWPAN-ND Host
* UIP_CONF_6LR: 6LoWPAN-ND Router
* UIP_CONF_6LBR: 6LoWPAN-ND Border Router

###Git repository

* pur : implementation et validation of 6LoWPAN-ND with manuel routing
* rpl : implementation et validation of 6LoWPAN-ND with routing by RPL
* optimization: implementation and validation of optimization between 6LoWPAN-ND and RPL
* depl_cetic : micro deployment in CETIC


##Documentation
Master's thesis report (in French): [dir](https://bitbucket.org/sdefauw/memoire/) [pdf](https://bytebucket.org/sdefauw/memoire/raw/bbcea8351661e62f5aecf97bc66a58536f79fc39/me%CC%81moire.pdf)


##Source
* contiki OS: [http://contiki-os.org](http://contiki-os.org)
* fork : [https://github.com/contiki-os/contiki](https://github.com/contiki-os/contiki)
