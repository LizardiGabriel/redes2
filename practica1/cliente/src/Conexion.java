import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;

public class Conexion {

    private Socket socket;
    private PrintWriter salida;
    private BufferedReader entrada;
    String hostName = "localhost";
    int portNumber = 8080;
    int portNumber2 = 8081;

    public void SocketCliente(String env){


        try{
            socket = new Socket(hostName, portNumber);

            entrada = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            salida = new PrintWriter(socket.getOutputStream(), true);

            int tam = env.length();
            salida.println(tam);
            salida.println(env);

            String mensaje = entrada.readLine();
            System.out.println("\n");
            JsonToString pasar = new JsonToString();
            pasar.leer(mensaje);

            socket.close();


        } catch (UnknownHostException e) {
            System.err.println(e.getMessage());
            System.exit(1);
        } catch (IOException e) {
            System.err.println(e.getMessage());

        }

    }


    public void enviarArchivo(String rutitaNueva) {
        System.out.println("Enviando archivo...");

        FileInputStream fileInputStream = null;
        BufferedOutputStream bufferedOutputStream = null;

        // enviar archivo
        try{
            socket = new Socket(hostName, portNumber2);
            File file = new File(rutitaNueva);
            if (!file.exists()) {
                System.err.println("El archivo no existe");
                return;
            }

            OutputStream outputStream = socket.getOutputStream();
            bufferedOutputStream = new BufferedOutputStream(outputStream);

            fileInputStream = new FileInputStream(file);
            BufferedInputStream bufferedInputStream = new BufferedInputStream(fileInputStream);


            byte[] buffer = new byte[1024];
            int bytesRead;
            while ((bytesRead = bufferedInputStream.read(buffer)) != -1) {
                bufferedOutputStream.write(buffer, 0, bytesRead);
            }
            bufferedOutputStream.flush();
            System.out.println("Archivo enviado exitosamente");

            // recibir respuesta
            entrada = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String mensaje = entrada.readLine();
            System.out.println("\n");
            JsonToString pasar = new JsonToString();
            pasar.leer(mensaje);



        }catch (UnknownHostException e) {
            System.err.println("Don't know about host " + hostName);
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to " + hostName);
            System.exit(1);
        }finally {

            try {
                // Cerrar los flujos y el socket
                if (bufferedOutputStream != null) {
                    bufferedOutputStream.close();
                }
                if (fileInputStream != null) {
                    fileInputStream.close();
                }
                if (socket != null) {
                    socket.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }

    public void recibirArchivo(String localPath, String archivo) {
        System.out.println("Recibiendo archivo...");

        try {
            socket = new Socket(hostName, portNumber2);
            InputStream inputStream = socket.getInputStream();
            BufferedInputStream bufferedInputStream = new BufferedInputStream(inputStream);

            byte[] buffer = new byte[1024];
            int bytesRead;
            String ruta = localPath + "/" + archivo;
            FileOutputStream fileOutputStream = new FileOutputStream(ruta);
            BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(fileOutputStream);

            while ((bytesRead = bufferedInputStream.read(buffer)) != -1) {
                bufferedOutputStream.write(buffer, 0, bytesRead);
            }
            bufferedOutputStream.flush();
            System.out.println("Archivo recibido exitosamente");

            // enviar respuesta
            salida = new PrintWriter(socket.getOutputStream(), true);
            salida.println("Archivo recibido exitosamente");

            // cerrar flujos y socket
            bufferedOutputStream.close();
            fileOutputStream.close();
            bufferedInputStream.close();
            inputStream.close();
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
