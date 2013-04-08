/*
 * Copyright (c) 2009, Swedish Institute of Computer Science.
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
 */

package se.sics.cooja.plugins;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Observable;
import java.util.Observer;
import java.util.Properties;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.ButtonGroup;
import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSplitPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.KeyStroke;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.AbstractTableModel;

import org.apache.log4j.Logger;
import org.jdom.Element;

import se.sics.cooja.ClassDescription;
import se.sics.cooja.ConvertedRadioPacket;
import se.sics.cooja.GUI;
import se.sics.cooja.Plugin;
import se.sics.cooja.PluginType;
import se.sics.cooja.RadioConnection;
import se.sics.cooja.RadioMedium;
import se.sics.cooja.RadioPacket;
import se.sics.cooja.Simulation;
import se.sics.cooja.VisPlugin;
import se.sics.cooja.dialogs.TableColumnAdjuster;
import se.sics.cooja.interfaces.Radio;
import se.sics.cooja.plugins.analyzers.ICMPv6Analyzer;
import se.sics.cooja.plugins.analyzers.IEEE802154Analyzer;
import se.sics.cooja.plugins.analyzers.IPHCPacketAnalyzer;
import se.sics.cooja.plugins.analyzers.IPv6PacketAnalyzer;
import se.sics.cooja.plugins.analyzers.PacketAnalyzer;
import se.sics.cooja.plugins.analyzers.RadioLoggerAnalyzerSuite;
import se.sics.cooja.util.StringUtils;

/**
 * Radio logger listens to the simulation radio medium and lists all transmitted
 * data in a table.
 *
 * @author Fredrik Osterlind
 */
@ClassDescription("Radio messages")
@PluginType(PluginType.SIM_PLUGIN)
public class RadioLogger extends VisPlugin {
  private static Logger logger = Logger.getLogger(RadioLogger.class);
  private static final long serialVersionUID = -6927091711697081353L;

  private final Simulation simulation;
  private final JTable dataTable;
  private ArrayList<RadioConnectionLog> connections = new ArrayList<RadioConnectionLog>();
  private RadioMedium radioMedium;
  private Observer radioMediumObserver;
  private AbstractTableModel model;

  private HashMap<String,Action> analyzerMap = new HashMap<String,Action>();
  private String analyzerName = null;
  private PacketAnalyzer analyzer = null;

  public RadioLogger(final Simulation simulationToControl, final GUI gui) {
    super("Radio messages", gui, false);
    setLayout(new BorderLayout());

    simulation = simulationToControl;
    radioMedium = simulation.getRadioMedium();

    analyser = new IEEE802154Analyzer(true);

    /* Load additional analyzers specified by projects (cooja.config) */
/*
    String[] projectAnalyzerSuites =
      gui.getProjectConfig().getStringArrayValue(RadioLogger.class, "ANALYZERS");
    if (projectAnalyzerSuites != null) {
      for (String suiteName: projectAnalyzerSuites) {
        Class<? extends RadioLoggerAnalyzerSuite> suiteClass =
          gui.tryLoadClass(RadioLogger.this, RadioLoggerAnalyzerSuite.class, suiteName);
        try {
          RadioLoggerAnalyzerSuite suite = suiteClass.newInstance();
          ArrayList<PacketAnalyzer> suiteAnalyzers = suite.getAnalyzers();
          rbMenuItem = new JRadioButtonMenuItem(createAnalyzerAction(
              suite.getDescription(), suiteName, suiteAnalyzers, false));
          group.add(rbMenuItem);
          popupMenu.add(rbMenuItem);
          logger.debug("Loaded radio logger analyzers: " + suite.getDescription());
        } catch (InstantiationException e1) {
          logger.warn("Failed to load analyzer suite '" + suiteName + "': " + e1.getMessage());
        } catch (IllegalAccessException e1) {
          logger.warn("Failed to load analyzer suite '" + suiteName + "': " + e1.getMessage());
        }
      }
    }
*/


    radioMedium.addRadioMediumObserver(radioMediumObserver = new Observer() {
      public void update(Observable obs, Object obj) {
        RadioConnection conn = radioMedium.getLastConnection();
        if (conn == null) {
          return;
        }
        final RadioConnectionLog loggedConn = new RadioConnectionLog();
        loggedConn.startTime = conn.getStartTime();
        loggedConn.endTime = simulation.getSimulationTime();
        loggedConn.connection = conn;
        loggedConn.packet = conn.getSource().getLastPacketTransmitted();
        java.awt.EventQueue.invokeLater(new Runnable() {
          public void run() {
            int lastSize = connections.size();
            // Check if the last row is visible
            boolean isVisible = false;
            int rowCount = dataTable.getRowCount();
            if (rowCount > 0) {
              Rectangle lastRow = dataTable.getCellRect(rowCount - 1, 0, true);
              Rectangle visible = dataTable.getVisibleRect();
              isVisible = visible.y <= lastRow.y && visible.y + visible.height >= lastRow.y + lastRow.height;
            }
            connections.add(loggedConn);
            if (connections.size() > lastSize) {
              model.fireTableRowsInserted(lastSize, connections.size() - 1);
            }
            if (isVisible) {
              dataTable.scrollRectToVisible(dataTable.getCellRect(dataTable.getRowCount() - 1, 0, true));
            }
            setTitle("Radio messages: " + dataTable.getRowCount() + " messages seen");
          }
        });
        }
      });

  }

	private void searchSelectNext(String text, boolean reverse) {
		if (text.isEmpty()) {
			return;
		}
		int row = dataTable.getSelectedRow();
    if (row < 0) {
    	row = 0;
    }

    if (!reverse) {
    	row++;
    } else {
    	row--;
    }

    int rows = dataTable.getModel().getRowCount();
    for (int i=0; i < rows; i++) {
    	int r;
    	if (!reverse) {
    		r = (row + i + rows)%rows;
    	} else {
    		r = (row - i + rows)%rows;
    	}
    	String val = (String) dataTable.getModel().getValueAt(r, COLUMN_DATA);
    	if (!val.contains(text)) {
    		continue;
    	}
    	dataTable.setRowSelectionInterval(r,r);
    	dataTable.scrollRectToVisible(dataTable.getCellRect(r, COLUMN_DATA, true));
    	searchField.setBackground(Color.WHITE);
    	return;
    }
  	searchField.setBackground(Color.RED);
	}

  private void prepareDataString(RadioConnectionLog conn) {
    byte[] data;
    if (conn.packet == null) {
      data = null;
    } else if (conn.packet instanceof ConvertedRadioPacket) {
      data = ((ConvertedRadioPacket)conn.packet).getOriginalPacketData();
    } else {
      data = conn.packet.getPacketData();
    }
    if (data == null) {
      conn.data = "[unknown data]";
      return;
    }

    StringBuffer brief = new StringBuffer();
    StringBuffer verbose = new StringBuffer();

    /* default analyzer */
    PacketAnalyzer.Packet packet = new PacketAnalyzer.Packet(data, PacketAnalyzer.MAC_LEVEL);

    if (analyzePacket(packet, brief, verbose)) {
        if (packet.hasMoreData()) {
            byte[] payload = packet.getPayload();
            brief.append(StringUtils.toHex(payload, 4));
            if (verbose.length() > 0) {
                verbose.append("<p>");
            }
            verbose.append("<b>Payload (")
            .append(payload.length).append(" bytes)</b><br><pre>")
            .append(StringUtils.hexDump(payload))
            .append("</pre>");
        }
        conn.data = (data.length < 100 ? (data.length < 10 ? "  " : " ") : "")
        + data.length + ": " + brief;
        if (verbose.length() > 0) {
            conn.tooltip = verbose.toString();
        }
    } else {
        conn.data = data.length + ": 0x" + StringUtils.toHex(data, 4);
    }
  }

  private boolean analyzePacket(PacketAnalyzer.Packet packet, StringBuffer brief, StringBuffer verbose) {
      if (analyzers == null) return false;
      try {
        boolean analyze = true;
        while (analyze) {
          analyze = false;
          for (int i = 0; i < analyzers.size(); i++) {
              PacketAnalyzer analyzer = analyzers.get(i);
              if (analyzer.matchPacket(packet)) {
                  int res = analyzer.analyzePacket(packet, brief, verbose);
                  if (packet.hasMoreData() && brief.length() > 0) {
                      brief.append('|');
                      verbose.append("<br>");
                  }
                  if (res != PacketAnalyzer.ANALYSIS_OK_CONTINUE) {
                      /* this was the final or the analysis failed - no analyzable payload possible here... */
                      return brief.length() > 0;
                  }
                  /* continue another round if more bytes left */
                  analyze = packet.hasMoreData();
                  break;
              }
          }
      }
      } catch (Exception e) {
        logger.debug("Error when analyzing packet: " + e.getMessage(), e);
        return false;
      }
      return brief.length() > 0;
  }


  public void closePlugin() {
    if (radioMediumObserver != null) {
      radioMedium.deleteRadioMediumObserver(radioMediumObserver);
    }
  }

/*
  public Collection<Element> getConfigXML() {
    ArrayList<Element> config = new ArrayList<Element>();

    Element element = new Element("split");
    element.addContent(Integer.toString(splitPane.getDividerLocation()));
    config.add(element);

    if (analyzerName != null && analyzers != null) {
      element = new Element("analyzers");
      element.setAttribute("name", analyzerName);
      config.add(element);
    }

    if (aliases != null) {
      for (Object key: aliases.keySet()) {
        element = new Element("alias");
        element.setAttribute("payload", (String) key);
        element.setAttribute("alias", (String) aliases.get(key));
        config.add(element);
      }
    }

    return config;
  }*/



  private class RadioConnectionLog {
    long startTime;
    long endTime;
    RadioConnection connection;
    RadioPacket packet;

    String data = null;
    String tooltip = null;

    public String toString() {
    	if (data == null) {
    		RadioLogger.this.prepareDataString(this);
    	}
    	return
    	Long.toString(startTime / Simulation.MILLISECOND) + "\t" +
    	connection.getSource().getMote().getID() + "\t" +
    	getDestString(this) + "\t" +
    	data;
    }
  }

/*
  private static String getDestString(RadioConnectionLog c) {
    Radio[] dests = c.connection.getDestinations();
    if (dests.length == 0) {
      return "-";
    }
    if (dests.length == 1) {
      return "" + dests[0].getMote().getID();
    }
    StringBuilder sb = new StringBuilder();
    for (Radio dest: dests) {
      sb.append(dest.getMote().getID()).append(',');
    }
    sb.setLength(sb.length()-1);
    return sb.toString();
  }
*/
/*
  private Action createAnalyzerAction(String name, final String actionName,
          final ArrayList<PacketAnalyzer> analyzerList, boolean selected) {
      Action action = new AbstractAction(name) {
        private static final long serialVersionUID = -608913700422638454L;

        public void actionPerformed(ActionEvent event) {
            if (analyzers != analyzerList) {
                analyzers = analyzerList;
                analyzerName = actionName;
                if (connections.size() > 0) {
                    // Remove the cached values
                    for(int i = 0; i < connections.size(); i++) {
                        RadioConnectionLog conn = connections.get(i);
                        conn.data = null;
                        conn.tooltip = null;
                    }
                    model.fireTableRowsUpdated(0, connections.size() - 1);
                }
                verboseBox.setText("");
            }
        }
      };
      action.putValue(Action.SELECTED_KEY, selected ? Boolean.TRUE : Boolean.FALSE);
      analyzerMap.put(actionName, action);
      return action;
  }
*/
/*
  private Action saveAction = new AbstractAction("Save to file...") {
    private static final long serialVersionUID = -3942984643211482179L;

    public void actionPerformed(ActionEvent e) {
      JFileChooser fc = new JFileChooser();
      int returnVal = fc.showSaveDialog(GUI.getTopParentContainer());
      if (returnVal != JFileChooser.APPROVE_OPTION) {
        return;
      }

      File saveFile = fc.getSelectedFile();
      if (saveFile.exists()) {
        String s1 = "Overwrite";
        String s2 = "Cancel";
        Object[] options = { s1, s2 };
        int n = JOptionPane.showOptionDialog(
            GUI.getTopParentContainer(),
            "A file with the same name already exists.\nDo you want to remove it?",
            "Overwrite existing file?", JOptionPane.YES_NO_OPTION,
            JOptionPane.QUESTION_MESSAGE, null, options, s1);
        if (n != JOptionPane.YES_OPTION) {
          return;
        }
      }

      if (saveFile.exists() && !saveFile.canWrite()) {
        logger.fatal("No write access to file: " + saveFile);
        return;
      }

      try {
        PrintWriter outStream = new PrintWriter(new FileWriter(saveFile));
        for(int i=0; i < connections.size(); i++) {
          outStream.print("" + (i + 1) + '\t');
          outStream.print("" + dataTable.getValueAt(i, COLUMN_TIME) + '\t');
          outStream.print("" + dataTable.getValueAt(i, COLUMN_FROM) + '\t');
          outStream.print("" + getDestString(connections.get(i)) + '\t');
          outStream.print("" + dataTable.getValueAt(i, COLUMN_DATA) + '\n');
        }
        outStream.close();
      } catch (Exception ex) {
        logger.fatal("Could not write to file: " + saveFile);
        return;
      }

    }
  };
*/
/*
  public String getConnectionsString() {
  	StringBuilder sb = new StringBuilder();
  	RadioConnectionLog[] cs = connections.toArray(new RadioConnectionLog[0]);
  	for(RadioConnectionLog c: cs) {
      sb.append(c.toString() + "\n");
    }
    return sb.toString();
  };

  public void saveConnectionsToFile(String fileName) {
    StringUtils.saveToFile(new File(fileName), getConnectionsString());
  };
*/
}
