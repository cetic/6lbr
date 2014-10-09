<?xml version="1.0" encoding="UTF-8"?>
<simconf>
  <project EXPORT="discard">[APPS_DIR]/mrm</project>
  <project EXPORT="discard">[APPS_DIR]/mspsim</project>
  <project EXPORT="discard">[APPS_DIR]/avrora</project>
  <project EXPORT="discard">[APPS_DIR]/serial_socket</project>
  <project EXPORT="discard">[APPS_DIR]/collect-view</project>
  <project EXPORT="discard">[APPS_DIR]/powertracker</project>
  <simulation>
    <title>6lbr template</title>
    <speedlimit>1.0</speedlimit>
    <randomseed>generated</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
      se.sics.cooja.radiomediums.UDGM
      <transmitting_range>300.0</transmitting_range>
      <interference_range>400.0</interference_range>
      <success_ratio_tx>1.0</success_ratio_tx>
      <success_ratio_rx>1.0</success_ratio_rx>
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      org.contikios.cooja.mspmote.WismoteMoteType
      <identifier>6lbr-rpl</identifier>
      <description>6LBR RPL routing</description>
      <source EXPORT="discard">[CONTIKI_DIR]/examples/6lowpan-nd/6lbr-rpl/ex-6lowpannd-6lbr.c</source>
      <commands EXPORT="discard">make ex-6lowpannd-6lbr.wismote TARGET=wismote WITH_STATISTICS=1</commands>
      <firmware EXPORT="copy">[CONTIKI_DIR]/examples/6lowpan-nd/6lbr-rpl/ex-6lowpannd-6lbr.wismote</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDefaultSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
    </motetype>
    <motetype>
      org.contikios.cooja.mspmote.WismoteMoteType
      <identifier>6lr-rpl</identifier>
      <description>6LR RPL routing</description>
      <source EXPORT="discard">[CONTIKI_DIR]/examples/6lowpan-nd/6lr-rpl/ex-6lowpannd-6lr.c</source>
      <commands EXPORT="discard">make ex-6lowpannd-6lr.wismote TARGET=wismote WITH_STATISTICS=1</commands>
      <firmware EXPORT="copy">[CONTIKI_DIR]/examples/6lowpan-nd/6lr-rpl/ex-6lowpannd-6lr.wismote</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDefaultSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
    </motetype>
    <motetype>
      org.contikios.cooja.mspmote.WismoteMoteType
      <identifier>6lh</identifier>
      <description>6LH manual routing</description>
      <source EXPORT="discard">[CONTIKI_DIR]/examples/6lowpan-nd/6lh/ex-6lowpannd-6lh.c</source>
      <commands EXPORT="discard">make ex-6lowpannd-6lh.wismote TARGET=wismote WITH_STATISTICS=1</commands>
      <firmware EXPORT="copy">[CONTIKI_DIR]/examples/6lowpan-nd/6lh/ex-6lowpannd-6lh.wismote</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDefaultSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
    </motetype>
    <mote>
      <breakpoints />
      <interface_config>
	se.sics.cooja.mspmote.interfaces.MspMoteID
	<id>1</id>
      </interface_config>
      <interface_config>
	se.sics.cooja.interfaces.Position
	<x>0.0000000000000</x>
	<y>0.0000000000000</y>
	<z>0.0000000000000</z>
      </interface_config>
      <motetype_identifier>6lbr-rpl</motetype_identifier>
    </mote>
    <mote>
      <breakpoints />
      <interface_config>
	se.sics.cooja.mspmote.interfaces.MspMoteID
	<id>2</id>
      </interface_config>
      <interface_config>
	se.sics.cooja.interfaces.Position
	<x>30.0000000000000</x>
	<y>0.0000000000000</y>
	<z>0.0000000000000</z>
      </interface_config>
      <motetype_identifier>6lh</motetype_identifier>
    </mote>
  </simulation>
  <plugin>
    se.sics.cooja.plugins.SimControl
    <width>280</width>
    <z>1</z>
    <height>160</height>
    <location_x>465</location_x>
    <location_y>0</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.Visualizer
    <plugin_config>
      <skin>se.sics.cooja.plugins.skins.IDVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.GridVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.TrafficVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.UDGMVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.MoteTypeVisualizerSkin</skin>
      <viewport>1.3090909090909089 0.0 0.0 1.3090909090909089 146.45454545454544 173.0</viewport>
    </plugin_config>
    <width>462</width>
    <z>6</z>
    <height>400</height>
    <location_x>1</location_x>
    <location_y>1</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.LogListener
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
    se.sics.cooja.plugins.Notes
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
    se.sics.cooja.plugins.ScriptRunner
    <plugin_config>
      <scriptfile>
        [CONFIG_DIR]/../line-rpl_script.js
      </scriptfile>
      <active>true</active>
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
  <plugin>
    SerialSocketServer
    <mote_arg>0</mote_arg>
    <width>459</width>
    <z>4</z>
    <height>119</height>
    <location_x>5</location_x>
    <location_y>525</location_y>
  </plugin>
  <plugin>
    SerialSocketServer
    <mote_arg>1</mote_arg>
    <width>459</width>
    <z>4</z>
    <height>119</height>
    <location_x>5</location_x>
    <location_y>525</location_y>
  </plugin>
</simconf>

