package de.fau.cooja.plugins;


import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

public class Serial2PtyRegistry {

    private static LinkedHashMap<String, Integer> _registry = new LinkedHashMap<String, Integer>();
    private static SerialPortDiscoveryServer _serialPortDiscoveryServer = null;
    
    private Serial2PtyRegistry() {}

    public static void addSerialPort(String serialPort, Integer moteId) {
        // start discovery server when first serial port is added.
        if (_serialPortDiscoveryServer == null) {
            _serialPortDiscoveryServer = new SerialPortDiscoveryServer();
            _serialPortDiscoveryServer.start();
        }

        _registry.put(serialPort, moteId);
    }

    public static void removeSerialPort(String serialPort) {
        if (_registry.containsKey(serialPort)) {
            _registry.remove(serialPort);
        }
    }

    public static String getSerialPort(Integer moteId) {
        for (Map.Entry<String, Integer> entry : _registry.entrySet()) {
            if (entry.getValue().equals(moteId)) {
                return entry.getKey();
            }
        }
        return "";
    }



    public static Set<String> getAllSerialPorts() {
        return _registry.keySet();
    }

}
