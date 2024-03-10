import java.io.IOException;
import java.util.Scanner;
import org.json.JSONArray;
import org.json.JSONObject;

public class Remoto {
    public void listarContenido() throws IOException {
        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "list1");
        System.out.println("mensaje a enviar: " + jsonObject.toString());
        conn.SocketCliente(jsonObject.toString());

    }

    public void crearCarpeta() {
    }

    public void eliminarCarpeta() {
    }

    public void cambiarDirectorio() {

        System.out.print("Escribe el nombre de la carpeta a la que quieres cambiar: ");
        Scanner scanner = new Scanner(System.in);
        String carpeta = scanner.nextLine();

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "cd4");
        jsonObject.put("carpeta", carpeta);
        System.out.println("mensaje a enviar: " + jsonObject.toString());

        Conexion conn = new Conexion();
        conn.SocketCliente(jsonObject.toString());





    }

    public void enviarArchivo() {
    }

    public void recibirArchivo() {
    }

    public void getDirectorio() {

        Conexion conn = new Conexion();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("opcion", "list0");
        System.out.println("mensaje a enviar: " + jsonObject.toString());
        conn.SocketCliente(jsonObject.toString());



    }
}
