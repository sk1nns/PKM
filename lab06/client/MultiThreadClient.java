package lab06.client;

import java.io.*;
import java.net.*;

public class MultiThreadClient {
    public static void main(String[] args) {
        InetAddress addr=null;
        Socket s = null;
        InputStreamReader isr = null;
        BufferedReader br = null;
        PrintStream ps = null;

        try {
            addr = InetAddress.getLocalHost();
            int port = 8071;
            s = new Socket(addr,port);
            ps = new PrintStream(s.getOutputStream());
            isr = new InputStreamReader(s.getInputStream());
            br = new BufferedReader(isr);

            for (int i = 1; i <= 10; ++i){
                ps.println("PING");
                System.out.println(br.readLine());
                Thread.sleep(100);
            }
        } catch (UnknownHostException e) {
            System.out.println("Сервер недоступний");
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println("Помилка введення/виведення");
            e.printStackTrace();
        } catch (InterruptedException e) {
            System.out.println("Помилка потоку виконання");
            e.printStackTrace();
        }
        finally{
            try {
                s.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
