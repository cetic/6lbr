# General environment variables
SHELL=/bin/bash
CONTIKIDIR=../../..
COOJADIR=${CONTIKIDIR}/tools/cooja
COOJA_USER_CONFIG=~/.cooja.user.properties
COOJA_ROOT_CONFIG=/root/.cooja.user.properties
COOJA_CONFIG_SAMPLE=./conf-templates/cooja.user.properties.example

# COOJA generation
COOJAGEN=./coojagen

launch-6lbr-router-1-cooja:
	../package/usr/bin/6lbr conf-templates/cooja-1-router.conf

launch-6lbr-smartbridge-2br-cooja:
	@echo "*************************************************************************"
	@echo "* Creating bridge in LAZY mode, needs to be deleted manually at the end *"
	@echo "* Example:    ip link set br0 down                                      *"
	@echo "*             brctl delbr br0                                           *"
	@echo "*************************************************************************"
	../package/usr/bin/6lbr conf-templates/cooja-2-smartbridge-a.conf &
	../package/usr/bin/6lbr conf-templates/cooja-2-smartbridge-b.conf &

cooja-small: preset_1dag_10nodes.csc build-cooja config-cooja launch-cooja

cooja-2br-grid: grid_2br_3x6.csc build-cooja config-cooja launch-cooja

launch-cooja:
	java -jar ${COOJADIR}/dist/cooja.jar -quickstart=`cat ${COOJAGEN}/output/LASTFILE`

launch-cooja-nogui:
	java -jar ${COOJADIR}/dist/cooja.jar -nogui=`cat ${COOJAGEN}/output/LASTFILE`

%.csc: ${COOJAGEN}/examples/config_%.py
	python2.7 gensimfile.py $<

build-cooja: do-build-cooja serial2pty radiologger-headless

do-build-cooja:
	pushd ${COOJADIR}; ant jar; popd

config-cooja:
	if [ ! -f $(COOJA_USER_CONFIG) ]; then cp $(COOJA_CONFIG_SAMPLE) $(COOJA_USER_CONFIG); fi
	if [ `whoami` = root ] && [ ! -f $(COOJA_ROOT_CONFIG) ] ; then cp $(COOJA_CONFIG_SAMPLE) $(COOJA_ROOT_CONFIG); fi
	if ! grep -Fq "serial2pty" $(COOJA_USER_CONFIG) ; then sed -i "/^DEFAULT_PROJECTDIRS=/ s/\$$/\;\[APPS_DIR\]\/serial2pty/" $(COOJA_USER_CONFIG); fi
	if ! grep -Fq "radiologger-headless" $(COOJA_USER_CONFIG) ; then sed -i "/^DEFAULT_PROJECTDIRS=/ s/\$$/\;\[APPS_DIR\]\/radiologger-headless/" $(COOJA_USER_CONFIG); fi
	if [ `whoami` = root ] && ! grep -Fq serial2pty $(COOJA_ROOT_CONFIG) ; then sed -i "/^DEFAULT_PROJECTDIRS=/ s/\$$/\;\[APPS_DIR\]\/serial2pty/" $(COOJA_ROOT_CONFIG); fi
	if [ `whoami` = root ] && ! grep -Fq radiologger-headless $(COOJA_ROOT_CONFIG) ; then sed -i "/^DEFAULT_PROJECTDIRS=/ s/\$$/\;\[APPS_DIR\]\/radiologger-headless/" $(COOJA_ROOT_CONFIG); fi

serial2pty:
	pushd ${COOJADIR}/apps/serial2pty; echo $(JAVA_HOME); ant jar; popd

radiologger-headless:	
	pushd ${COOJADIR}/apps/radiologger-headless; ant jar; popd

clean-cooja-firmwares:
	cd ${COOJAGEN}/firmwares/6lbr-demo && $(MAKE) TARGET=sky clean
	cd ${COOJAGEN}/firmwares/6lbr-demo-delay && $(MAKE) TARGET=sky clean
	cd ${COOJAGEN}/firmwares/6lbr-demo-ndp && $(MAKE) TARGET=sky clean
	cd ${CONTIKIDIR}/examples/ipv6/slip-radio && $(MAKE) TARGET=sky clean
	cd ${COOJAGEN}/firmwares/6lbr-demo && $(MAKE) TARGET=cooja clean
	cd ${COOJAGEN}/firmwares/6lbr-demo-delay && $(MAKE) TARGET=cooja clean
	cd ${COOJAGEN}/firmwares/6lbr-demo-ndp && $(MAKE) TARGET=cooja clean
	cd ${CONTIKIDIR}/examples/ipv6/slip-radio && $(MAKE) TARGET=cooja clean

build-cooja-firmwares:
	cd ${COOJAGEN}/firmwares/6lbr-demo && $(MAKE) TARGET=sky
	cd ${COOJAGEN}/firmwares/6lbr-demo-delay && $(MAKE) TARGET=sky
	cd ${COOJAGEN}/firmwares/6lbr-demo-ndp && $(MAKE) TARGET=sky
	cd ${CONTIKIDIR}/examples/ipv6/slip-radio && $(MAKE) TARGET=sky

non-regression: nr-router nr-router-llsec nr-smartbridge nr-rpl-root-6lr nr-ndp-router nr-rpl-root-rpl-relay nr-multi-smartbridge nr-rpl-root-multi-rpl-relay nr-rpl-multi-6lr nr-result

nr-router:
	$(MAKE) preset_1dag_10nodes.csc
	python2.7 run_tests.py --scenarios NonRegressionScenarios --modes Router --log nr-router.log

nr-router-llsec:
	$(MAKE) preset_1dag_10nodes_llsec.csc
	python2.7 run_tests.py --scenarios NonRegressionScenarios --modes RouterLlsec --log nr-router.log

nr-smartbridge:
	$(MAKE) preset_1dag_10nodes.csc
	python2.7 run_tests.py --scenarios NonRegressionScenarios --modes SmartBridge --log nr-smartbridge.log

nr-rpl-root-6lr:
	$(MAKE) preset_1dag_10nodes.csc
	python2.7 run_tests.py --scenarios NonRegressionScenarios --modes RplRoot6lr --log nr-rpl-root-6lr.log

nr-ndp-router:
	$(MAKE) preset_1dag_1node.csc
	python2.7 run_tests.py --scenarios NonRegressionScenarios --modes NdpRouter --log nr-ndp-router.log

nr-rpl-root-rpl-relay:
	$(MAKE) preset_1dag_10nodes.csc
	python2.7 run_tests.py --scenarios NonRegressionScenarios --modes RplRootRplRelay --log nr-rpl-root-rpl-relay.log

nr-multi-smartbridge:
	$(MAKE) preset_2dags_20nodes_disjoint.csc
	python2.7 run_tests.py --scenarios MultiBrDisjointScenarios --modes MultiSmartBridge --log nr-multi-smartbridge-disjoint.log
	$(MAKE) grid_2br_3x6.csc
	python2.7 run_tests.py --scenarios MultiBrOverlapScenarios --modes MultiSmartBridge --log nr-multi-smartbridge-overlap.log

nr-rpl-root-multi-rpl-relay:
	$(MAKE) preset_2dags_20nodes_disjoint.csc
	python2.7 run_tests.py --scenarios MultiBrDisjointScenarios --modes RplRootMultiRplRelay --log nr-rpl-root-multi-rpl-relay-disjoint.log
	$(MAKE) grid_2br_3x6.csc
	python2.7 run_tests.py --scenarios MultiBrOverlapScenarios --modes RplRootMultiRplRelay --log nr-rpl-root-multi-rpl-relay-overlap.log

nr-rpl-multi-6lr:
	$(MAKE) preset_2dags_20nodes_disjoint.csc
	python2.7 run_tests.py --scenarios MultiBrDisjointScenarios --modes Multi6lr --log nr-rpl-multi-6lr-disjoint.log
	$(MAKE) grid_2br_3x6.csc
	python2.7 run_tests.py --scenarios MultiBrOverlapScenarios --modes Multi6lr --log nr-rpl-multi-6lr-overlap.log

nr-result:
	find report -name 'nr-*.log' | xargs head -6 | grep -v SUMMARY
