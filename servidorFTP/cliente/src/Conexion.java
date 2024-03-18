import org.json.JSONObject;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;

import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Conexion {

    private Socket socket;
    private PrintWriter salida;
    private BufferedReader entrada;
    String hostName = "localhost";
    int portNumber = 1234;
    int portNumber2 = 1235;
    int portNumber3 = 1236;

    public String SocketCliente(String env){
        String regreso = "";
        JSONObject jsonObject = new JSONObject();




        try{
            socket = new Socket(hostName, portNumber);

            entrada = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            salida = new PrintWriter(socket.getOutputStream(), true);

            salida.println(env);

            String mensaje = entrada.readLine();
            System.out.println("\n");


            socket.close();

            return mensaje;


        } catch (UnknownHostException e) {
            System.err.println(e.getMessage());
            System.exit(1);
        } catch (IOException e) {
            System.err.println(e.getMessage());

        }
        return "";

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

    public void recibirArchivo(String localPath, String archivo, int tamFile, String typeFile){
        System.out.println("Recibiendo archivo...");

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
            System.out.println("Archivo recibido exitosamente");

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
            System.err.println("Don't know about host " + hostName);
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to " + hostName);
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
        e.printStackTrace();
    }
}

}
