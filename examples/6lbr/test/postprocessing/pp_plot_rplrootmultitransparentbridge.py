from pylab import *
import re
import inspect

formatter = matplotlib.ticker.EngFormatter(places=3)
formatter.ENG_PREFIXES[-6] = 'u'

def scatterplot_RplRootMultiTransparentBridge_400x(results):

    print "scatterplot_RplRootMultiTransparentBridge_400x"
    data = {}

    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (ms)"

    for result in results:
        if result.mode == "RplRootMultiTransparentBridge" and 'S400' in result.id:
            if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                    if result.id not in data:
                        data[result.id] = {'x':[], 'y':[]}

                    data[result.id]['x'].append(64 - int(result.ping_info['ping1']['ttl']))
                    data[result.id]['y'].append(int(result.time_info['pingm'])/1000)

    fig400x = plt.figure(figsize=(25,15)) #figsize=(,)
    index400x = 1

    for testid in sorted(data.keys()):
        if 'S400' in testid:
            ax = fig400x.add_subplot(nrow,ncol,index400x, title="%s, %d points" % (testid,len(data[testid]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
            ax.scatter(data[testid]['x'],data[testid]['y'])
            print("plotting %s len %d" % (testid, len(data[testid]['x'])))
            # print(index400x)
            index400x+=1

    fig400x.savefig('RplRootMultiTransparentBridge_400x.pdf', format='pdf')

def scatterplot_RplRootMultiTransparentBridge_500x(results):

    print "scatterplot_RplRootMultiTransparentBridge_500x"
    data = {}

    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (ms)"

    for result in results:
        if result.mode == "RplRootMultiTransparentBridge" and 'S500' in result.id:
            if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                    if result.id not in data:
                        data[result.id] = {'x':[], 'y':[]}

                    data[result.id]['x'].append(64 - int(result.ping_info['ping1']['ttl']))
                    data[result.id]['y'].append(int(result.time_info['pingm'])/1000)

    fig500x = plt.figure(figsize=(25,15)) #figsize=(,)
    index500x = 1

    for testid in sorted(data.keys()):
        if 'S500' in testid:
            ax = fig500x.add_subplot(nrow,ncol,index500x, title="%s, %d points" % (testid,len(data[testid]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
            ax.scatter(data[testid]['x'],data[testid]['y'])
            print("plotting %s len %d" % (testid, len(data[testid]['x'])))
            # print(index500x)
            index500x+=1

    fig500x.savefig('RplRootMultiTransparentBridge_500x.pdf', format='pdf')


def scatterplot_RplRootMultiTransparentBridge_502x(results):

    print "scatterplot_RplRootMultiTransparentBridge_502x"
    data = {}

    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (ms)"

    for result in results:
        if result.mode == "RplRootMultiTransparentBridge" and 'S502' in result.id:
            if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                    if result.id not in data:
                        data[result.id] = {}
                    if result.start_delay not in data[result.id]:
                        data[result.id][result.start_delay] = {'x':[], 'y':[]}

                    data[result.id][result.start_delay]['x'].append(64 - int(result.ping_info['ping1']['ttl']))
                    data[result.id][result.start_delay]['y'].append(int(result.time_info['pingm'])/1000)

    fig502x = plt.figure(figsize=(25,15))
    index502x = 1

    for testid in data:
        sortedid = sorted(data.keys())
        for start_delay in data[testid]:
            sorteddelay = sorted(data[testid].keys())
            if 'S502' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid) + 1
                ax = fig502x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                # print(index502x)
                # index502x+=1

    fig502x.savefig('RplRootMultiTransparentBridge_502x.pdf', format='pdf')

    
def scatterplot_RplRootMultiTransparentBridge_600x(results):

    print "scatterplot_RplRootMultiTransparentBridge_600x"
    data = {}

    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (ms)"

    for result in results:
        if result.mode == "RplRootMultiTransparentBridge" and 'S502' in result.id:
            if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                    if result.id not in data:
                        data[result.id] = {}
                    if result.start_delay not in data[result.id]:
                        data[result.id][result.start_delay] = {'x':[], 'y':[]}

                    data[result.id][result.start_delay]['x'].append(64 - int(result.ping_info['ping1']['ttl']))
                    data[result.id][result.start_delay]['y'].append(int(result.time_info['pingm'])/1000)

    fig600x = plt.figure(figsize=(25,15))
    index600x = 1

    for testid in data:
        sortedid = sorted(data.keys())
        for start_delay in data[testid]:
            sorteddelay = sorted(data[testid].keys())
            if 'S600' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid) + 1
                ax = fig600x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                # print(index600x)
                index600x+=1

    fig600x.savefig('RplRootMultiTransparentBridge_600x.pdf', format='pdf')
