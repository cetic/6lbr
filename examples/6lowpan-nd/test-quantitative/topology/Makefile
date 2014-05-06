all: gen simu matlab

gen:
	cd ../; python gentest.py; cd topology/

simu: line.simu

%.simu:
	sh sim.sh $*

matlab:
	cd graph/; python adddata.py; cd ../

%.matlab:
	cd graph/; python adddata.py $*; cd ../


%.all: gen
	make $*.simu; make $*.matlab

rmsky:
	cd ../../6lbr/; rm -rf *sky*; cd ../6lr/; rm -rf *sky*; cd ../6lh/; rm -rf *sky*; cd ../test-quantitative/topology/