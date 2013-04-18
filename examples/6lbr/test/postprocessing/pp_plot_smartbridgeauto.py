from pylab import *
import re
import math
import inspect

def scatterplot_SmartBridgeAuto_separate(results):

    print "scatterplot_SmartBridgeAuto_separate"
    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 
    ncol = 4
    nrow = 3

    for result in results:
        if "SmartBridgeAuto" in result.mode:
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
    fig110xline = plt.figure(figsize=(25,15))
    fig111xline = plt.figure(figsize=(25,15))
    fig200xline = plt.figure(figsize=(25,15))
    index100xline = 1
    index110xline = 1
    index111xline = 1

    fig100xother = plt.figure(figsize=(25,15)) #figsize=(,)
    fig110xother = plt.figure(figsize=(25,15))
    fig111xother = plt.figure(figsize=(25,15))
    fig200xother = plt.figure(figsize=(25,15))
    index100xother = 1
    index110xother = 1
    index111xother = 1

    fig100x = plt.figure(figsize=(25,15)) #figsize=(,)
    fig110x = plt.figure(figsize=(25,15))
    fig111x = plt.figure(figsize=(25,15))
    fig200x = plt.figure(figsize=(25,15))
    index100x = 1
    index110x = 1
    index111x = 1

    for testid in data:
        sortedid = sorted(data.keys())
        print sortedid
        for start_delay in data[testid]:
            sorteddelay = sorted(data[testid].keys())
            print sorteddelay
            if 'line' in testid:
                if 'S100' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2))) + 1
                    ax = fig100xline.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index100xline)
                    #index100xline+=1
                if 'S110' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2))) + 1
                    ax = fig110xline.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    #print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index110xline)
                    index110xline+=1
                if 'S111' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-4 + 1
                    ax = fig111xline.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index111xline)
                    #index111xline+=1
                if 'S200' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-8 + 1
                    ax = fig200xline.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))                     
            if 'other' in testid:
                if 'S100' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-1 + 1
                    ax = fig100xother.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index100xother)
                    #index100xother+=1
                if 'S110' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-1 + 1
                    ax = fig110xother.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index110xother)
                    #index110xother+=1
                if 'S111' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-1-4 + 1
                    ax = fig111xother.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index111xother)
                    #index111xother+=1
                if 'S200' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-1-8 + 1
                    ax = fig200xgrid.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))                     

            #plt.axes().yaxis.set_major_formatter(formatter)

    fig100xline.savefig('SmartBridgeAuto_100x_line.pdf', format='pdf')
    fig110xline.savefig('SmartBridgeAuto_110x_line.pdf', format='pdf')
    fig111xline.savefig('SmartBridgeAuto_111x_line.pdf', format='pdf')
    fig200xline.savefig('SmartBridgeAuto_200x_line.pdf', format='pdf')

    fig100xother.savefig('SmartBridgeAuto_100x_other.pdf', format='pdf')
    fig110xother.savefig('SmartBridgeAuto_110x_other.pdf', format='pdf')
    fig111xother.savefig('SmartBridgeAuto_111x_other.pdf', format='pdf')
    fig200xother.savefig('SmartBridgeAuto_200x_other.pdf', format='pdf')

def scatterplot_SmartBridgeAuto(results):

    print "scatterplot_SmartBridgeAuto"
    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 
    ncol = 4
    nrow = 3

    for result in results:
        if "SmartBridgeAuto" in result.mode:
            if result.ping_info != None:
                if 'ping1' in result.ping_info and result.ping_info['ping1'] != None:

                    if result.id not in data:
                        data[result.id] = {}
                    if result.start_delay not in data[result.id]:
                        data[result.id][result.start_delay] = {'x':[], 'y':[], 'xmean':[], 'ymean':[]}

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
    fig110x = plt.figure(figsize=(25,15))
    fig111x = plt.figure(figsize=(25,15))
    fig200x = plt.figure(figsize=(25,15))
    index100x = 1
    index110x = 1
    index111x = 1
    
    figmean100x = plt.figure(figsize=(25,15))
    figmean110x = plt.figure(figsize=(25,15))
    figmean111x = plt.figure(figsize=(25,15))
    figmean200x = plt.figure(figsize=(25,15))
    indexmean100x = 0
    indexmean110x = 0
    indexmean111x = 0

    for testid in data:
        sortedid = sorted(data.keys())
        print sortedid
        for start_delay in data[testid]:
            sorteddelay = sorted(data[testid].keys())
            print sorteddelay
            data[testid][start_delay]['mean'] = mean(data[testid][start_delay]['y'])
            if 'S100' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid) + 1
                ax = fig100x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                #print(index100x)
                #index100x+=1
            if 'S110' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid) + 1
                ax = fig110x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                #print(index110x)
                #index110x+=1
            if 'S111' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid)-4 + 1
                ax = fig111x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                #print(index111x)
                #index111x+=1
            if 'S200' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid)-8 + 1
                ax = fig200x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))                

    style = 'ro-'
    for testid in sorted(data.keys()):
        if 'S100' in testid:
            indexmean100x += 1
            ax = figmean100x.add_subplot(nrow,ncol,indexmean100x, title="Mean values %s, all delays" % (testid,), xlim=(0,10), ylim=(0,80)) 
            for start_delay in sorted(data[testid].keys()):
                data[testid][start_delay]['xmean'] = sorted(unique(data[testid][start_delay]['x']))
                temp = [[] for i in range(len(data[testid][start_delay]['xmean']))]
                for k in range(len(data[testid][start_delay]['x'])):
                    temp[data[testid][start_delay]['xmean'].index(data[testid][start_delay]['x'][k])].append(data[testid][start_delay]['y'][k])
                for i in range(len(temp)):
                    temp[i] = mean(temp[i])
                data[testid][start_delay]['ymean'] = temp 
                if sorted(data[testid].keys()).index(start_delay) == 0:
                    style = 'bo-'
                elif sorted(data[testid].keys()).index(start_delay) == 1:
                    style = 'go-'
                else:
                    style = 'ro-'
                ax.plot(data[testid][start_delay]['xmean'],data[testid][start_delay]['ymean'],style)
        elif 'S110' in testid:
            indexmean110x += 1
            ax = figmean110x.add_subplot(nrow,ncol,indexmean110x, title="Mean values %s, all delays" % (testid,), xlim=(0,10), ylim=(0,80)) 
            for start_delay in sorted(data[testid].keys()):
                data[testid][start_delay]['xmean'] = sorted(unique(data[testid][start_delay]['x']))
                temp = [[] for i in range(len(data[testid][start_delay]['xmean']))]
                for k in range(len(data[testid][start_delay]['x'])):
                    temp[data[testid][start_delay]['xmean'].index(data[testid][start_delay]['x'][k])].append(data[testid][start_delay]['y'][k])
                for i in range(len(temp)):
                    temp[i] = mean(temp[i])
                data[testid][start_delay]['ymean'] = temp
                if sorted(data[testid].keys()).index(start_delay) == 0:
                    style = 'bo-'
                elif sorted(data[testid].keys()).index(start_delay) == 1:
                    style = 'go-'
                else:
                    style = 'ro-'
                ax.plot(data[testid][start_delay]['xmean'],data[testid][start_delay]['ymean'],style)
        if 'S111' in testid:
            indexmean111x += 1
            ax = figmean111x.add_subplot(nrow,ncol,indexmean111x, title="Mean values %s, all delays" % (testid,), xlim=(0,10), ylim=(0,80)) 
            for start_delay in sorted(data[testid].keys()):
                data[testid][start_delay]['xmean'] = sorted(unique(data[testid][start_delay]['x']))
                temp = [[] for i in range(len(data[testid][start_delay]['xmean']))]
                for k in range(len(data[testid][start_delay]['x'])):
                    temp[data[testid][start_delay]['xmean'].index(data[testid][start_delay]['x'][k])].append(data[testid][start_delay]['y'][k])
                for i in range(len(temp)):
                    temp[i] = mean(temp[i])
                data[testid][start_delay]['ymean'] = temp
                if sorted(data[testid].keys()).index(start_delay) == 0:
                    style = 'bo-'
                elif sorted(data[testid].keys()).index(start_delay) == 1:
                    style = 'go-'
                else:
                    style = 'ro-'
                ax.plot(data[testid][start_delay]['xmean'],data[testid][start_delay]['ymean'],style)
                
            
            #plt.axes().yaxis.set_major_formatter(formatter)

    fig100x.savefig('SmartBridgeAuto_100x.pdf', format='pdf')
    fig110x.savefig('SmartBridgeAuto_110x.pdf', format='pdf')
    fig111x.savefig('SmartBridgeAuto_111x.pdf', format='pdf')
    fig200x.savefig('SmartBridgeAuto_200x.pdf', format='pdf')

    figmean100x.savefig('SmartBridgeAuto_mean100x.pdf', format='pdf')
    figmean110x.savefig('SmartBridgeAuto_mean110x.pdf', format='pdf')
    figmean111x.savefig('SmartBridgeAuto_mean111x.pdf', format='pdf')

