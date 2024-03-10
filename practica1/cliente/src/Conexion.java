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

    public void SocketCliente(String env){


        try{
            socket = new Socket(hostName, portNumber);

            entrada = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            salida = new PrintWriter(socket.getOutputStream(), true);
            ///System.out.println("Conexion establecida con el servidor");

            //size o ennv
            int tam = env.length();
            salida.println(tam);

            long veces = tam/255;
            // redondear hacia arriba
            if(tam%255 != 0){
                veces++;
            }
            salida.println(veces);

            salida.println(env);

            /*for (int i = 0; i < veces; i++) {
                salida.println(env.substring(i*255, Math.min(tam, (i+1)*255)));
            }

             */

            //System.out.println("1. Mensaje enviado al servidor");

            // voy a leer una cadena que tiene saltos de linea
            String mensaje = entrada.readLine();
            System.out.println("\n");
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
