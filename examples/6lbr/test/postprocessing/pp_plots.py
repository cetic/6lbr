from __future__ import print_function
from pylab import *

def plot1(results):
    fig = plt.figure() #figsize=(,)
    ax = fig.add_subplot(111)
    x = []
    y = []
    for result in results:
        if result.ping_info != None:
            if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                x.append(64 - int(result.ping_info['ping1']['ttl']))
                y.append(result.time_info['ping1'])
    ax.scatter(x,y)
    fig.savefig('plot.pdf', format='pdf')
