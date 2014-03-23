<?xml version="1.0" encoding="UTF-8"?>
<simconf>
  <project EXPORT="discard">[APPS_DIR]/mrm</project>
  <project EXPORT="discard">[APPS_DIR]/mspsim</project>
  <project EXPORT="discard">[APPS_DIR]/avrora</project>
  <project EXPORT="discard">[APPS_DIR]/serial_socket</project>
  <project EXPORT="discard">[APPS_DIR]/collect-view</project>
  <project EXPORT="discard">[APPS_DIR]/powertracker</project>
  <simulation>
    <title>6LoWPAN-ND</title>
    <randomseed>123456</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
      org.contikios.cooja.radiomediums.UDGM
      <transmitting_range>50.0</transmitting_range>
      <interference_range>100.0</interference_range>
      <success_ratio_tx>1.0</success_ratio_tx>
      <success_ratio_rx>1.0</success_ratio_rx>
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      org.contikios.cooja.mspmote.SkyMoteType
      <identifier>sky1</identifier>
      <description>Host</description>
      <source EXPORT="discard">[CONTIKI_DIR]/examples/6lowpan-nd/6lh/ex-6lowpannd-6lh.c</source>
      <commands EXPORT="discard">make ex-6lowpannd-6lh.sky TARGET=sky</commands>
      <firmware EXPORT="copy">[CONTIKI_DIR]/examples/6lowpan-nd/6lh/ex-6lowpannd-6lh.sky</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyFlash</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>
    <motetype>
      org.contikios.cooja.mspmote.SkyMoteType
      <identifier>sky2</identifier>
      <description>Router</description>
      <source EXPORT="discard">[CONTIKI_DIR]/examples/6lowpan-nd/6lr/ex-6lowpannd-6lr.c</source>
      <commands EXPORT="discard">make ex-6lowpannd-6lr.sky TARGET=sky</commands>
      <firmware EXPORT="copy">[CONTIKI_DIR]/examples/6lowpan-nd/6lr/ex-6lowpannd-6lr.sky</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyFlash</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>
    <motetype>
      org.contikios.cooja.mspmote.SkyMoteType
      <identifier>sky3</identifier>
      <description>Border router</description>
      <source EXPORT="discard">[CONTIKI_DIR]/examples/6lowpan-nd/6lbr/ex-6lowpannd-6lbr.c</source>
      <commands EXPORT="discard">make ex-6lowpannd-6lbr.sky TARGET=sky</commands>
      <firmware EXPORT="copy">[CONTIKI_DIR]/examples/6lowpan-nd/6lbr/ex-6lowpannd-6lbr.sky</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyFlash</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>
    <mote>
      <breakpoints />
      <interface_config>
        org.contikios.cooja.interfaces.Position
        <x>48.045261104095495</x>
        <y>53.694046148582835</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspMoteID
        <id>1</id>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspSerial
        <history>netd nc~;</history>
      </interface_config>
      <motetype_identifier>sky3</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        org.contikios.cooja.interfaces.Position
        <x>24.57368106472331</x>
        <y>93.63732872459359</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspMoteID
        <id>2</id>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspSerial
        <history>netd nc~;</history>
      </interface_config>
      <motetype_identifier>sky2</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        org.contikios.cooja.interfaces.Position
        <x>-16.292769319017438</x>
        <y>94.53772929025756</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspMoteID
        <id>3</id>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspSerial
        <history>netd nc~;netd rt~;netd nc~;netd rt~;netd nc~;</history>
      </interface_config>
      <motetype_identifier>sky1</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        org.contikios.cooja.interfaces.Position
        <x>61.90372840805272</x>
        <y>92.2744691858602</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspMoteID
        <id>4</id>
      </interface_config>
      <motetype_identifier>sky2</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        org.contikios.cooja.interfaces.Position
        <x>44.49388880880046</x>
        <y>128.8125912889984</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspMoteID
        <id>5</id>
      </interface_config>
      <motetype_identifier>sky2</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
        org.contikios.cooja.interfaces.Position
        <x>85.70465169454424</x>
        <y>130.9533389936734</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspMoteID
        <id>6</id>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspSerial
        <history>netd cp~;netd nc~;netd cp~;netd nc~;help~;restart~;netd nc~;</history>
      </interface_config>
      <motetype_identifier>sky1</motetype_identifier>
    </mote>
  </simulation>
  <plugin>
    org.contikios.cooja.plugins.SimControl
    <width>280</width>
    <z>2</z>
    <height>160</height>
    <location_x>62</location_x>
    <location_y>747</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.LogListener
    <plugin_config>
      <filter />
      <formatted_time />
      <coloring />
    </plugin_config>
    <width>919</width>
    <z>1</z>
    <height>911</height>
    <location_x>411</location_x>
    <location_y>3</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.TimeLine
    <plugin_config>
      <mote>0</mote>
      <mote>1</mote>
      <mote>2</mote>
      <mote>3</mote>
      <mote>4</mote>
      <mote>5</mote>
      <showRadioRXTX />
      <showRadioHW />
      <showLEDs />
      <zoomfactor>500.0</zoomfactor>
    </plugin_config>
    <width>1319</width>
    <z>5</z>
    <height>166</height>
    <location_x>0</location_x>
    <location_y>847</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.Visualizer
    <plugin_config>
      <moterelations>true</moterelations>
      <skin>org.contikios.cooja.plugins.skins.IDVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.GridVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.TrafficVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.UDGMVisualizerSkin</skin>
      <viewport>1.4307387660996298 0.0 0.0 1.4307387660996298 116.37903238554924 24.639981351565215</viewport>
    </plugin_config>
    <width>399</width>
    <z>3</z>
    <height>400</height>
    <location_x>1</location_x>
    <location_y>1</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.RadioLogger
    <plugin_config>
      <split>217</split>
      <formatted_time />
      <showdups>false</showdups>
      <hidenodests>false</hidenodests>
      <analyzers name="6lowpan" />
    </plugin_config>
    <width>865</width>
    <z>4</z>
    <height>438</height>
    <location_x>1</location_x>
    <location_y>402</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.ScriptRunner
    <plugin_config>
      <script>var prefix = "bbbb";&#xD;
var brID = 1;&#xD;
&#xD;
//Wait until all mote started&#xD;
for(var i=0; i&lt;3; i++) {&#xD;
    YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki&gt;") != -1);&#xD;
}&#xD;
&#xD;
// Display all mote Table&#xD;
function displayAllTable(){&#xD;
    var allm = sim.getMotes();&#xD;
    for(var id in  allm) {&#xD;
	    write(allm[id], "netd nc");&#xD;
	    YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki&gt;") != -1);&#xD;
	    write(allm[id], "netd rt");&#xD;
	    YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki&gt;") != -1);&#xD;
	    write(allm[id], "netd cp");&#xD;
	    YIELD_THEN_WAIT_UNTIL(msg.indexOf("Contiki&gt;") != -1);&#xD;
	}&#xD;
}&#xD;
//displayAllTable();&#xD;
&#xD;
// Add to routing table&#xD;
function genip(num, pref){&#xD;
    var ip = "";&#xD;
    ip += pref;&#xD;
    for(var i=0; i&lt;3; i++){&#xD;
        ip +=":0000";&#xD;
    }&#xD;
    var n;&#xD;
    ip += ":0212";&#xD;
    n = 0x7400 + num;&#xD;
    ip += ":"+n.toString(16);&#xD;
    n = 0x0 + num;&#xD;
    ip += ":000"+n.toString(16);&#xD;
    n = (0x100 * num) + num;&#xD;
    ip += ":0"+n.toString(16);&#xD;
    return ip;&#xD;
}&#xD;
function gpip(num) { return genip(num, prefix); }&#xD;
function llip(num) { return genip(num, "fe80"); }&#xD;
function genpref(pref){&#xD;
    var ip = "";&#xD;
    ip += pref;&#xD;
    var n = pref.split(":");&#xD;
    var r = 8 - n.length;&#xD;
    for(var i=0; i&lt;r; i++){&#xD;
        ip +=":0000";&#xD;
    }&#xD;
    return ip;&#xD;
}&#xD;
function addroute(moteID, to, nexthop, len) {&#xD;
    mote = sim.getMoteWithID(moteID);&#xD;
    var cmd = "route -a "+gpip(to)+" "+llip(nexthop)+" "+len;&#xD;
    log.log("MOTE " +moteID + "-&gt;" + cmd + "\n");&#xD;
    write(mote, cmd);&#xD;
}&#xD;
&#xD;
&#xD;
//Waiting configurate of all mote was done&#xD;
function waitingConfig(){&#xD;
	var lastid = 0;&#xD;
	for(var i=0;; i++) {&#xD;
	    YIELD_THEN_WAIT_UNTIL(msg.contains("Sending") || msg.contains("timeout"));&#xD;
        if(msg.contains("Sending")){&#xD;
	        GENERATE_MSG(75000, "timeout"+lastid);&#xD;
	        lastid++;&#xD;
	    }else if(msg.equals("timeout"+(lastid-1))) {&#xD;
	        return;&#xD;
	    }&#xD;
	}&#xD;
}&#xD;
&#xD;
function sendudp(from, to) {&#xD;
    mote = sim.getMoteWithID(from);&#xD;
    var cmd = "sendudp " + gpip(to);&#xD;
    log.log("mote "+from+" -&gt; "+cmd+"\n");&#xD;
    write(mote, cmd);&#xD;
    YIELD_THEN_WAIT_UNTIL(msg.contains("DATA recv 'Hello"));   &#xD;
}&#xD;
&#xD;
function sendudpBR(br) {&#xD;
    var allm = sim.getMotes();&#xD;
    for(var i in  allm) {&#xD;
        var id = allm[i].getID();&#xD;
        if(id != br) {&#xD;
            sendudp(id, br);&#xD;
            sendudp(br, id);&#xD;
        }&#xD;
    }&#xD;
}&#xD;
&#xD;
&#xD;
function buildRT(s) {&#xD;
    while(true) {&#xD;
	    YIELD_THEN_WAIT_UNTIL(msg.contains("Received NA"));&#xD;
	    var from = msg.split("to")[1].split("::")[1].split(":")[2];&#xD;
	    var alldone = true;&#xD;
	    for(var i in s){&#xD;
	        var v = s[i];&#xD;
	        if(v.mote == from) {&#xD;
	            v.fct();&#xD;
	            v.done = true;    &#xD;
	        }&#xD;
	        if(!v.done) alldone = false;&#xD;
	    }&#xD;
	    if(alldone) return;&#xD;
    }&#xD;
}&#xD;
&#xD;
function addCO(prefix, len) {&#xD;
    var cmd = "cp -a "+genpref(prefix)+" "+len;&#xD;
    log.log("CO added -&gt; "+cmd+"\n");&#xD;
    write(sim.getMoteWithID(brID), cmd);&#xD;
}&#xD;
&#xD;
function rmCO(prefix, len) {&#xD;
    var cmd = "cp -r "+genpref(prefix)+" "+len;&#xD;
    log.log("CO remove -&gt; "+cmd+"\n");&#xD;
    write(sim.getMoteWithID(brID), cmd);&#xD;
}&#xD;
&#xD;
//Display NC when changement of msg was done&#xD;
//TIMEOUT(300000);&#xD;
log.log("Modify RT\n");&#xD;
buildRT([&#xD;
    {"mote":2, &#xD;
     "fct":function(){&#xD;
        addroute(1,3,2,128);&#xD;
        addroute(1,5,2,128);&#xD;
        }&#xD;
    },&#xD;
    {"mote":4, &#xD;
     "fct":function(){&#xD;
        addroute(1,6,4,128);&#xD;
        }&#xD;
    }&#xD;
]);&#xD;
&#xD;
addCO("cccc",16);&#xD;
&#xD;
while(true) {&#xD;
	waitingConfig();&#xD;
	log.log("Topology stable\n");&#xD;
	//addroute(1,1,2,32);&#xD;
	displayAllTable();&#xD;
	//log.log("Sending udp packet...\n");&#xD;
    //sendudpBR(brID);&#xD;
	//log.log("   OK\n");&#xD;
}&#xD;
log.testOK();</script>
      <active>true</active>
    </plugin_config>
    <width>560</width>
    <z>0</z>
    <height>995</height>
    <location_x>1341</location_x>
    <location_y>1</location_y>
  </plugin>
</simconf>

