import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class Conexion {

    private Socket socket;
    private PrintWriter salida;
    private BufferedReader entrada;
    String hostName = "localhost";
    int portNumber = 8080;

    public void SocketCliente(int opcion){


        try{
            socket = new Socket(hostName, portNumber);

            entrada = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            salida = new PrintWriter(socket.getOutputStream(), true);
            System.out.println("Conexion establecida con el servidor");


            salida.println(opcion);
            System.out.println("1. Mensaje enviado al servidor");

            // voy a leer una cadena que tiene saltos de linea
            String mensaje = entrada.readLine();
            System.out.println("Mensaje del servidor: ");
            JsonToString pasar = new JsonToString();
            pasar.leer(mensaje);

            socket.close();



        } catch (UnknownHostException e) {
            System.err.println("Don't know about host " + hostName);
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to " + hostName);
            System.exit(1);
        }
    }

}
