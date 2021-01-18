package pr07.server;

import java.io.*;
import java.net.*;

public class MulticastServer {
    private BufferedReader in = null;
    private String str = null;
    private byte[] buffer;
    private String group;
    private DatagramPacket packet;
    private InetAddress address;
    private DatagramSocket socket;

    public MulticastServer() throws IOException {
        System.out.println("Sending messages");
        socket = new DatagramSocket();
        this.group = "224.0.0.0";
        transmit();
    }

    public void transmit() {
        try {
            in = new BufferedReader(new InputStreamReader(System.in));
            while (true) {
                System.out.println("Введіть групове повідомлення: ");
                str = in.readLine();
                buffer = str.getBytes();
                address = InetAddress.getByName(group);
                packet = new DatagramPacket(buffer, buffer.length, address, 1502);
                socket.send(packet);
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        } finally {
            try {
                in.close();
                socket.close();
            } catch (Exception e) {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
        }
    }

    public static void main(String args[]) throws Exception {
        new MulticastServer();
    }
}
