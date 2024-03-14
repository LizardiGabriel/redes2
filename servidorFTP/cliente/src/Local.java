import java.io.File;
import java.util.Scanner;

public class Local {
    public void listarContenido() {
        // ver los archivos o carpetas almacenadas en la carpeta donde se esta ejecutando la aplicacion java
        String directorioActual = System.getProperty("user.dir");
        File folder = new File(directorioActual);

        if (folder.exists() && folder.isDirectory()) {
            File[] files = folder.listFiles();
            if (files != null) {
                for (File file : files) {
                    if (file.isDirectory()) {
                        System.out.println("Carpeta: " + file.getName());
                    } else {
                        System.out.println("Archivo: " + file.getName());
                    }
                }
            } else {
                System.out.println("El directorio está vacío.");
            }
        } else {
            System.out.println("El directorio actual no existe o no es un directorio válido.");
        }


    }

    public void crearCarpeta() {
        // crear una carpeta en el directorio donde se esta ejecutando la aplicacion java
    }

    public void eliminarCarpeta() {
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
}
