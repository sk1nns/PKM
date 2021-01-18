package lab07.client;

import java.io.*;
import java.net.*;

public class MultiThreadClient {
    public static void main(String[] args) {
        InetAddress inetAddress = null;
        Socket socket = null;
        InputStreamReader inputStreamReader = null;
        BufferedReader bufferedReader = null;
        PrintStream printStream = null;
        try {
            inetAddress = InetAddress.getLocalHost();
            int port = 8071;
            socket = new Socket(inetAddress, port);
            printStream = new PrintStream(socket.getOutputStream());
            inputStreamReader = new InputStreamReader(socket.getInputStream());
            bufferedReader = new BufferedReader(inputStreamReader);

            for (int i = 1; i <= 10; ++i){
                printStream.println("PING");
                System.out.println(bufferedReader.readLine());
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
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
