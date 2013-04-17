from pylab import *
import re

def scatterplot_MultiBrSmartBridgeAuto(results):

    data = {}

    for result in results:
        if result.mode == "MultiBrSmartBridgeAuto":
            if 'pingm' in result.time_info and result.time_info['pingm'] > -1:
                if result.id not in data:
                    data[result.id] = {}
                if result.start_delay not in data[result.id]:
                    data[result.id][result.start_delay] = {'x':[], 'y':[]}

                data[result.id][result.start_delay]['x'].append(64 - int(result.ping_info['ping0']['ttl']))
                data[result.id][result.start_delay]['y'].append(int(result.time_info['pingm'])/1000)

    fig500x = plt.figure(figsize=(25,15)) #figsize=(,)
    fig502x = plt.figure(figsize=(25,15))
    index500x = 1
    index502x = 1

    for testid in data:
        for start_delay in data[testid]:
            if 'S500' in testid:
                ax = fig500x.add_subplot(3,4,index500x, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))#, xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                print(index500x)
                index500x+=1
            if 'S502' in testid:
                ax = fig502x.add_subplot(3,4,index502x, title="%s-%s, %d points" % (testid,start_delay,len(data[testid][start_delay]['x'])))#, xlim=(0,10), ylim=(0,80)) 
                ax.scatter(data[testid][start_delay]['x'],data[testid][start_delay]['y'])
                print("plotting %s %s len %d" % (testid, start_delay, len(data[testid][start_delay]['x'])))
                print(index502x)
                index502x+=1

            
            #plt.axes().yaxis.set_major_formatter(formatter)

    fig500x.savefig('MultiBrSmartBridgeAuto_500x.pdf', format='pdf')
    fig502x.savefig('MultiBrSmartBridgeAuto_502x.pdf', format='pdf')

