from __future__ import print_function
from pylab import *
import re

import pp_plot_multibrsmartbridgeauto
import pp_plot_smartbridgeauto

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

def plot_all(results):

    pp_plot_smartbridgeauto.scatterplot_SmartBridgeAuto(results)
    pp_plot_smartbridgeauto.scatterplot_SmartBridgeAuto_seperate(results)
    pp_plot_multibrsmartbridgeauto.scatterplot_MultiBrSmartBridgeAuto(results)


