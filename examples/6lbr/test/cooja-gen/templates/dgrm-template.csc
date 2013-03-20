<?xml version="1.0" encoding="UTF-8"?>
<simconf>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/mrm</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/mspsim</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/avrora</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/serial_socket</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/collect-view</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/powertracker</project>
  <project EXPORT="discard">/home/sd/svn/contikiprojects/uni-luebeck.de/ewsn11_cooja_interference/frossi-scanner</project>
  <project EXPORT="discard">/home/sd/svn/contikiprojects/uni-luebeck.de/ewsn11_cooja_interference/cooja_configuration</project>
  <project EXPORT="discard">/home/sd/svn/midflex/tools/04-testbed-interface/coojatwist</project>
  <project EXPORT="discard">/home/sd/svn/midflex/tools/04-testbed-interface/shell-download</project>
  <simulation>
    <title>TWIST</title>
    <randomseed>123456</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
		se.sics.cooja.radiomediums.DirectedGraphMedium
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      se.sics.cooja.mspmote.SkyMoteType
      <identifier>templatesky1</identifier>
      <description>template</description>
      <source EXPORT="discard">[CONFIG_DIR]/template.c</source>
      <commands EXPORT="discard">make template.sky TARGET=sky</commands>
      <firmware EXPORT="copy">[CONFIG_DIR]/template.sky</firmware>
      <moteinterface>se.sics.cooja.interfaces.Position</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyButton</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyFlash</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyByteRadio</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>
    <mote>
      <breakpoints />
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>XPOS</x>
        <y>YPOS</y>
        <z>ZPOS</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.mspmote.interfaces.MspMoteID
        <id>1</id>
      </interface_config>
      <motetype_identifier>templatesky1</motetype_identifier>
    </mote>
  </simulation>
  <plugin>
    se.sics.cooja.plugins.SimControl
    <width>280</width>
    <z>3</z>
    <height>160</height>
    <location_x>400</location_x>
    <location_y>0</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.Visualizer
    <plugin_config>
      <skin>se.sics.cooja.plugins.skins.IDVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.GridVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.DGRMVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.TrafficVisualizerSkin</skin>
      <viewport>17.876047993816435 0.0 0.0 17.876047993816435 83.28708690682097 -61.80454204202107</viewport>
    </plugin_config>
    <width>400</width>
    <z>0</z>
    <height>400</height>
    <location_x>1</location_x>
    <location_y>1</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.LogListener
    <plugin_config>
      <filter />
    </plugin_config>
    <width>1280</width>
    <z>2</z>
    <height>240</height>
    <location_x>400</location_x>
    <location_y>160</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.Notes
    <plugin_config>
      <notes>Enter notes here</notes>
      <decorations>true</decorations>
    </plugin_config>
    <width>1000</width>
    <z>1</z>
    <height>160</height>
    <location_x>680</location_x>
    <location_y>0</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.ScriptRunner
    <plugin_config>
    <script>
        TIMEOUT(999999999);
        log.log("Setting real-time delay\n");
        mote.getSimulation().setDelayTime(java.lang.Integer.MIN_VALUE);
        while (true) 
        {
           log.log(time + ":" + id + ":" + msg + "\n");
           YIELD();
        }
    </script>
    <active>true</active>
    </plugin_config>
    <width>600</width>
    <z>1</z>
    <height>700</height>
    <location_x>469</location_x>
    <location_y>0</location_y>
  </plugin>
</simconf>

