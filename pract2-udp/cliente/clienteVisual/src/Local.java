import java.io.File;
import java.util.Scanner;

public class Local {

    public String crearCarpeta(String localPath, String carpeta) {
        // crear una carpeta en localpath
        String path1 = localPath+"/"+ carpeta;
        File directorio = new File(path1);


        if (directorio.exists() && directorio.isDirectory()) {
            return "La carpeta ya existe.";



        } else {
            if(directorio.mkdir()){
                return "La carpeta ha sido creada.";
            }else{
                return "Error al crear la carpeta.";
            }
        }




    }

    public String eliminarCarpeta(String path, String carpeta) {
        // eliminar una carpeta en localpath

        String path1 = path+"/"+ carpeta;
        File directorio = new File(path1);


        if (directorio.exists() && directorio.isDirectory()) {
            if(directorio.delete()){
                return "La carpeta "+ path1+" ha sido eliminada.";
            }else{
                return "Error al eliminar la carpeta.";
            }
        }if(directorio.exists() && directorio.isFile()) {
            if(directorio.delete()){
                return "archivo eliminado: " + path1;
            }else{
                return "Error al eliminar el archivo.";
            }
        } else {
            return "error";

        }
    }

    public String[] cambiarDirectorio(String localPath, String carpeta) {

        String ret[] = new String[2];

        if(carpeta.equals("..")){
            String path1 = localPath.substring(0, localPath.lastIndexOf("/"));
            ret[0] = "Cambiando de directorio.";
            ret[1] = path1;
            return ret;

        }else if(carpeta.equals(".")){
            ret[0] = "Directorio actual.";
            ret[1] = localPath;
            return ret;

        }else{
            String path1 = localPath+"/"+ carpeta;
            File directorio = new File(path1);

            if (directorio.exists() && directorio.isDirectory()) {
                ret[0] = "La carpeta existe.";
                ret[1] = path1;
                return ret;

            } else {
                ret[0] = "La carpeta no existe.";
                ret[1] = localPath;
                return ret;
            }

        }




    }

    public String getDirectorio() {
        return System.getProperty("user.dir");
    }


    public static String imprimirDirectorio(String ruta, String espacio) {
        String separador = "        ";
        File dir = new File(ruta);
        File[] files = dir.listFiles();
        StringBuilder resultado = new StringBuilder();

        if (files == null) {
            resultado.append("Error: No se puede acceder al directorio ").append(ruta);
            return resultado.toString();
        }

        for (File file : files) {
            if (file.isDirectory()) {
                String nombreDirectorio = file.getName() + "/";
                resultado.append(espacio).append("├─── ").append(nombreDirectorio).append("\n");
                resultado.append(imprimirDirectorio(file.getAbsolutePath(), espacio + "│" + separador));
            } else {
                resultado.append(espacio).append("└─── ").append(file.getName()).append("\n");
            }
        }

        return resultado.toString();
    }




}
