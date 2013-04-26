package de.fau.cooja.plugins;

import org.apache.log4j.Logger;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Set;

public class SerialPortDiscoveryServer extends Thread {
    public final static int LISTEN_PORT = 6100;
    private static Logger _logger = Logger.getLogger(SerialPortDiscoveryServer.class);

    @Override
    public void run() {
        ServerSocket serverSocket;

        try {
            serverSocket = new ServerSocket(LISTEN_PORT);
            _logger.info("Creating server for serial2pty serial port discovery. Listening on port " + LISTEN_PORT);
        } catch (IOException e) {
            _logger.fatal("cannot open ServerSocket on port " + LISTEN_PORT);
            e.printStackTrace();
            return;
        }

        try {
            while(true) {
                Socket client = serverSocket.accept();
                _logger.info("new Connection " + client + " established.");

                DataOutputStream cos = new DataOutputStream(client.getOutputStream());

                Set<String> serialPorts = Serial2PtyRegistry.getAllSerialPorts();
                
                for (String port : serialPorts) {
                    //_logger.debug("sending port " + port);
                    cos.writeBytes(port + "\n");
                }
                cos.flush();

                client.shutdownOutput();
                client.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
