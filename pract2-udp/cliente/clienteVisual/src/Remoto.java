import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;



public class Remoto {
    public String  listarContenido() throws IOException {
        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "list1");
        String a = conn.SocketCliente(jsonObject.toString());

        JsonToString pasar = new JsonToString();
        return pasar.list(a);


    }

    public String crearCarpeta(String carpeta) {

        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "mkdir2");
        jsonObject.put("carpeta", carpeta);
        String a = conn.SocketCliente(jsonObject.toString());
        JsonToString pasar = new JsonToString();
        return pasar.leer(a);


    }

    public String eliminarAlgo(String archivo) {

        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "rmdir3");
        jsonObject.put("archivo", archivo);
        String a = conn.SocketCliente(jsonObject.toString());
        JsonToString pasar = new JsonToString();
        return pasar.leer(a);

    }

    public String cambiarDirectorio(String carpeta) {

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "cd4");
        jsonObject.put("carpeta", carpeta);

        Conexion conn = new Conexion();
        String a = conn.SocketCliente(jsonObject.toString());
        JsonToString pasar = new JsonToString();
        return pasar.leer(a);

    }

    public String enviarArchivo(String ruta, String archivo) {

        String rutita = ruta + "/" + archivo;
        String rutitaNueva = ruta + "/" + archivo + ".zip";

        String queEs = comprimir(rutita, rutitaNueva);

        File file = new File(rutitaNueva);
        if (!file.exists()) {
            return "El archivo no existe";
        }

        // tam del archivo en bytes
        long tam = file.length();

        String tamString = String.valueOf(tam);

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "put5");
        jsonObject.put("nombreOrig", archivo);
        jsonObject.put("tam", tamString);
        jsonObject.put("type", queEs);

        Ventana ventana = new Ventana();
        int tamBuffer = ventana.seleccionBuff(tam, "buffer");
        int tamVentana = ventana.seleccionVentana(tam, tamBuffer, "tam de ventana");


        String tamBufferString = String.valueOf(tamBuffer);
        String tamVentanaString = String.valueOf(tamVentana);

        jsonObject.put("tamBuffer", tamBufferString);
        jsonObject.put("tamVentana", tamVentanaString);

        Conexion conn = new Conexion();
        String a = conn.SocketCliente(jsonObject.toString());
        JsonToString pasar = new JsonToString();
        pasar.leer(a);


        Conexion conn2 = new Conexion();
        // cambiar aqui para udp o tcp
        conn2.enviarArchivo(rutitaNueva, tamBuffer, tamVentana);
        //conn2.enviarArchivoUDP(rutitaNueva, tamBuffer, tamVentana);


        // eliminar el .zip
        try {
            file.delete();
        } catch (Exception e) {
            ventana.mensaje("Error al eliminar el archivo: " + e.getMessage());

        }
        return "Archivo enviado exitosamente.";


    }


    String comprimir(String ruta, String rutaNueva)  {
        String retorno = "";
        try {

            File file = new File(ruta);
            if (file.isDirectory()) {
                retorno = "fldr";
                pack(ruta, rutaNueva);

            }else{
                retorno = "file";

                File sourceFile = new File(ruta);
                FileOutputStream fos = new FileOutputStream(rutaNueva);
                ZipOutputStream zipOut = new ZipOutputStream(fos);
                FileInputStream fis = new FileInputStream(sourceFile);
                ZipEntry zipEntry = new ZipEntry(sourceFile.getName());
                zipOut.putNextEntry(zipEntry);

                final byte[] bytes = new byte[1024];
                int length;
                while ((length = fis.read(bytes)) >= 0) {
                    zipOut.write(bytes, 0, length);
                }

                fis.close();
                zipOut.close();
                fos.close();
                //System.out.println("Archivo comprimido exitosamente.");

            }
        } catch(
            IOException e)

        {
            Ventana ventana = new Ventana();
            ventana.mensaje("Error al comprimir el archivo: " + e.getMessage());
        }
        return retorno;
    }

    public String recibirArchivo(String localPath, String archivo) {
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "get6");
        jsonObject.put("nombreOrig", archivo);


        Conexion conn = new Conexion();
        String msj = conn.SocketCliente(jsonObject.toString());
        JsonToString pasar = new JsonToString();
        pasar.leer(msj);

        JSONObject jsonObject1 = new JSONObject(msj);
        String regreso = jsonObject1.getString("mensaje");
        String tipo = jsonObject1.getString("typeFile");
        int tam = Integer.parseInt(regreso);

        Conexion conn2 = new Conexion();
        conn2.recibirArchivo(localPath, archivo, tam, tipo);

        return "Archivo recibido exitosamente.";
    }

    public String getDirectorio() {

        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "list0");
        String a = conn.SocketCliente(jsonObject.toString());
        JsonToString pasar = new JsonToString();
        return  pasar.leer(a);




    }


    public static void pack(String sourceDirPath, String zipFilePath) throws IOException {
        Path p = Files.createFile(Paths.get(zipFilePath));
        try (ZipOutputStream zs = new ZipOutputStream(Files.newOutputStream(p))) {
            Path pp = Paths.get(sourceDirPath);
            Files.walk(pp)
                    .filter(path -> !Files.isDirectory(path))
                    .forEach(path -> {
                        ZipEntry zipEntry = new ZipEntry(pp.relativize(path).toString());
                        try {
                            zs.putNextEntry(zipEntry);
                            Files.copy(path, zs);
                            zs.closeEntry();
                        } catch (IOException e) {
                            Ventana ventana = new Ventana();
                            ventana.mensaje("Error al pack: " + e.getMessage());
                        }
                    });
        }
    }






}

