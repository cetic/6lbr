import re

#[1365976818.828652] 16 bytes from 8888::212:7402:2:202: icmp_seq=1 ttl=63 time=61.2 ms
re_ping_info = re.compile(".*ttl=([0-9]+) time=([0-9,\.]+) ms\n")

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


    

    
