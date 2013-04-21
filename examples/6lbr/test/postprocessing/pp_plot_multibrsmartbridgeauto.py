from pylab import *
import re
import inspect
from pp_utils import *



formatter = matplotlib.ticker.EngFormatter(places=3)
formatter.ENG_PREFIXES[-6] = 'u'

def scatterplot_MultiBrSmartBridgeAuto_400x(results):

    print "scatterplot_MultiBrSmartBridgeAuto_400x"
    data = {}

    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (s)"

    for result in results:
        if result.mode == "MultiBrSmartBridgeAuto" and 'S400' in result.id:
            if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                    if result.id not in data:
                        data[result.id] = {'x':[], 'y':[]}

                    data[result.id]['x'].append(64 - int(result.ping_info['ping1']['ttl']))
                    data[result.id]['y'].append(int(result.time_info['pingm'])/1000)

    fig400x = plt.figure(figsize=(25,15)) #figsize=(,)
    index400x = 1

    for testid in sorted(data.keys()):
        if 'S400' in testid:
            # ax = fig400x.add_subplot(nrow,ncol,index400x, title="%s, %d points" % (testid,len(data[testid]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
            ax = fig400x.add_subplot(nrow,ncol,index400x, title="%s, %d points" % (testid,len(data[testid]['x'])), xlabel=xtitle, ylabel=ytitle)
            ax.scatter(data[testid]['x'],data[testid]['y'])
            print("plotting %s len %d" % (testid, len(data[testid]['x'])))
            # print(index400x)
            index400x+=1

    fig400x.savefig('MultiBrSmartBridgeAuto_400x.pdf', format='pdf')

def scatterplot_MultiBrSmartBridgeAuto_500x(results):

    print "scatterplot_MultiBrSmartBridgeAuto_500x"
    data = {}

    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (s)"

    for result in results:
        if result.mode == "MultiBrSmartBridgeAuto" and 'S500' in result.id:
            if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                    if result.id not in data:
                        data[result.id] = {'x':[], 'y':[]}

                    data[result.id]['x'].append(64 - int(result.ping_info['ping1']['ttl']))
                    data[result.id]['y'].append(int(result.time_info['pingm'])/1000)

    fig500x = plt.figure(figsize=(25,15)) #figsize=(,)
    index500x = 1

    for testid in sorted(data.keys()):
        if 'S500' in testid:
            # ax = fig500x.add_subplot(nrow,ncol,index500x, title="%s, %d points" % (testid,len(data[testid]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
            ax = fig500x.add_subplot(nrow,ncol,index500x, title="%s, %d points" % (testid,len(data[testid]['x'])), xlabel=xtitle, ylabel=ytitle)
            ax.scatter(data[testid]['x'],data[testid]['y'])
            print("plotting %s len %d" % (testid, len(data[testid]['x'])))
            # print(index500x)
            index500x+=1

    fig500x.savefig('MultiBrSmartBridgeAuto_500x.pdf', format='pdf')



def scatterplot_MultiBrSmartBridgeAuto_500x_vs_delay(results):

    print "scatterplot_MultiBrSmartBridgeAuto_500x_vs_delay"
    data = {}

    ncol = 4
    nrow = 3

    xtitle = "Time befor move (s)"
    ytitle = "Unreachability Duration (s)"

    for result in results:
        if result.mode == "MultiBrSmartBridgeAuto" and 'S500' in result.id:
            if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                    if result.id not in data:
                        data[result.id] = {'xraw':[], 'yraw':[], 'x':[], 'y':[]}

                    data[result.id]['xraw'].append(int(result.start_delay))
                    data[result.id]['yraw'].append(int(result.time_info['pingm'])/1000)

    fig500x = plt.figure(figsize=(25,15)) #figsize=(,)
    index500x = 1

    plotcolor = 'r'
    plotmarker = 'o'
    plotline = '-'
    
    for testid in sorted(data.keys()):
        data[testid]['x'] = sorted(unique(data[testid]['xraw']))
        temp = [[] for i in range(len(data[testid]['x']))]
        for k in range(len(data[testid]['xraw'])):
            temp[data[testid]['x'].index(data[testid]['xraw'][k])].append(data[testid]['yraw'][k])
        data[testid]['y'] = temp
           
    ax = fig500x.add_subplot(nrow,ncol,index500x, title="Unreachability Duration vs. Move Time", xlim=(0,2400), ylim=(0,100), xlabel=xtitle, ylabel=ytitle)
    for testid in sorted(data.keys()):
        if sorted(data.keys()).index(testid) == 0:
            plotcolor = 'b'
            plotmarker = 'x'
            plotlabel = 'No extra traffic'
        elif sorted(data.keys()).index(testid) == 1:
            plotcolor = 'g'
            plotmarker = 'o'
            plotlabel = 'Self UDP collect traffic'
        elif sorted(data.keys()).index(testid) == 2:
            plotcolor = 'y'
            plotmarker = 's'
            plotlabel = 'All-node UDP collect traffic'
        else:
            plotcolor = 'r'
            plotmarker = '^'
            plotlabel = 'All-node UDP echo traffic'
        tempmean = [[] for i in range(len(data[testid]['x']))]
        tempstd = [[] for i in range(len(data[testid]['x']))]
        for i in range(len(tempmean)):
            tempmean[i] = mean(data[testid]['y'][i])
            tempstd[i] = std(data[testid]['y'][i])
        ax.plot(data[testid]['x'],tempmean, label=plotlabel, linestyle=plotline, marker=plotmarker, color=plotcolor)
        ax.errorbar(data[testid]['x'], tempmean, tempstd, fmt='-', color=plotcolor)                
        handles, labels = ax.get_legend_handles_labels()
        ax.legend(handles[::-1], labels[::-1])
        print("plotting 500x RvsD %s" % testid)
    index500x+=1

    ax = fig500x.add_subplot(nrow,ncol,index500x, title="Unreachability Duration vs. Move Time", xlim=(0,2400), ylim=(0,100), xlabel=xtitle, ylabel=ytitle)
    plotcolor = 'b'
    plotmarker = 'o'
    plotline = '-'
    temp = [[] for i in range(len(data[testid]['x']))]
    tempmean = [[] for i in range(len(data[testid]['x']))]
    tempstd = [[] for i in range(len(data[testid]['x']))]
    for testid in sorted(data.keys()):
        for i in range(len(data[testid]['y'])):
            temp[i].extend(data[testid]['y'][i])
    for i in range(len(tempmean)):
        tempmean[i] = mean(temp[i])
        tempstd[i] = std(temp[i])
    ax.plot(data[testid]['x'],tempmean, label=plotlabel, linestyle=plotline, marker=plotmarker, color=plotcolor)
    ax.errorbar(data[testid]['x'], tempmean, tempstd, fmt='-', color=plotcolor)                

    fig500x.savefig('MultiBrSmartBridgeAuto_500x_vs_delay.pdf', format='pdf')


def scatterplot_MultiBrSmartBridgeAuto_502x(results):

    print "scatterplot_MultiBrSmartBridgeAuto_502x"
    data = {}

    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (s)"

    for result in results:
        if result.mode == "MultiBrSmartBridgeAuto" and 'S502' in result.id:
            if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
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
                # ax = fig502x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                ax = fig502x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlabel=xtitle, ylabel=ytitle)
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                # print(index502x)
                # index502x+=1

    fig502x.savefig('MultiBrSmartBridgeAuto_502x.pdf', format='pdf')


def scatterplot_MultiBrSmartBridgeAuto_502x_vs_delay(results):

    print "scatterplot_MultiBrSmartBridgeAuto_502x_vs_delay"
    data = {}

    ncol = 4
    nrow = 3

    xtitle = "Time befor move (s)"
    ytitle = "Unreachability Duration (s)"

    for result in results:
        if result.mode == "MultiBrSmartBridgeAuto" and 'S502' in result.id:
            if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                    if result.id not in data:
                        data[result.id] = {'xraw':[], 'yraw':[], 'x':[], 'y':[]}

                    data[result.id]['xraw'].append(int(result.start_delay))
                    data[result.id]['yraw'].append(int(result.time_info['pingm'])/1000)

    fig500x = plt.figure(figsize=(25,15)) #figsize=(,)
    index500x = 1

    plotcolor = 'r'
    plotmarker = 'o'
    plotline = '-'
    
    for testid in sorted(data.keys()):
        data[testid]['x'] = sorted(unique(data[testid]['xraw']))
        temp = [[] for i in range(len(data[testid]['x']))]
        for k in range(len(data[testid]['xraw'])):
            temp[data[testid]['x'].index(data[testid]['xraw'][k])].append(data[testid]['yraw'][k])
        data[testid]['y'] = temp
           
    ax = fig500x.add_subplot(nrow,ncol,index500x, title="Unreachability Duration vs. Move Time", xlim=(0,2400), ylim=(0,100), xlabel=xtitle, ylabel=ytitle)
    for testid in sorted(data.keys()):
        if sorted(data.keys()).index(testid) == 0:
            plotcolor = 'b'
            plotmarker = 'x'
            plotlabel = 'No extra traffic'
        elif sorted(data.keys()).index(testid) == 1:
            plotcolor = 'g'
            plotmarker = 'o'
            plotlabel = 'Self UDP collect traffic'
        elif sorted(data.keys()).index(testid) == 2:
            plotcolor = 'y'
            plotmarker = 's'
            plotlabel = 'All-node UDP collect traffic'
        else:
            plotcolor = 'r'
            plotmarker = '^'
            plotlabel = 'All-node UDP echo traffic'
        tempmean = [[] for i in range(len(data[testid]['x']))]
        tempstd = [[] for i in range(len(data[testid]['x']))]
        for i in range(len(tempmean)):
            tempmean[i] = mean(data[testid]['y'][i])
            tempstd[i] = std(data[testid]['y'][i])
        ax.plot(data[testid]['x'],tempmean, label=plotlabel, linestyle=plotline, marker=plotmarker, color=plotcolor)
        ax.errorbar(data[testid]['x'], tempmean, tempstd, fmt='-', color=plotcolor)                
        handles, labels = ax.get_legend_handles_labels()
        ax.legend(handles[::-1], labels[::-1])
        print("plotting 502x RvsD %s" % testid)
    index500x+=1

    ax = fig500x.add_subplot(nrow,ncol,index500x, title="Unreachability Duration vs. Move Time", xlim=(0,2400), ylim=(0,100), xlabel=xtitle, ylabel=ytitle)
    plotcolor = 'b'
    plotmarker = 'o'
    plotline = '-'
    temp = [[] for i in range(len(data[testid]['x']))]
    tempmean = [[] for i in range(len(data[testid]['x']))]
    tempstd = [[] for i in range(len(data[testid]['x']))]
    for testid in sorted(data.keys()):
        for i in range(len(data[testid]['y'])):
            temp[i].extend(data[testid]['y'][i])
    for i in range(len(tempmean)):
        tempmean[i] = mean(temp[i])
        tempstd[i] = std(temp[i])
    ax.plot(data[testid]['x'],tempmean, label=plotlabel, linestyle=plotline, marker=plotmarker, color=plotcolor)
    ax.errorbar(data[testid]['x'], tempmean, tempstd, fmt='-', color=plotcolor)                

    fig500x.savefig('MultiBrSmartBridgeAuto_502x_vs_delay.pdf', format='pdf')

    
def scatterplot_MultiBrSmartBridgeAuto_600x(results):

    print "scatterplot_MultiBrSmartBridgeAuto_600x"
    data = {}

    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (s)"

    for result in results:
        if result.mode == "MultiBrSmartBridgeAuto" and 'S502' in result.id:
            if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
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
                # ax = fig600x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                ax = fig600x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlabel=xtitle, ylabel=ytitle)
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                # print(index600x)
                index600x+=1

    fig600x.savefig('MultiBrSmartBridgeAuto_600x.pdf', format='pdf')
