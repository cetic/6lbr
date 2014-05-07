all: gen simu matlab

gen:
	cd ../; python gentest.py; cd topology/

simu: line-rpl.simu

%.simu:
	sh sim.sh $*

matlab:
	cd graph/; python adddata.py; cd ../

%.matlab:
	cd graph/; python adddata.py $*; cd ../


%.all: gen
	make $*.simu; make $*.matlab

rmdevice:
	cd ../../; rm -rf 6l*/*sky* 6l*/*wismote* 6l*/*z1*; cd test-quantitative/topology/