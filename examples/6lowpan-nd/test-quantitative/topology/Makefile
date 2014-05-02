
all: generate simulate genmatlab

generate:
	cd ../; python gentest.py; cd topology/

simulate:
	sh sim.sh

genmatlab:
	cd graph/; python adddata.py; cd ../