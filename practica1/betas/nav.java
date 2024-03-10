import java.io.File;

public class nav {
    public static void main(String[] args) {
        System.out.println("Hola Mundo");

        // Obtener la ruta del directorio actual
        String directorioActual = System.getProperty("user.dir");

        // Crear un objeto File con la ruta del directorio actual
        File folder = new File(directorioActual);

        // Verificar si el directorio existe
        if (folder.exists() && folder.isDirectory()) {
            // Obtener la lista de archivos y carpetas en el directorio actual
            File[] files = folder.listFiles();

            // Iterar sobre los archivos y carpetas
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
}