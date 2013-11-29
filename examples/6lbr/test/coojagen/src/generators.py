import math
import random

def gengrid(step, xcount, ycount):
	step = int(step)
	xcount = int(xcount)
	ycount = int(ycount)
	points = []
	for xcoord in range(xcount):
		for ycoord in range(ycount):
			point = (xcoord*step,ycoord*step, 0)
			points.append(point)
	return(points)

def gengrid_ratio(step, ratio, count):
	step = int(step)
	count = int(count)

	ratio_xy = ratio.split(':')
	ratio = float(ratio_xy[0])/float(ratio_xy[1])

	xcount = round(math.sqrt(ratio*count))
	ycount = round(xcount / ratio)
	return gengrid(step, xcount, ycount)
	

def gengrid_square(step, count):
	side = round(math.sqrt(count))
	return gengrid(step, side, side)

def genrandom(xmin,xmax,ymin,ymax,count):
	count = int(count)
	xmin = int(xmin)
	ymin = int(ymin)
	xmax = int(xmax)
	ymax = int(ymax)
	random.seed()
	points = []
	for a in range(count):
		points.append((random.uniform(xmin,xmax),random.uniform(ymin,ymax)),0)

	return(points)

def genline(step, count):
	step = float(step)
	count = int(count)
	points = []
	for xcoord in range(count):
		point = (xcoord*step, 0, 0)
		points.append(point)
	return(points)

def hasattrs(_object, _attributes):
	for attr in _attributes:
		if not hasattr(_object, attr):
			print("Error: %s topology requires a '%s' attribute" %(_object.topology, attr))
			return False
	return True

def gen(config_simgen, mote_count):
	
	if not hasattr(config_simgen, 'topology'):
		print("Error: no 'topology' found in sim generation config file")
		return None
	if config_simgen.topology == 'line':
		if not hasattrs(config_simgen, ['step',]):
			print("Error: line topology requires a 'step' parameter")
			return None
		return genline(config_simgen.step, mote_count)

	elif config_simgen.topology == 'grid':
		if not hasattrs(config_simgen, ['step', 'xcount', 'ycount']):
			return None
		return gengrid(config_simgen.step, config_simgen.xcount, config_simgen.ycount)

	elif config_simgen.topology == 'grid_square':
		if not hasattrs(config_simgen, ['step',]):
			return None
		return gengrid_square(config_simgen.step, mote_count)

	elif config_simgen.topology == 'grid_ratio':
		if not hasattrs(config_simgen, ['step', 'ratio']):
			return None
		return gengrid_ratio(config_simgen.step, config_simgen.ratio, mote_count)

def load_preset(preset_data_path):
	points = []
	preset_data_file = open(preset_data_path, 'r')
	
	for line in preset_data_file:
		line = line.rstrip()
		currpoints = []
		positions = line.split(';')
		for position in positions:
			xy = position.split(',')
			x = xy[0]
			y = xy[1]
			currpoints.append((int(x),int(y),0))
		points.append(currpoints)

	preset_data_file.close()
	return(points)
