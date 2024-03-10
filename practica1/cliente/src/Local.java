import java.io.File;
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

    public void cambiarDirectorio() {
    }
}
