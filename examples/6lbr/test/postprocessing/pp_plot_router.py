from pylab import *
import re
import math
import inspect

def scatterplot_Router_separate(results):

    print "scatterplot_Router_separate"
    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 
    ncol = 4
    nrow = 3
    
    xtitle = "Hop Count"
    ytitle = "Reach Delay (ms)"

    for result in results:
        print "%s - %s" % (result.mode,result.id)
        if result.mode == "Router":
            print "Router!!!"
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
                else:
                    print "No ping1 or none"
            else:
                print "No ping info"
            
    
    formatter = matplotlib.ticker.EngFormatter(places=3)
    formatter.ENG_PREFIXES[-6] = 'u'
    fig100xline = plt.figure(figsize=(25,15)) #figsize=(,)
    fig200xline = plt.figure(figsize=(25,15))
    index100xline = 1
    index110xline = 1
    index111xline = 1

    fig100xother = plt.figure(figsize=(25,15)) #figsize=(,)
    fig200xother = plt.figure(figsize=(25,15))
    index100xother = 1
    index110xother = 1
    index111xother = 1

    fig100x = plt.figure(figsize=(25,15)) #figsize=(,)
    fig200x = plt.figure(figsize=(25,15))
    index100x = 1
    index110x = 1
    index111x = 1

    for testid in data:
        sortedid = sorted(data.keys())
        # print sortedid
        for start_delay in data[testid]:
            sorteddelay = sorted(data[testid].keys())
            # print sorteddelay
            if 'line' in testid:
                if 'S100' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2))) + 1
                    ax = fig100xline.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    #print(index100xline)
                    #index100xline+=1
                if 'S200' in testid:
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-4 + 1
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
                    idx = sorteddelay.index(start_delay)*ncol + int(math.ceil(float(sortedid.index(testid))/float(2)))-1-4 + 1
                    ax = fig200xother.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle)
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))                     

            #plt.axes().yaxis.set_major_formatter(formatter)

    fig100xline.savefig('Router_100x_line.pdf', format='pdf')
    fig200xline.savefig('Router_200x_line.pdf', format='pdf')

    fig100xother.savefig('Router_100x_other.pdf', format='pdf')
    fig200xother.savefig('Router_200x_other.pdf', format='pdf')

def scatterplot_Router(results):

    print "scatterplot_Router"
    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 
    ncol = 4
    nrow = 3

    xtitle = "Hop Count"
    ytitle = "Reach Delay (ms)"
    
    for result in results:
        if result.mode == "Router":
            if result.ping_info != None:
                if 'ping1' in result.ping_info or 'ping2' in result.ping_info:
                    if result.ping_info['ping1'] != None or ('ping2' in result.ping_info and result.ping_info['ping2'] != None):

                        if result.id not in data:
                            data[result.id] = {}
                        if result.start_delay not in data[result.id]:
                            data[result.id][result.start_delay] = {'x':[], 'y':[]}

                        if 'ping2' in result.ping_info and result.ping_info['ping2'] != None:
                            pingnum = 'ping2'
                        elif result.ping_info['ping1'] != None:
                            pingnum = 'ping1'
                        else:
                            continue

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
   
    for testid in data:
        sortedid = sorted(data.keys())
        # print sortedid
        for start_delay in sorted(data[testid].keys()):
            sorteddelay = sorted(data[testid].keys())
            # print sorteddelay
            if 'S100' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid) + 1
                ax = fig100x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                #print(index100x)
                #index100x+=1
            if 'S200' in testid:
                idx = sorteddelay.index(start_delay)*ncol + sortedid.index(testid)-4 + 1
                ax = fig200x.add_subplot(nrow,ncol,idx, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80), xlabel=xtitle, ylabel=ytitle) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))                
            
            #plt.axes().yaxis.set_major_formatter(formatter)

    fig100x.savefig('Router_100x.pdf', format='pdf')
    fig200x.savefig('Router_200x.pdf', format='pdf')

