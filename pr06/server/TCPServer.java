package pr06.server;

import java.io.IOException;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;

public class TCPServer {
    public static void main(String[] args) {
        Socket socket = null;
        ServerSocket servSocket = null;
        PrintStream printStream = null;
        int port = 8030;
        try {
            servSocket = new ServerSocket(port);
            socket = servSocket.accept();
            System.out.println("Connection established [" + socket.getInetAddress() + "]");
            printStream = new PrintStream(socket.getOutputStream());
            printStream.println("Hello");
            printStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            printStream.close();
            try {
                socket.close();
            } catch (IOException e1) {
                e1.printStackTrace();
            }
            try {
                servSocket.close();
            } catch (IOException e2) {
                e2.printStackTrace();
            }
        }
    }
}
