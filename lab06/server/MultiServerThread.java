package lab06.server;

import java.io.*;
import java.net.*;

public class MultiServerThread extends Thread {
    private PrintStream printStream = null;
    private BufferedReader bufferedReader = null;
    private InetAddress inetAddress = null;

    public MultiServerThread(Socket s) throws IOException {
        printStream = new PrintStream(s.getOutputStream());
        bufferedReader = new BufferedReader(
                new InputStreamReader(s.getInputStream()));
        inetAddress = s.getInetAddress();
    }

    public void disconnect() {
        try {
            System.out.println(inetAddress.getHostName() + " disconnected");
            printStream.close();
            bufferedReader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        finally{
            this.interrupt();
        }
    }

    public void run (){
        int i = 0;
        String str = null;
        try {
            while ((bufferedReader.readLine())!= null){
                if ("PING".equalsIgnoreCase(str)) {
                    printStream.println("PONG " + ++i);
                }
                System.out.println("PING-PONG " + i + " with " + inetAddress.getHostName());
            }
        } catch (IOException e) {
            System.out.println("Disconnect");
        }
        finally{
            disconnect();
        }
    }
}
