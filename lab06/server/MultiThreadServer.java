package lab06.server;

import java.io.*;
import java.net.*;

public class MultiThreadServer {
    public static void main(String[] args) {
        ServerSocket servSocket = null;
        int port = 8071;
        try {
            System.out.println("Initialize...");
            servSocket = new ServerSocket(port);
            System.out.println("Waiting incomming connections");
            while (true){
                Socket s = servSocket.accept();
                System.out.println("["+s.getInetAddress().getHostName()+ "] connected");
                ServerThread srvThread = new ServerThread(s);
                srvThread.start();
            }
        } catch (Exception e) {
            System.err.println(e);
        }
        finally{
            try {
                servSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
