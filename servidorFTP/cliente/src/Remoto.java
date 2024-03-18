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
    public void listarContenido() throws IOException {
        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "list1");
        //System.out.println("mensaje a enviar: " + jsonObject.toString());
        String a = conn.SocketCliente(jsonObject.toString());

    }

    public void crearCarpeta() {

        System.out.print("Escribe el nombre de la carpeta que quieres crear: ");
        Scanner scanner = new Scanner(System.in);
        String carpeta = scanner.nextLine();

        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "mkdir2");
        jsonObject.put("carpeta", carpeta);
        String a = conn.SocketCliente(jsonObject.toString());

    }

    public void eliminarAlgo() {

        System.out.print("Escribe el nombre de la carpeta o archivo que quieres eliminar: ");
        Scanner scanner = new Scanner(System.in);
        String archivo = scanner.nextLine();

        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "rmdir3");
        jsonObject.put("archivo", archivo);
        String a = conn.SocketCliente(jsonObject.toString());
    }

    public void cambiarDirectorio() {

        System.out.print("Escribe el nombre de la carpeta a la que quieres cambiar: ");
        Scanner scanner = new Scanner(System.in);
        String carpeta = scanner.nextLine();

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "cd4");
        jsonObject.put("carpeta", carpeta);
        //System.out.println("mensaje a enviar: " + jsonObject.toString());

        Conexion conn = new Conexion();
        String a = conn.SocketCliente(jsonObject.toString());

    }

    public void enviarArchivo(String ruta) {
        System.out.print("Escribe el nombre del archivo que quieres enviar: ");
        Scanner scanner = new Scanner(System.in);
        String archivo = scanner.nextLine();
        String rutita = ruta + "/" + archivo;
        String rutitaNueva = ruta + "/" + archivo + ".zip";

        String queEs = comprimir(rutita, rutitaNueva);

        File file = new File(rutitaNueva);
        if (!file.exists()) {
            System.err.println("El archivo no existe");
            return;
        }

        // tam del archivo en bytes
        long tam = file.length();

        String tamString = String.valueOf(tam);

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "put5");
        jsonObject.put("nombreOrig", archivo);
        jsonObject.put("tam", tamString);
        jsonObject.put("type", queEs);

        Conexion conn = new Conexion();
        String a = conn.SocketCliente(jsonObject.toString());

        Conexion conn2 = new Conexion();
        conn2.enviarArchivo(rutitaNueva);


        // eliminar el .zip
        try {
            file.delete();
        } catch (Exception e) {
            System.err.println("Error al eliminar el archivo: " + e.getMessage());
        }


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
                System.out.println("Archivo comprimido exitosamente.");

            }
        } catch(
            IOException e)

        {
           System.err.println("Error al comprimir el archivo: " + e.getMessage());
        }
        return retorno;
    }

    public void recibirArchivo(String localPath) {
        System.out.print("Escribe el nombre del archivo que quieres recibir: ");
        Scanner scanner = new Scanner(System.in);
        String archivo = scanner.nextLine();

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "get6");
        jsonObject.put("nombreOrig", archivo);


        Conexion conn = new Conexion();
        String msj = conn.SocketCliente(jsonObject.toString());
        JSONObject jsonObject1 = new JSONObject(msj);
        String regreso = jsonObject1.getString("mensaje");
        String tipo = jsonObject1.getString("typeFile");
        int tam = Integer.parseInt(regreso);

        Conexion conn2 = new Conexion();
        conn2.recibirArchivo(localPath, archivo, tam, tipo);
    }

    public void getDirectorio() {

        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "list0");
        //System.out.println("mensaje a enviar: " + jsonObject.toString());
        String a = conn.SocketCliente(jsonObject.toString());



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
                            System.err.println(e);
                        }
                    });
        }
    }






}

