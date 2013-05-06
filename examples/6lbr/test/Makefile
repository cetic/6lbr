# General environment variables
SHELL=/bin/bash
CONTIKIDIR=../../..
COOJADIR=${CONTIKIDIR}/tools/cooja

# COOJA generation
COOJAGEN=./coojagen

launch-6lbr-router-1-cooja:
	../package/usr/bin/6lbr conf-templates/cooja-1-router.conf

cooja-small: preset_1dag_10nodes.csc build-cooja
	java -jar ${COOJADIR}/dist/cooja.jar -quickstart=`cat ${COOJAGEN}/output/LASTFILE`

%.csc: ${COOJAGEN}/examples/config_%.py ${COOJAGEN}/templates/%
	python2.7 gensimfile.py $<

build-cooja: serial2pty radiologger-headless
	pushd ${COOJADIR}; ant jar; popd

serial2pty:
	pushd ${COOJADIR}/apps/serial2pty; ant jar; popd

radiologger-headless:	
	pushd ${COOJADIR}/apps/radiologger-headless; ant jar; popd

