from coojagen.src import lib_generation


simfile = ["line-rpl","grid-rpl", "grid-9-rpl"]

#Generate .csc from template
for filename in simfile:
	config = lib_generation.ConfigParser()
	config.parse_config_file("topology/"+filename+".py")


#Add lib in cooja script
#import from ../test/genjs.js
print("ADD LIBRARY IN COOJA SCRIPT")
def load_src(name, fpath):
    import os, imp
    return imp.load_source(name, os.path.join(os.path.dirname(__file__), fpath))
 
load_src("genjs", "../test/genjs.py")
import genjs

