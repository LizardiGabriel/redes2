import java.io.File;
import java.util.Scanner;

public class Local {
    public void listarContenido(String localPath) {
        // ver los archivos o carpetas almacenadas en la carpeta donde se esta ejecutando la aplicacion java
        String ruta = localPath;

        System.out.println("\nContenido del directorio local: \n");
        System.out.println(ruta);
        imprimirDirectorio(ruta, "  ");



    }

    public void crearCarpeta(String localPath) {
        // crear una carpeta en localpath
        System.out.print("Escribe el nombre de la carpeta que quieres crear: ");
        Scanner scanner = new Scanner(System.in);
        String carpeta = scanner.nextLine();

        String path1 = localPath+"/"+ carpeta;
        File directorio = new File(path1);


        if (directorio.exists() && directorio.isDirectory()) {
            System.out.println("La carpeta ya existe.");



        } else {
            if(directorio.mkdir()){
                System.out.println("La carpeta ha sido creada.");
            }else{
                System.out.println("Error al crear la carpeta.");
            }
        }



    }

    public void eliminarCarpeta(String path) {
        // eliminar una carpeta en localpath
        System.out.print("Escribe el nombre de la carpeta que quieres eliminar: ");
        Scanner scanner = new Scanner(System.in);
        String carpeta = scanner.nextLine();

        String path1 = path+"/"+ carpeta;
        File directorio = new File(path1);


        if (directorio.exists() && directorio.isDirectory()) {
            if(directorio.delete()){
                System.out.println("La carpeta ha sido eliminada.");
            }else{
                System.out.println("Error al eliminar la carpeta.");
            }
        } else {
            System.out.println("La carpeta no existe.");
        }
    }

    public String cambiarDirectorio(String localPath) {
        System.out.print("Escribe el nombre de la carpeta a la que quieres cambiar: ");
        Scanner scanner = new Scanner(System.in);
        String carpeta = scanner.nextLine();

        String path1 = localPath+"/"+ carpeta;

        File directorio = new File(path1);

        if (directorio.exists() && directorio.isDirectory()) {
            System.out.println("La carpeta existe.");
            return path1;
            // Aquí puedes agregar la lógica para cambiar al directorio, por ejemplo:
            // System.setProperty("user.dir", directorio.getAbsolutePath());
        } else {
            System.out.println("La carpeta no existe.");
            return localPath;
            // Aquí puedes agregar la lógica para manejar el caso en que la carpeta no existe
        }


    }

    public String getDirectorio() {
        return System.getProperty("user.dir");
    }


    public static void imprimirDirectorio(String ruta, String espacio) {
        File dir = new File(ruta);
        File[] files = dir.listFiles();

        if (files == null) {
            System.err.println("Error: No se puede acceder al directorio " + ruta);
            return;
        }

        for (File file : files) {
            if (file.isFile()) {
                System.out.println(espacio  + file.getName());
            }
        }

        for (File file : files) {
            if (file.isDirectory()) {
                System.out.println(espacio  + file.getName() + "/");
                imprimirDirectorio(file.getAbsolutePath(), espacio + "  ");
            }
        }
    }



}
