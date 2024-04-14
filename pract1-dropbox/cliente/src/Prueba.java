import org.json.JSONObject;

public class Prueba {
    public void leerJson(String msj){
        JSONObject jsonObject = new JSONObject(msj);
        for (String key : jsonObject.keySet()) {
            Object value = jsonObject.get(key);
            System.out.println(key + ": " + value);
        }
    }
}
