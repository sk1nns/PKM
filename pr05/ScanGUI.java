package pr05;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.io.IOException;


public class ScanGUI {

    private int minPort = 1;
    private int maxPort = 0x10000;
    private int timeout = 100;

    private static JLabel hostLabel;
    private static JTextField hostText;
    private static JButton enterButton;
    private static JLabel portLabel;
    private static JTextField portText;
    private static JTextArea textArea;

    private static String host;
    private static String port;

    public int getTimeout() {
        return timeout;
    }

    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    public int getMinPort() {
        return minPort;
    }

    public void setMinPort(int minPort) {
        this.minPort = minPort;
    }

    public int getMaxPort() {
        return maxPort;
    }

    public void setMaxPort(int maxPort) {
        this.maxPort = maxPort;
    }

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    private List<Integer> scan() {
        try {
            InetAddress ia = InetAddress.getByName(getHost());
            return scan(ia);
        } catch (IOException ioe) {
            return null;
        }
    }

    private List<Integer> scan(InetAddress inetAddress) {
        List<Integer> openPortsList = new ArrayList<Integer>(0xFF);
        textArea.append("scanning ports: \n");
        for (int port = minPort; port <= maxPort; port++) {
            textArea.append(String.valueOf(port));
            try {
                InetSocketAddress isa = new InetSocketAddress(inetAddress, port);
                Socket socket = new Socket();
                socket.connect(isa, timeout);
                textArea.append(" opened\n");
                openPortsList.add(port);
                socket.close();
            } catch (IOException ioe) {
                textArea.append("\n");
            }
        }
        return openPortsList;
    }

    public static void main(String[] args) {
        JFrame frame = new JFrame("PortScanner");
        frame.setSize(500, 500);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        JPanel panel = new JPanel();
        frame.add(panel);

        hostLabel = new JLabel("Host");
        hostText = new JTextField(20);
        portLabel = new JLabel("Port");
        portText = new JTextField(20);
        enterButton = new JButton("Enter");
        textArea = new JTextArea();

        placeComponents(panel);
        frame.setVisible(true);

        enterButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                host = hostText.getText();
                port = portText.getText();

                textArea.append("Scanning host " + host + "\n");
                ScanGUI scanner = new ScanGUI();
                String trim = port.trim();

                if (trim.contains("-")) {

                    String[] ports = trim.split("-");
                    try {
                        int minPort = Integer.parseInt(ports[0]);
                        int maxPort = Integer.parseInt(ports[1]);
                        scanner.setMinPort(minPort);
                        scanner.setMaxPort(maxPort);
                    } catch (NumberFormatException nfe) {
                        textArea.setText("Wrong ports!");
                        return;
                    }
                } else {
                    try {
                        scanner.setMinPort(Integer.parseInt(port));
                        scanner.setMaxPort(Integer.parseInt(port));
                    } catch (NumberFormatException nfe) {
                        textArea.setText("Wrong ports!");
                        return;

                    }
                }

                scanner.setHost(host);
                List<Integer> openPortsList = scanner.scan();
                if (openPortsList != null) {
                    if (openPortsList.size() > 0) {
                        textArea.append("List of opened ports:\n");
                        for (Integer openedPort : openPortsList) {
                            textArea.append(openedPort.toString() + "\n");
                        }
                    } else {
                        textArea.append("No opened ports!\n");
                    }
                } else {
                    textArea.append("Error happened!\n");
                }
            }
        });
    }

    private static void placeComponents(JPanel panel) {
        panel.setLayout(null);

        hostLabel.setBounds(10, 20, 130, 25);
        panel.add(hostLabel);

        hostText.setBounds(140, 20, 165, 25);
        panel.add(hostText);

        portLabel.setBounds(10, 50, 80, 25);
        panel.add(portLabel);

        portText.setBounds(140, 50, 165, 25);
        panel.add(portText);

        enterButton.setBounds(10, 80, 80, 25);
        panel.add(enterButton);

        textArea.setBounds(0, 110, 500, 390);
        textArea.setEditable(false);
        panel.add(textArea);
    }
}
