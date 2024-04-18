import org.json.JSONObject;

import java.io.*;
import java.net.*;

import java.nio.ByteBuffer;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Conexion {

    private Socket socket;
    private PrintWriter salida;
    private BufferedReader entrada;
    String hostName = "192.168.0.153";
    int portNumber = 1234;
    int portNumber2 = 1235;
    int portNumber3 = 1236;

    Ventana ventana = new Ventana();

    public String SocketCliente(String env){
        String regreso = "";
        JSONObject jsonObject = new JSONObject();




        try{
            socket = new Socket(hostName, portNumber);

            entrada = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            salida = new PrintWriter(socket.getOutputStream(), true);
            salida.println(env);
            String mensaje = entrada.readLine();
            socket.close();

            return mensaje;


        } catch (UnknownHostException e) {

            ventana.mensaje("Don't know about host " + hostName);
        } catch (IOException e) {
            ventana.mensaje("Couldn't get I/O for the connection to " + hostName);


        }
        return "";

    }

    public void enviarArchivoUDP(String rutitaNueva, int tamBuffer, int tamVentana) {
        ventana.mensaje("Enviando archivo...");

        try {
            File file = new File(rutitaNueva);
            if (!file.exists()) {
                System.out.println("File not found: " + rutitaNueva);
                return;
            }

            byte[] buffer = new byte[tamBuffer];
            DatagramSocket socket = new DatagramSocket();
            InetAddress serverAddress = InetAddress.getByName(hostName);

            FileInputStream fileInputStream = new FileInputStream(file);
            BufferedInputStream bufferedInputStream = new BufferedInputStream(fileInputStream);


            int ventanaInicio = 0;
            int ventanaFin = tamVentana-1;

            int finArchivo = 0;
            int contador = 0;

            int totalBytesSend = 0;

            while(finArchivo == 0){
                int bytesRead;
                int i = 0;
                while (i< tamVentana) {
                    bytesRead = bufferedInputStream.read(buffer);
                    if(contador >= ventanaInicio && contador <= ventanaFin && bytesRead != -1) {
                        DatagramPacket packet = new DatagramPacket(buffer, bytesRead, serverAddress, portNumber2);
                        socket.send(packet);
                        System.out.println("envie " + contador);
                        contador++;
                        totalBytesSend += bytesRead;
                        if(totalBytesSend >= file.length()){
                            finArchivo = 1;
                            System.out.println("fin archivo, acabeeee");
                            break;

                        }
                    }
                    i++;
                }

                // recibirRespuesta
                byte[] buffer2 = new byte[1024];
                DatagramPacket packet2 = new DatagramPacket(buffer2, buffer2.length);
                socket.receive(packet2);

                String mensaje = new String(packet2.getData(), 0, packet2.getLength());
                int sigVentana = Integer.parseInt(mensaje);

                System.out.println("socket udp resp: "+sigVentana);
                System.out.println("ventanaFin+1: "+(ventanaFin+1));

                if(sigVentana == ventanaFin+1){
                    System.out.println("correcto");
                    ventanaInicio = ventanaFin+1;
                    ventanaFin = ventanaInicio + tamVentana-1;

                }else{
                    System.out.println("incorrecto");
                    ventanaInicio = sigVentana;

                }
                if(ventanaInicio >= file.length()){
                    finArchivo = 1;
                    System.out.println("fin archivo, acabeeee");
                }


            }

            bufferedInputStream.close();
            socket.close();

            System.out.println("File sent successfully.");

        } catch (IOException e) {
            e.printStackTrace();
        }
    }



    public void enviarArchivo(String rutitaNueva, int tamBuffer, int tamVentana) {

        ventana.mensaje("Enviando archivo...");
        FileInputStream fileInputStream = null;
        BufferedOutputStream bufferedOutputStream = null;

        // enviar archivo
        try{
            socket = new Socket(hostName, portNumber2);
            File file = new File(rutitaNueva);
            if (!file.exists()) {
                ventana.mensaje("El archivo no existe");
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
            ventana.mensaje("Archivo enviado exitosamente");

            // recibir respuesta
            entrada = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String mensaje = entrada.readLine();
            System.out.println("\n");
            JsonToString pasar = new JsonToString();
            pasar.leer(mensaje);



        }catch (UnknownHostException e) {
            ventana.mensaje("Don't know about host " + hostName);
            System.exit(1);
        } catch (IOException e) {
            ventana.mensaje("Couldn't get I/O for the connection to " + hostName);
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
                ventana.mensaje("Error al cerrar los flujos y el socket: " + e.getMessage());
            }

        }
    }

    public void recibirArchivo(String localPath, String archivo, int tamFile, String typeFile){
        ventana.mensaje("Recibiendo archivo...");

        try {
            socket = new Socket(hostName, portNumber3);
            InputStream inputStream = socket.getInputStream();
            BufferedInputStream bufferedInputStream = new BufferedInputStream(inputStream);

            byte[] buffer = new byte[tamFile]; // Utilizar el tamaño especificado para el buffer
            int bytesRead;
            String ruta = localPath + "/" + archivo;
            if (typeFile.equals("fldr")) {
                ruta = localPath + "/" + archivo + ".zip";

            }
            FileOutputStream fileOutputStream = new FileOutputStream(ruta);
            BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(fileOutputStream);

            while ((bytesRead = bufferedInputStream.read(buffer)) != -1) {
                bufferedOutputStream.write(buffer, 0, bytesRead);
            }
            bufferedOutputStream.flush();
            ventana.mensaje("Archivo recibido exitosamente");

            if (typeFile.equals("fldr")) {
                // descomprimir archivo
                descomprimir(ruta, localPath+"/"+archivo);
            }

            // cerrar flujos y socket
            bufferedOutputStream.close();
            fileOutputStream.close();
            bufferedInputStream.close();
            inputStream.close();
            socket.close();

        } catch (UnknownHostException e) {
            ventana.mensaje("Don't know about host " + hostName);
            System.exit(1);
        } catch (IOException e) {
            ventana.mensaje("Couldn't get I/O for the connection to " + hostName);
            System.exit(1);
        }
    }



    public void descomprimir(String rutaZip, String rutaDestino) {
        // crear la carpeta destino
        File folder1 = new File(rutaDestino);
        if (!folder1.exists()) {
            folder1.mkdirs();
        }
        // close folder
        folder1 = null;


    byte[] buffer = new byte[1024];
    try {
        // Create a ZipInputStream to read the zip file
        ZipInputStream zis = new ZipInputStream(new FileInputStream(rutaZip));
        // Get the next entry from the zip file
        ZipEntry zipEntry = zis.getNextEntry();
        while (zipEntry != null) {
            // Create a new File for each ZipEntry
            File newFile = new File(rutaDestino + File.separator + zipEntry.getName());
            // If the entry is a directory, create the directory
            if (zipEntry.isDirectory()) {
                newFile.mkdirs();
            } else {
                // If the entry is a file, write the file
                new FileOutputStream(newFile).write(zis.readAllBytes());
            }
            // Close the current entry
            zis.closeEntry();
            // Get the next entry
            zipEntry = zis.getNextEntry();
        }
        // Close the last entry
        zis.closeEntry();
        // Close the zip file
        zis.close();

        // eliminar el .zip
        File file3 = new File(rutaZip);
        file3.delete();
        file3 = null;

    } catch (IOException e) {
        ventana.mensaje("Error al descomprimir el archivo: " + e.getMessage());
    }
}

}
