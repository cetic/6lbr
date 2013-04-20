from pylab import *
import re
import math
import inspect
from pp_utils import *



def scatterplot_RplRootTransparentBridge_separate(results):

    print "scatterplot_RplRootTransparentBridge_separate"
    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 
    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (s)"

    for result in results:
        if result.mode == "RplRootTransparentBridge":
            if result.ping_info != None:
                if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:
                    if 'line' in result.topology:
                        topo = '-line'
                    else:
                        topo = '-other'
                    if result.id+topo not in data:
                        data[result.id+topo] = {}
                    if result.start_delay not in data[result.id+topo]:
                        data[result.id+topo][result.start_delay] = {'x':[], 'y':[]}

                    if 'ping2' in result.ping_info and result.ping_info['ping2'] != None:
                        pingnum = 'ping2'
                    else:
                        pingnum = 'ping1'

                    re_line_topo = re.compile(".*line-([0-9]+)-.*")
                    if topo == '-line':
                        data[result.id+topo][result.start_delay]['x'].append(int(re_line_topo.match(result.topology).group(1))-1)
                        data[result.id+topo][result.start_delay]['y'].append(int(result.time_info[pingnum])/1000)
                    else:
                        data[result.id+topo][result.start_delay]['x'].append(64 - int(result.ping_info[pingnum]['ttl']))
                        data[result.id+topo][result.start_delay]['y'].append(int(result.time_info[pingnum])/1000)
    
    formatter = matplotlib.ticker.EngFormatter(places=3)
    formatter.ENG_PREFIXES[-6] = 'u'
    fig100xline = plt.figure(figsize=(25,15)) #figsize=(,)
    fig200xline = plt.figure(figsize=(25,15))
    index100xline = 1
    index200xline = 1

    fig100xother = plt.figure(figsize=(25,15)) #figsize=(,)
    fig200xother = plt.figure(figsize=(25,15))
    index100xother = 1
    index200xother = 1

    fig100x = plt.figure(figsize=(25,15)) #figsize=(,)
    fig200x = plt.figure(figsize=(25,15))
    index100x = 1
    index200x = 1

    for testid in data:
        sortedid = sorted(data.keys())
        print sortedid
        for start_delay in data[testid]:
            sorteddelay = sorted(data[testid].keys())
            print sorteddelay
            if 'line' in testid:
                if 'S100' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2))) + 1
                    ax = fig100xline.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index100xline)
                    #index100xline+=1
                if 'S200' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-8 + 1
                    ax = fig200xline.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))                    
            if 'other' in testid:
                if 'S100' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-1 + 1
                    ax = fig100xother.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index100xother)
                    #index100xother+=1
                if 'S200' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-1-8 + 1
                    ax = fig200xother.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))                       

            #plt.axes().yaxis.set_major_formatter(formatter)

    fig100xline.savefig('RplRootTransparentBridge_100x_line.pdf', format='pdf')
    fig200xline.savefig('RplRootTransparentBridge_200x_line.pdf', format='pdf')

    fig100xother.savefig('RplRootTransparentBridge_100x_other.pdf', format='pdf')
    fig200xother.savefig('RplRootTransparentBridge_200x_other.pdf', format='pdf')

def scatterplot_RplRootTransparentBridge(results):

    print "scatterplot_RplRootTransparentBridge"
    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 
    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (s)"

    for result in results:
        if result.mode == "RplRootTransparentBridge":
            if result.ping_info != None:
                if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:

                    if result.id not in data:
                        data[result.id] = {}
                    if result.start_delay not in data[result.id]:
                        data[result.id][result.start_delay] = {'x':[], 'y':[]}

                    if 'ping2' in result.ping_info and result.ping_info['ping2'] != None:
                        pingnum = 'ping2'
                    else:
                        pingnum = 'ping1'

                    re_line_topo = re.compile(".*line-([0-9]+)-.*")
                    if 'line' in result.topology:
                        data[result.id][result.start_delay]['x'].append(int(re_line_topo.match(result.topology).group(1))-1)
                        data[result.id][result.start_delay]['y'].append(int(result.time_info[pingnum])/1000)
                    else:
                        data[result.id][result.start_delay]['x'].append(64 - int(result.ping_info[pingnum]['ttl']))
                        data[result.id][result.start_delay]['y'].append(int(result.time_info[pingnum])/1000)

    fig100x = plt.figure(figsize=(25,15)) #figsize=(,)
    fig200x = plt.figure(figsize=(25,15))
    index100x = 1
    index200x = 1
   
    for testid in data:
        sortedid = sorted(data.keys())
        print sortedid
        for start_delay in sorted(data[testid].keys()):
            sorteddelay = sorted(data[testid].keys())
            print sorteddelay
            if 'S100' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid) + 1
                ax = fig100x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                #print(index100x)
                #index100x+=1
            if 'S200' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid)-8 + 1
                ax = fig200x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                #print(index200x)                
#        if index100x>7:
#            index100x = index100x - 7
#        if index110x>7:
#            index110x = index110x - 7
#        if index111x>7:
#            index111x = index111x - 7
            
            #plt.axes().yaxis.set_major_formatter(formatter)

    fig100x.savefig('RplRootTransparentBridge_100x.pdf', format='pdf')
    fig200x.savefig('RplRootTransparentBridge_200x.pdf', format='pdf')




def scatterplot_RplRootTransparentBridge_mean(results):

    print "scatterplot_RplRootTransparentBridge_mean"
    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 
    ncol = 4
    nrow = 3
    lonelynesslevel = 1
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (s)"

    for result in results:
        if result.mode == "RplRootTransparentBridge":
            if result.ping_info != None:
                if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:

                    if result.id not in data:
                        data[result.id] = {}
                    if result.start_delay not in data[result.id]:
                        data[result.id][result.start_delay] = {'x':[], 'y':[], 'xmean':[], 'ymean':[], 'ystd':[]}

                    if 'ping2' in result.ping_info and result.ping_info['ping2'] != None:
                        pingnum = 'ping2'
                    else:
                        pingnum = 'ping1'

                    re_line_topo = re.compile(".*line-([0-9]+)-.*")
                    if 'line' in result.topology:
                        data[result.id][result.start_delay]['x'].append(int(re_line_topo.match(result.topology).group(1))-1)
                        data[result.id][result.start_delay]['y'].append(int(result.time_info[pingnum])/1000)
                    else:
                        data[result.id][result.start_delay]['x'].append(64 - int(result.ping_info[pingnum]['ttl']))
                        data[result.id][result.start_delay]['y'].append(int(result.time_info[pingnum])/1000)
    
    figmean100x = plt.figure(figsize=(25,15))
    figmean200x = plt.figure(figsize=(25,15))

    indexmean100x = 0
    indexmean200x = 0
    
    print "  mean"
    plotcolor = 'r'
    plotmarker = 'o'
    plotline = '-'
    for testid in sorted(data.keys()):
        if 'S100' in testid:
            indexmean100x += 1
            ax = figmean100x.add_subplot(nrow,ncol,indexmean100x, title="Mean values %s, all delays" % (testid,), xlim=(0,10), ylim=(0,65), xlabel=xtitle, ylabel=ytitle) 
            for start_delay in sorted(data[testid].keys()):
                data[testid][start_delay]['xmean'] = sorted(unique(data[testid][start_delay]['x']))
                temp = [[] for i in range(len(data[testid][start_delay]['xmean']))]
                for k in range(len(data[testid][start_delay]['x'])):
                    temp[data[testid][start_delay]['xmean'].index(data[testid][start_delay]['x'][k])].append(data[testid][start_delay]['y'][k])
                for i in range(len(temp)):
                    data[testid][start_delay]['ymean'].append(mean(temp[i]).tolist())
                    data[testid][start_delay]['ystd'].append(std(temp[i]).tolist())

                if sorted(data[testid].keys()).index(start_delay) == 0:
                    plotcolor = 'b'
                    plotmarker = 'x'
                elif sorted(data[testid].keys()).index(start_delay) == 1:
                    plotcolor = 'g'
                    plotmarker = 'o'
                else:
                    plotcolor = 'r'
                    plotmarker = '^'
                pruned = prunevalues(data[testid][start_delay]['xmean'],data[testid][start_delay]['ymean'],data[testid][start_delay]['ystd'], lonelyness=lonelynesslevel)
                ax.plot(pruned['x'],pruned['y'], label="DAG delay %ds"%int(start_delay), linestyle=plotline, marker=plotmarker, color=plotcolor)
                ax.errorbar(pruned['x'],pruned['y'], pruned['z'], fmt='-', color=plotcolor)
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles[::-1], labels[::-1])
                # ax.plot(data[testid][start_delay]['xmean'],data[testid][start_delay]['ymean'], label="DAG delay %ds"%int(start_delay), linestyle=plotline, marker=plotmarker, color=plotcolor)
                # ax.errorbar(data[testid][start_delay]['xmean'], data[testid][start_delay]['ymean'], data[testid][start_delay]['ystd'], fmt='-', color=plotcolor)
                # print("plotting mean %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['xmean'])))                
        if 'S200' in testid:
            indexmean200x += 1
            ax = figmean200x.add_subplot(nrow,ncol,indexmean200x, title="Mean values %s, all delays" % (testid,), xlim=(0,10), ylim=(0,65), xlabel=xtitle, ylabel=ytitle) 
            for start_delay in sorted(data[testid].keys()):
                data[testid][start_delay]['xmean'] = sorted(unique(data[testid][start_delay]['x']))
                temp = [[] for i in range(len(data[testid][start_delay]['xmean']))]
                for k in range(len(data[testid][start_delay]['x'])):
                    temp[data[testid][start_delay]['xmean'].index(data[testid][start_delay]['x'][k])].append(data[testid][start_delay]['y'][k])
                for i in range(len(temp)):
                    data[testid][start_delay]['ymean'].append(mean(temp[i]).tolist())
                    data[testid][start_delay]['ystd'].append(std(temp[i]).tolist())

                if sorted(data[testid].keys()).index(start_delay) == 0:
                    plotcolor = 'b'
                    plotmarker = 'x'
                elif sorted(data[testid].keys()).index(start_delay) == 1:
                    plotcolor = 'g'
                    plotmarker = 'o'
                else:
                    plotcolor = 'r'
                    plotmarker = '^'
                pruned = prunevalues(data[testid][start_delay]['xmean'],data[testid][start_delay]['ymean'],data[testid][start_delay]['ystd'], lonelyness=lonelynesslevel)
                ax.plot(pruned['x'],pruned['y'], label="DAG delay %ds"%int(start_delay), linestyle=plotline, marker=plotmarker, color=plotcolor)
                ax.errorbar(pruned['x'],pruned['y'], pruned['z'], fmt='-', color=plotcolor)
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles[::-1], labels[::-1])
                # ax.plot(data[testid][start_delay]['xmean'],data[testid][start_delay]['ymean'], label="DAG delay %ds"%int(start_delay), linestyle=plotline, marker=plotmarker, color=plotcolor)
                # ax.errorbar(data[testid][start_delay]['xmean'], data[testid][start_delay]['ymean'], data[testid][start_delay]['ystd'], fmt='-', color=plotcolor)
                # print("plotting mean %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['xmean'])))                

    figmean100x.savefig('RplRootTransparentBridge_mean100x.pdf', format='pdf')
    figmean200x.savefig('RplRootTransparentBridge_mean200x.pdf', format='pdf')

    figmeandelay100x = plt.figure(figsize=(25,15))
    figmeandelay200x = plt.figure(figsize=(25,15))
    
    figmeantraffic100x = plt.figure(figsize=(25,15))
    figmeantraffic200x = plt.figure(figsize=(25,15))

        
    #Prepare all the data
    alldata = {}
    for testid in sorted(data.keys()):
        if not alldata.has_key(testid[:-1]):
            alldata[testid[:-1]] = {}
        if not alldata[testid[:-1]].has_key('x'+testid[-1:]):
            alldata[testid[:-1]]['x'+testid[-1:]] = {}
        for start_delay in sorted(data[testid].keys()):
            if not alldata[testid[:-1]]['x'+testid[-1:]].has_key(start_delay):
                alldata[testid[:-1]]['x'+testid[-1:]][start_delay] = {}
            alldata[testid[:-1]]['x'+testid[-1:]][start_delay]["x"] = data[testid][start_delay]['xmean']
            alldata[testid[:-1]]['x'+testid[-1:]][start_delay]["ymean"] = data[testid][start_delay]['ymean']

    datadelay = {}
    datatraffic = {}
    for testclass in sorted(alldata.keys()):
        if not datadelay.has_key(testclass):
            datadelay[testclass] = {}
        if not datatraffic.has_key(testclass):
            datatraffic[testclass] = {}
        for traffic in sorted(alldata[testclass].keys()):
            for start_delay in sorted(alldata[testclass][traffic].keys()):
                if not datadelay[testclass].has_key(start_delay):
                    datadelay[testclass][start_delay] = {}
                if not datatraffic[testclass].has_key(traffic):
                    datatraffic[testclass][traffic] = {} 
                               
                if not datadelay[testclass][start_delay].has_key("x") or not datadelay[testclass][start_delay].has_key("y"):
                    tmp = mergevector(None,None,alldata[testclass][traffic][start_delay]["x"],alldata[testclass][traffic][start_delay]["ymean"])
                else:
                    tmp = mergevector(datadelay[testclass][start_delay]["x"],datadelay[testclass][start_delay]["y"],alldata[testclass][traffic][start_delay]["x"],alldata[testclass][traffic][start_delay]["ymean"])
                datadelay[testclass][start_delay]["x"] = tmp["x"]
                datadelay[testclass][start_delay]["y"] = tmp["y"]

                if not datatraffic[testclass][traffic].has_key("x") or not datatraffic[testclass][traffic].has_key("y"):
                    tmp = mergevector(None,None,alldata[testclass][traffic][start_delay]["x"],alldata[testclass][traffic][start_delay]["ymean"])
                else:
                    tmp = mergevector(datatraffic[testclass][traffic]["x"],datatraffic[testclass][traffic]["y"],alldata[testclass][traffic][start_delay]["x"],alldata[testclass][traffic][start_delay]["ymean"])
                datatraffic[testclass][traffic]["x"] = tmp["x"]
                datatraffic[testclass][traffic]["y"] = tmp["y"]

    indexmean100x = 0
    indexmean200x = 0
    
    print "  meandelay"
    
    for testclass in sorted(datadelay.keys()):
        if 'S100' in testclass:
            indexmean100x += 1
            ax = figmeandelay100x.add_subplot(nrow,ncol,indexmean100x, title="Mean values %s, mixed traffic by delay" % (testclass,), xlim=(0,10), ylim=(0,65), xlabel=xtitle, ylabel=ytitle)
            for start_delay in sorted(datadelay[testclass].keys()):
                if sorted(datadelay[testclass].keys()).index(start_delay) == 0:
                    plotcolor = 'b'
                    plotmarker = 'x'
                elif sorted(datadelay[testclass].keys()).index(start_delay) == 1:
                    plotcolor = 'g'
                    plotmarker = 'o'
                else:
                    plotcolor = 'r'
                    plotmarker = '^'
                pruned = prunevalues(datadelay[testclass][start_delay]['x'],datadelay[testclass][start_delay]['y'], lonelyness=lonelynesslevel)
                ax.plot(pruned['x'],mean(pruned['y'],0), label="DAG delay %ds"%int(start_delay), linestyle=plotline, marker=plotmarker, color=plotcolor)
                ax.errorbar(pruned['x'], mean(pruned['y'],0), std(pruned['y'],0), fmt='-', color=plotcolor)                
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles[::-1], labels[::-1])
                # ax.plot(datadelay[testclass][start_delay]['x'],mean(datadelay[testclass][start_delay]['y'],0), label="DAG delay %ds"%int(start_delay), linestyle=plotline, marker=plotmarker, color=plotcolor)
                # ax.errorbar(datadelay[testclass][start_delay]['x'], mean(datadelay[testclass][start_delay]['y'],0), std(datadelay[testclass][start_delay]['y'],0), fmt='-', color=plotcolor)                
        if 'S200' in testclass:
            indexmean200x += 1
            ax = figmeandelay200x.add_subplot(nrow,ncol,indexmean200x, title="Mean values %s, mixed traffic by delay" % (testclass,), xlim=(0,10), ylim=(0,65), xlabel=xtitle, ylabel=ytitle)
            for start_delay in sorted(datadelay[testclass].keys()):
                if sorted(datadelay[testclass].keys()).index(start_delay) == 0:
                    plotcolor = 'b'
                    plotmarker = 'x'
                elif sorted(datadelay[testclass].keys()).index(start_delay) == 1:
                    plotcolor = 'g'
                    plotmarker = 'o'
                else:
                    plotcolor = 'r'
                    plotmarker = '^'
                pruned = prunevalues(datadelay[testclass][start_delay]['x'],datadelay[testclass][start_delay]['y'], lonelyness=lonelynesslevel)
                ax.plot(pruned['x'],mean(pruned['y'],0), label="DAG delay %ds"%int(start_delay), linestyle=plotline, marker=plotmarker, color=plotcolor)
                ax.errorbar(pruned['x'], mean(pruned['y'],0), std(pruned['y'],0), fmt='-', color=plotcolor)                
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles[::-1], labels[::-1])
                # ax.plot(datadelay[testclass][start_delay]['x'],mean(datadelay[testclass][start_delay]['y'],0), label="DAG delay %ds"%int(start_delay), linestyle=plotline, marker=plotmarker, color=plotcolor)
                # ax.errorbar(datadelay[testclass][start_delay]['x'], mean(datadelay[testclass][start_delay]['y'],0), std(datadelay[testclass][start_delay]['y'],0), fmt='-', color=plotcolor)                

    figmeandelay100x.savefig('RplRootTransparentBridge_meandelay100x.pdf', format='pdf')
    figmeandelay200x.savefig('RplRootTransparentBridge_meandelay200x.pdf', format='pdf')

    indexmean100x = 0
    indexmean200x = 0
    
    print "  meantraffic"
    
    for testclass in sorted(datatraffic.keys()):
        if 'S100' in testclass:
            indexmean100x += 1
            ax = figmeantraffic100x.add_subplot(nrow,ncol,indexmean100x, title="Mean values %s, mixed delay by traffic" % (testclass,), xlim=(0,10), ylim=(0,65), xlabel=xtitle, ylabel=ytitle)
            for traffic in sorted(datatraffic[testclass].keys()):
                if sorted(datatraffic[testclass].keys()).index(traffic) == 0:
                    plotcolor = 'b'
                    plotmarker = 'x'
                    plotlabel = 'No extra traffic'
                elif sorted(datatraffic[testclass].keys()).index(traffic) == 1:
                    plotcolor = 'g'
                    plotmarker = 'o'
                    plotlabel = 'Self UDP collect traffic'
                elif sorted(datatraffic[testclass].keys()).index(traffic) == 2:
                    plotcolor = 'y'
                    plotmarker = 's'
                    plotlabel = 'All-node UDP collect traffic'
                else:
                    plotcolor = 'r'
                    plotmarker = '^'
                    plotlabel = 'All-node UDP echo traffic'
                pruned = prunevalues(datatraffic[testclass][traffic]['x'],datatraffic[testclass][traffic]['y'], lonelyness=lonelynesslevel)
                ax.plot(pruned['x'],mean(pruned['y'],0), label=plotlabel, linestyle=plotline, marker=plotmarker, color=plotcolor)
                ax.errorbar(pruned['x'], mean(pruned['y'],0), std(pruned['y'],0), fmt='-', color=plotcolor)                
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles[::-1], labels[::-1])
                # ax.plot(datatraffic[testclass][traffic]['x'],mean(datatraffic[testclass][traffic]['y'],0), label="Traffic %s"%traffic, linestyle=plotline, marker=plotmarker, color=plotcolor)
                # ax.errorbar(datatraffic[testclass][traffic]['x'], mean(datatraffic[testclass][traffic]['y'],0), std(datatraffic[testclass][traffic]['y'],0), fmt='-', color=plotcolor)                
        if 'S200' in testclass:
            indexmean200x += 1
            ax = figmeantraffic200x.add_subplot(nrow,ncol,indexmean200x, title="Mean values %s, mixed delay by traffic" % (testclass,), xlim=(0,10), ylim=(0,65), xlabel=xtitle, ylabel=ytitle)
            for traffic in sorted(datatraffic[testclass].keys()):
                if sorted(datatraffic[testclass].keys()).index(traffic) == 0:
                    plotcolor = 'b'
                    plotmarker = 'x'
                    plotlabel = 'No extra traffic'
                elif sorted(datatraffic[testclass].keys()).index(traffic) == 1:
                    plotcolor = 'g'
                    plotmarker = 'o'
                    plotlabel = 'Self UDP collect traffic'
                elif sorted(datatraffic[testclass].keys()).index(traffic) == 2:
                    plotcolor = 'y'
                    plotmarker = 's'
                    plotlabel = 'All-node UDP collect traffic'
                else:
                    plotcolor = 'r'
                    plotmarker = '^'
                    plotlabel = 'All-node UDP echo traffic'
                pruned = prunevalues(datatraffic[testclass][traffic]['x'],datatraffic[testclass][traffic]['y'], lonelyness=lonelynesslevel)
                ax.plot(pruned['x'],mean(pruned['y'],0), label=plotlabel, linestyle=plotline, marker=plotmarker, color=plotcolor)
                ax.errorbar(pruned['x'], mean(pruned['y'],0), std(pruned['y'],0), fmt='-', color=plotcolor)                
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles[::-1], labels[::-1])
                # ax.plot(datatraffic[testclass][traffic]['x'],mean(datatraffic[testclass][traffic]['y'],0), label="Traffic %s"%traffic, linestyle=plotline, marker=plotmarker, color=plotcolor)
                # ax.errorbar(datatraffic[testclass][traffic]['x'], mean(datatraffic[testclass][traffic]['y'],0), std(datatraffic[testclass][traffic]['y'],0), fmt='-', color=plotcolor)                

    figmeantraffic100x.savefig('RplRootTransparentBridge_meantraffic100x.pdf', format='pdf')
    figmeantraffic200x.savefig('RplRootTransparentBridge_meantraffic200x.pdf', format='pdf')

