<?xml version="1.0" encoding="UTF-8"?>
<simconf>
  <project EXPORT="discard">[APPS_DIR]/mrm</project>
  <project EXPORT="discard">[APPS_DIR]/mspsim</project>
  <project EXPORT="discard">[APPS_DIR]/avrora</project>
  <project EXPORT="discard">[APPS_DIR]/serial_socket</project>
  <project EXPORT="discard">[APPS_DIR]/collect-view</project>
  <project EXPORT="discard">[APPS_DIR]/powertracker</project>
  <project EXPORT="discard">[APPS_DIR]/serial2pty</project>
  <project EXPORT="discard">[APPS_DIR]/radiologger-headless</project>
  <simulation>
    <title>6lbr template</title>
    <speedlimit>1.0</speedlimit>
    <randomseed>generated</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
      org.contikios.cooja.radiomediums.UDGM
      <transmitting_range>45.0</transmitting_range>
      <interference_range>45.0</interference_range>
      <success_ratio_tx>1.0</success_ratio_tx>
      <success_ratio_rx>1.0</success_ratio_rx>
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      org.contikios.cooja.mspmote.SkyMoteType
      <identifier>templatesky1</identifier>
      <description>template</description>
      <source EXPORT="discard">[CONFIG_DIR]/template.c</source>
      <commands EXPORT="discard">make template.sky TARGET=sky</commands>
      <firmware EXPORT="copy">[CONFIG_DIR]/template.sky</firmware>
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
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyByteRadio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>
    <mote>
      <breakpoints />
      <interface_config>
        org.contikios.cooja.interfaces.Position
        <x>XPOS</x>
        <y>YPOS</y>
        <z>ZPOS</z>
      </interface_config>
      <interface_config>
        org.contikios.cooja.mspmote.interfaces.MspMoteID
        <id>1</id>
      </interface_config>
      <motetype_identifier>templatesky1</motetype_identifier>
    </mote>
  </simulation>
  <plugin>
    org.contikios.cooja.plugins.SimControl
    <width>280</width>
    <z>1</z>
    <height>160</height>
    <location_x>465</location_x>
    <location_y>0</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.Visualizer
    <plugin_config>
      <skin>org.contikios.cooja.plugins.skins.IDVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.GridVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.TrafficVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.UDGMVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.MoteTypeVisualizerSkin</skin>
      <viewport>1.3090909090909089 0.0 0.0 1.3090909090909089 146.45454545454544 173.0</viewport>
    </plugin_config>
    <width>462</width>
    <z>6</z>
    <height>400</height>
    <location_x>1</location_x>
    <location_y>1</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.LogListener
    <plugin_config>
      <filter />
    </plugin_config>
    <width>1213</width>
    <z>7</z>
    <height>821</height>
    <location_x>466</location_x>
    <location_y>161</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.Notes
    <plugin_config>
      <notes>6LBR Test Support

This is an automatically generated COOJA simulation file

more information: https://github.com/cetic/6lbr/wiki/6LBR-Test-Environment</notes>
      <decorations>true</decorations>
    </plugin_config>
    <width>932</width>
    <z>2</z>
    <height>162</height>
    <location_x>745</location_x>
    <location_y>-1</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.ScriptRunner
    <plugin_config>
      <script>
TIMEOUT(999999999999);&#xD;
sim.setSpeedLimit(1);&#xD;
&#xD;
while (true) {&#xD;
  log.log(time + ":" + id + ":" + msg + "\n");&#xD;
  &#xD;
  if(msg.contains("movemote")){&#xD;
    elems = msg.split(",");&#xD;
    log.log("command:" + elems[0] + "\n");&#xD;
    log.log("nodeid:" + elems[1] + "\n");&#xD;
    log.log("xpos:" + elems[2] + "\n");&#xD;
    log.log("ypos:" + elems[3] + "\n");&#xD;
    nodeid = parseInt(elems[1]);&#xD;
    xpos = parseFloat(elems[2]);&#xD;
    ypos = parseFloat(elems[3]);&#xD;
    pos = sim.getMoteWithID(nodeid).getInterfaces().getPosition();&#xD;
    pos.setCoordinates(xpos,ypos, pos.getZCoordinate());&#xD;
  }&#xD;
  &#xD;
  if(msg.contains("killcooja")){&#xD;
    log.log("Killing Cooja\n");&#xD;
    log.testOK();&#xD;
  }&#xD;
  YIELD();&#xD;
}</script>
      <active>false</active>
    </plugin_config>
    <width>462</width>
    <z>3</z>
    <height>335</height>
    <location_x>3</location_x>
    <location_y>646</location_y>
  </plugin>
  <plugin>
    be.cetic.cooja.plugins.RadioLoggerHeadless
    <width>216</width>
    <z>0</z>
    <height>103</height>
    <location_x>1501</location_x>
    <location_y>594</location_y>
  </plugin>
</simconf>

