var platform = "sky"

// load(../test/lib.js)
var prefix = "bbbb";
var brID = 1;

//Wait until all mote started
function WaitingStarting() {
    var allm = sim.getMotes();
    for(var id in  allm) {
        YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki>") != -1);
    }
}

// Display all mote Table
function displayAllTable(){
    var allm = sim.getMotes();
    for(var id in  allm) {
	    write(allm[id], "netd nc");
	    YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki>") != -1);
	    write(allm[id], "netd rt");
	    YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki>") != -1);
        write(allm[id], "netd cp");
        YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki>") != -1);
        if(allm[id].getID() == brID) {
            write(allm[id], "netd dad");
            YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki>") != -1);
        }
	}
}

// Add to routing table
function genip(num, pref){
    var ip = "";
    ip += pref;
    for(var i=0; i<3; i++){
        ip +=":0000";
    }
    // Platform Wismote
    if(platform == "wismote") {
        ip += ":0200:0000:0000:"
        hex = num.toString(16);
        for(var i=0; i<4-hex.length; i++) ip += "0"
        ip += hex;
    // Platform Sky
    } else {
        var n, hex;
        ip += ":0212";
        n = 0x7400 + num;
        ip += ":"+n.toString(16);
        n = 0x0 + num;
        hex = n.toString(16);
        ip += ":00"+(hex.length==1 ? '0' : '')+hex;
        n = (0x100 * num) + num;
        hex = n.toString(16);
        ip += ":"+(hex.length==3 ? '0' : '')+hex;
    }
    return ip;
}
function gpip(num) { return genip(num, prefix); }
function llip(num) { return genip(num, "fe80"); }
function genpref(pref){
    var ip = "";
    ip += pref;
    var n = pref.split(":");
    var r = 8 - n.length;
    for(var i=0; i<r; i++){
        ip +=":0000";
    }
    return ip;
}
function addroute(moteID, to, nexthop, len) {
    mote = sim.getMoteWithID(moteID);
    var cmd = "route -a "+gpip(to)+" "+llip(nexthop)+" "+len;
    log.log("MOTE " +moteID + "->" + cmd + "\n");
    write(mote, cmd);
}


//Waiting configurate of all mote was done
function waitingConfig(){
    var lastid = 0;
    for(var i=0;; i++) {
        YIELD_THEN_WAIT_UNTIL(msg.contains("#s") || msg.contains("#r") || msg.contains("timeout"));
        if(msg.contains("#s") || msg.contains("#r")){
            GENERATE_MSG(60000, "timeout"+lastid);
            lastid++;
        }else if(msg.equals("timeout"+(lastid-1))) {
            return;
        }
    }
}

//Send UDP packet from - to (number of mote)
function sendudp(from, to) {
    mote = sim.getMoteWithID(from);
    var cmd = "sendudp " + gpip(to);
    log.log("mote "+from+" -> "+cmd+"\n");
    write(mote, cmd);
    GENERATE_MSG(5000, "timeout"); //fail when timout (5s)
    YIELD_THEN_WAIT_UNTIL(msg.contains("DATA recv 'Hello") || msg.contains("timeout"));
    if(msg.contains("timeout")) {
        log.testFailed();
    }   
}

//Send UDP packet to the border router
function sendudpBR(br) {
    var allm = sim.getMotes();
    for(var i in  allm) {
        var id = allm[i].getID();
        if(id != br) {
            sendudp(id, br);
            sendudp(br, id);
        }
    }
}

//Build routing table when NA is received 
function buildRT(s) {
    if(!s) return;
    while(true) {
	    YIELD_THEN_WAIT_UNTIL(msg.contains("#rNA"));
	    var from = parseInt(msg.split(" ")[1].split("::")[1].split(":")[2],16);
	    var alldone = true;
	    for(var i in s){
	        var v = s[i];
	        if(v.mote == from) {
                if(v.fct){
                    v.fct();
                }
                if(v.eval){
                    eval(v.eval);
                }
	            v.done = true;    
	        }
	        if(!v.done) alldone = false;
	    }
	    if(alldone) return;
    }
}
// endload()

function generate_RT_formated(){
    var rt = [];
    var numMote = sim.getMotes().length;
    if(numMote<3) return false;
    for(var mote_i=2; mote_i<numMote; mote_i++) {
        var ev = "";
        for(var mote_to=mote_i+1; mote_to<=numMote; mote_to++) {
            ev += "addroute("+(mote_i-1)+","+mote_to+","+mote_i+",128);";
            if(mote_to<numMote) {
                ev += 'GENERATE_MSG(25, "continue'+mote_to+'");';
                ev += 'WAIT_UNTIL(msg.contains("continue'+mote_to+'"));';
            }
        }
        rt.push({"mote":mote_i, "eval":ev})
    }
    return rt;  
}

function analysis(){
    //ANALYSIS
    var json = "{"

    //timing of convergence
    json += '"time":'+(sim.getSimulationTimeMillis()-75000);

    msg_in_out = {"ip":[0,0,0,0], "nd6":[0,0]}
    var allm = sim.getMotes();
    for(var id in  allm) {
        write(allm[id], "stats");
        YIELD_THEN_WAIT_UNTIL(msg.contains("|"));
        msg_type = msg.split('|');
        msg_ip = msg_type[0].split(",");
        for(var i in msg_ip) {
            msg_in_out["ip"][i] += parseInt(msg_ip[i]);
        }
        msg_nd6 = msg_type[1].split(",");
        for(var i in msg_nd6) {
            msg_in_out["nd6"][i] += parseInt(msg_nd6[i]);
        }
    }
    json += ',"msg":{';
    json += '"ip":['+msg_in_out["ip"]+']';
    json += ',"nd6":['+msg_in_out["nd6"]+']';
    json += "}";

    json += "}\n";

    return json;
}


//Display NC when changement of msg was done
TIMEOUT(18000000); //5h
WaitingStarting();

//Routing Table
log.log("Modify RT\n");
rt = generate_RT_formated();
buildRT(rt);
log.log("- end modify RT -");

// Waiting topology stable
waitingConfig();
log.log("Topology stable\n");
// Analysis
log.log("#start->"+sim.getMotes().length+":"+analysis());


//Simulation of 1 hour
GENERATE_MSG(3600000, "continue1h");
WAIT_UNTIL(msg.contains("continue1h"));
log.log("#1hour->"+sim.getMotes().length+":"+analysis());


log.testOK();