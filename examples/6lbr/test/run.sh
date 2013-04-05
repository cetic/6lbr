#!/bin/bash

CALLDIR=$(dirname $(readlink -f $0))
rm -rf ${CALLDIR}/*.pyc
sudo ${CALLDIR}/top.py
