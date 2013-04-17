from pylab import *
import re

def scatterplot_SmartBridgeAuto_seperate(results):

    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 

    for result in results:
        if result.mode == "SmartBridgeAuto":
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
    index100xline = 1
    index110xline = 1
    index111xline = 1

    fig100xother = plt.figure(figsize=(25,15)) #figsize=(,)
    fig110xother = plt.figure(figsize=(25,15))
    fig111xother = plt.figure(figsize=(25,15))
    index100xother = 1
    index110xother = 1
    index111xother = 1

    fig100x = plt.figure(figsize=(25,15)) #figsize=(,)
    fig110x = plt.figure(figsize=(25,15))
    fig111x = plt.figure(figsize=(25,15))
    index100x = 1
    index110x = 1
    index111x = 1

    for testid in data:
        for start_delay in data[testid]:
            if 'line' in testid:
                if 'S100' in testid:
                    ax = fig100xline.add_subplot(3,4,index100xline, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    print(index100xline)
                    index100xline+=1
                if 'S110' in testid:
                    ax = fig110xline.add_subplot(3,4,index110xline, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    print(index110xline)
                    index110xline+=1
                if 'S111' in testid:
                    ax = fig111xline.add_subplot(3,4,index111xline, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    print(index111xline)
                    index111xline+=1
            if 'other' in testid:
                if 'S100' in testid:
                    ax = fig100xother.add_subplot(3,4,index100xother, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    print(index100xother)
                    index100xother+=1
                if 'S110' in testid:
                    ax = fig110xother.add_subplot(3,4,index110xother, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    print(index110xother)
                    index110xother+=1
                if 'S111' in testid:
                    ax = fig111xother.add_subplot(3,4,index111xother, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))
                    ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                    print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                    print(index111xother)
                    index111xother+=1

            #plt.axes().yaxis.set_major_formatter(formatter)

    fig100xline.savefig('SmartBridgeAuto_100x_line.pdf', format='pdf')
    fig110xline.savefig('SmartBridgeAuto_110x_line.pdf', format='pdf')
    fig111xline.savefig('SmartBridgeAuto_111x_line.pdf', format='pdf')

    fig100xother.savefig('SmartBridgeAuto_100x_other.pdf', format='pdf')
    fig110xother.savefig('SmartBridgeAuto_110x_other.pdf', format='pdf')
    fig111xother.savefig('SmartBridgeAuto_111x_other.pdf', format='pdf')

def scatterplot_SmartBridgeAuto(results):

    data = {}
    #dictionary data['Sxxxx']['delay'] = {'x':[], 'y',[]}

    results = sorted(results, key=lambda k: k.topology) 

    for result in results:
        if result.mode == "SmartBridgeAuto":
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
    fig110x = plt.figure(figsize=(25,15))
    fig111x = plt.figure(figsize=(25,15))
    index100x = 1
    index110x = 1
    index111x = 1

    for testid in data:
        for start_delay in data[testid]:
            if 'S100' in testid:
                ax = fig100x.add_subplot(3,4,index100x, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                print(index100x)
                index100x+=1
            if 'S110' in testid:
                ax = fig110x.add_subplot(3,4,index110x, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                print(index110x)
                index110x+=1
            if 'S111' in testid:
                ax = fig111x.add_subplot(3,4,index111x, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])), xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                print(index111x)
                index111x+=1
            
            #plt.axes().yaxis.set_major_formatter(formatter)

    fig100x.savefig('SmartBridgeAuto_100x.pdf', format='pdf')
    fig110x.savefig('SmartBridgeAuto_110x.pdf', format='pdf')
    fig111x.savefig('SmartBridgeAuto_111x.pdf', format='pdf')

