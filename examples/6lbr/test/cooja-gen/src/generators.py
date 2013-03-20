import math
import random

def gengrid(step, xcount, ycount):
	points = []
	for xcoord in range(xcount):
		for ycoord in range(ycount):
			point = (xcoord*step,ycoord*step)
			points.append(point)

	return(points)

def genrandom(xmin,xmax,ymin,ymax,count):
	random.seed()
	points = []
	for a in range(count):
		points.append((random.uniform(xmin,xmax),random.uniform(ymin,ymax)))

	return(points)


