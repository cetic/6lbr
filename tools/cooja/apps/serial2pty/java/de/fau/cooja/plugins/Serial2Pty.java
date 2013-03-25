/*
 * Changed for use with Contiki 2.3, sky motes and binary data
 * transmission
 *
 * 2009-10-26
 * Niko Pollner, Chair for Computer Science 6, FAU Erlangen-Nuremberg
 *
 * 2012-03-01
 * Andre Frimberger:
 *   * Implemented pseudo terminals as a real serial device wrapper.
 *   * Allows using standard serial tools to communicate with virtual mote
 *
 *
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: Serial2Pipe.java,v 1.1 2009/05/18 14:48:10 fros4943 Exp $
 */

package de.fau.cooja.plugins;

import java.awt.GridLayout;
import java.io.*;
import java.util.LinkedList;
import java.util.Observable;
import java.util.Observer;
import javax.swing.*;
import org.apache.log4j.Logger;

import se.sics.cooja.*;
import se.sics.cooja.GUI;
import se.sics.cooja.interfaces.SerialPort;

@ClassDescription("Serial 2 Pty")
@PluginType(PluginType.MOTE_PLUGIN)
public class Serial2Pty extends VisPlugin implements MotePlugin {

  private static Logger logger = Logger.getLogger(Serial2Pty.class);
  private static final long serialVersionUID = 1L;

  private Mote mote;
  private SerialPort serialPort;
  private String ptyDeviceName;

  private InputStream inPipeStream;
  private OutputStream outPipeStream;

  private Thread outPipeInitializer;
  private Thread pipeListener;

  private LinuxPseudoTerminal pty;

  private Observer serialObserver;

  public Serial2Pty(Mote moteToView, Simulation simulation, GUI gui) {
      super("Serial 2 Pty (" + moteToView + ")", gui, false);
      mote = moteToView;
      String pluginPath = "";

      // where is this plugin installed?
      COOJAProject[] projects = gui.getProjects();
      for (int i=0; i<projects.length; ++i) {

          if (projects[i].getDescription() != null &&
              projects[i].getDescription().startsWith("serial2pty")) {

              pluginPath = projects[i].dir.getAbsolutePath();
              break;
          }
      }

      if (pluginPath.isEmpty()) {
          logger.fatal("pluginPath of serial2pty not found!");
      }
      
      pty = new LinuxPseudoTerminal(pluginPath + "/lib/");

      try {
          pty.open();
      } catch (IOException e) {
          logger.fatal("failure while opening pseudo terminal.");
          e.printStackTrace();
      }

      inPipeStream = pty.getPtInputStream();
      outPipeStream = pty.getPtOutputStream();
      ptyDeviceName = pty.getDeviceName();
      
      Serial2PtyRegistry.addSerialPort(ptyDeviceName, mote.getID());

      for (MoteInterface intf: mote.getInterfaces().getInterfaces()) {
          if (intf instanceof SerialPort) {
              serialPort = (SerialPort) intf;
          }
      }

      if (serialPort == null) {
          throw new RuntimeException("No sky mote serial port");
      }

      // Forward serial interface to out pipe
      // Separate thread for initializing out pipe. Otherwise FileOutputStream
      // constructor would block Cooja until the other end of the pipe is opened
      outPipeInitializer = new Thread(new Runnable() {

          String _serialData;

          public void run() {
              serialPort.addSerialDataObserver(serialObserver = new Observer() {
                  public void update(Observable obs, Object obj) {
                      try {
                          outPipeStream.write(serialPort.getLastSerialData());
                          outPipeStream.flush();

                          _serialData += (char)serialPort.getLastSerialData();
                          
                          if (_serialData.endsWith("\n") || _serialData.length() > 512) {
                              _serialData = _serialData.substring(0, _serialData.length() - 1);
                              logger.debug("Forwarding from serial: " + _serialData);
                              _serialData = "";
                          }
                      } catch (IOException e) {
                          logger.error("Error when writing to out pipe: " + e);
  
                          // Stop writing and try to reopen pipe
                          serialPort.deleteSerialDataObserver(serialObserver);
                          try {
                              outPipeStream.close();
                          } catch (IOException ec) {
                              logger.fatal("Error when closing out pipe: " + ec);
                          }
                          outPipeInitializer.start();
                      }
                  }
              }); // Observer
          }
      }); // Runnable
      outPipeInitializer.start();

      // Forward incoming data from in pipe (separate thread)
      pipeListener = new Thread(new Runnable() {
          public void run() {
              try {
                  while (true) {

                      // FileInputStream constructor blocks until other end of pipe is opened
                      //inPipeStream = new BufferedInputStream(new FileInputStream(inPipeName));
			          int i = 0;
                      for (int byte_read = inPipeStream.read(); byte_read != -1;
                          byte_read = inPipeStream.read()) {
                          logger.debug("Forwarding to serial: " + (char) byte_read);
                          //logger.debug((byte) byte_read);
                          serialPort.writeByte((byte) byte_read);
			              if (++i >= 64) {
                              logger.debug("Enough work done. Sleeping one second." );
			                  // Wait a bit each 128th byte to give sensor node time for processing
			                  i = 0;
			                  Thread.sleep(1000);
                          }
                          serialPort.flushInput();
                      }
                      inPipeStream.close();
                      inPipeStream = null;
//                    logger.debug("Input stream closed");
                  }
              } catch (Exception e) {
                  logger.fatal("Error while reading from in pipe: " + e);
              }
          }
      });
      pipeListener.start();
      if (GUI.isVisualized()) {
          // Add GUI labels
          setLayout(new GridLayout(3, 2));


          add(new JLabel("moteID:"));
          add(new JLabel(String.valueOf(mote.getID())));
      
          add(new JLabel("Serial Device:"));
          add(new JLabel(ptyDeviceName));

          add(new JLabel("Discovery Server:"));
          add(new JLabel("Port " + String.valueOf(SerialPortDiscoveryServer.LISTEN_PORT)));

          //setSize(250, 75);
          setSize(250, 100);

          try {
            setSelected(true);
          } catch (java.beans.PropertyVetoException e) {
            // Could not select
          }
      }
  }

  public void closePlugin() {

      Serial2PtyRegistry.removeSerialPort(pty.getDeviceName());

      // Stop listening to serial port
      if (serialPort != null && serialObserver != null) {
          serialPort.deleteSerialDataObserver(serialObserver);
          serialObserver = null;
      }

      // Stop out pipe initializer thread
      if (outPipeInitializer != null) outPipeInitializer.interrupt();

      // Stop listening to in pipe
      if (pipeListener != null) pipeListener.interrupt();

      // Close pipes
      try {
          if (outPipeStream != null) outPipeStream.close();
      } catch (Exception e) {
          logger.fatal("Error when closing out pipe");
      }

      try {
          if (inPipeStream != null) inPipeStream.close();
      } catch (Exception e) {
          logger.fatal("Error when closing in pipe");
      }
      pty.close();
  }

  public Mote getMote() {
    return mote;
  }

}
