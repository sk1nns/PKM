package pr07.client;

import java.net.*;

public class MulticastClient {
    private static String group;
    private static InetAddress address;
    private static byte[] buffer;
    private static DatagramPacket packet;
    private static String str;
    private static MulticastSocket socket;

    public static void main(String args[]) throws Exception {
        group = "224.0.0.0";
        System.out.println("Очікування повідомлення від сервера");
        try {
            socket = new MulticastSocket(1502);
            address = InetAddress.getByName(group);
            socket.joinGroup(address);
            while (true) {
                buffer = new byte[1024];
                packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);
                str = new String(packet.getData());
                System.out.println("Отримано повідомлення: " + str.trim());
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        } finally {
            try {
                socket.leaveGroup(address);
                socket.close();
            } catch (Exception e) {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
        }
    }
}
