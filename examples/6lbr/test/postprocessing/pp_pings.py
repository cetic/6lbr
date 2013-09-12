import re
import pickle

#[1365976818.828652] 16 bytes from 8888::212:7402:2:202: icmp_seq=1 ttl=63 time=61.2 ms
re_ping_info = re.compile(".*ttl=([0-9]+) time=([0-9,\.]+) ms\n")
re_log_time = re.compile("(\d+):(\d+):.*")

def parse_ping(pingfilepath):
    pingfile = open(pingfilepath, 'r')
    pingdata = pingfile.readlines()
    pingfile.close()
    line_success = -1
    for index, line in enumerate(pingdata):
        if "1 received" in line:
            line_success = index
            break
    if line_success > 2:
        match_ping = re_ping_info.match(pingdata[line_success-3])
        if match_ping:
            ping_ttl = match_ping.group(1)
            ping_rtt = match_ping.group(2)
            info = {'ttl':ping_ttl, 'rtt':ping_rtt}
            return info
    return None

    
def parse_pingmote(pingmotefilepath):
    testlog = pingmotefilepath[0]
    br0pkl = pingmotefilepath[1]
    
    pingstart = None
    pingstop = None
    moteid = None
    with open(testlog) as f:
        for line in f:
            if "Sending ping..." in line:
                match_log_time = re_log_time.match(line)
                if match_log_time:
                    pingstart = match_log_time.group(1)
                    moteid = match_log_time.group(2)
            elif "Received an icmp6 echo reply" in line:
                match_log_time = re_log_time.match(line)
                if match_log_time:
                    pingstop = match_log_time.group(1)
    if pingstart is None or pingstop is None:
        return None
    else:
        brcap = pickle.load(open(br0pkl,"rb"))
        donothing = True
        for packet in brcap:
            if "echo_request" in packet['type']:
                donothing = False
                ping_rtt = (float(pingstop)-float(pingstart))/1000.0
                ping_ttl = packet['hoplimit']
                info = {'ttl':ping_ttl, 'rtt':ping_rtt}
                return info
        if donothing:
            return None


