from __future__ import print_function
from pylab import *
import re

import pp_plot_router
import pp_plot_smartbridgeauto
import pp_plot_rplroottransparentbridge
import pp_plot_multibrsmartbridgeauto
import pp_plot_rplrootmultitransparentbridge


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

    # pp_plot_router.scatterplot_Router(results)
    # # pp_plot_router.scatterplot_Router_separate(results)
    # pp_plot_router.scatterplot_Router_mean(results)
    
    # pp_plot_smartbridgeauto.scatterplot_SmartBridgeAuto(results)
    # # pp_plot_smartbridgeauto.scatterplot_SmartBridgeAuto_separate(results)
    # pp_plot_smartbridgeauto.scatterplot_SmartBridgeAuto_mean(results)
    
    # pp_plot_rplroottransparentbridge.scatterplot_RplRootTransparentBridge(results)
    # # pp_plot_rplroottransparentbridge.scatterplot_RplRootTransparentBridge_separate(results)
    # pp_plot_rplroottransparentbridge.scatterplot_RplRootTransparentBridge_mean(results)
    
    # pp_plot_multibrsmartbridgeauto.scatterplot_MultiBrSmartBridgeAuto_400x(results)
    # pp_plot_multibrsmartbridgeauto.scatterplot_MultiBrSmartBridgeAuto_500x(results)
    pp_plot_multibrsmartbridgeauto.scatterplot_MultiBrSmartBridgeAuto_500x_vs_delay(results)
    # pp_plot_multibrsmartbridgeauto.scatterplot_MultiBrSmartBridgeAuto_502x(results)
    pp_plot_multibrsmartbridgeauto.scatterplot_MultiBrSmartBridgeAuto_502x_vs_delay(results)
    # pp_plot_multibrsmartbridgeauto.scatterplot_MultiBrSmartBridgeAuto_600x(results)
    
    # pp_plot_rplrootmultitransparentbridge.scatterplot_RplRootMultiTransparentBridge_400x(results)
    # pp_plot_rplrootmultitransparentbridge.scatterplot_RplRootMultiTransparentBridge_500x_by_traffic(results)
    # pp_plot_rplrootmultitransparentbridge.scatterplot_RplRootMultiTransparentBridge_500x_by_delay(results)
    pp_plot_rplrootmultitransparentbridge.scatterplot_RplRootMultiTransparentBridge_500x_vs_delay(results)
    # pp_plot_rplrootmultitransparentbridge.scatterplot_RplRootMultiTransparentBridge_502x(results)
    pp_plot_rplrootmultitransparentbridge.scatterplot_RplRootMultiTransparentBridge_502x_vs_delay(results)
    # pp_plot_rplrootmultitransparentbridge.scatterplot_RplRootMultiTransparentBridge_600x(results)

