import org.json.JSONArray;
import org.json.JSONObject;

public class JsonToString {
    public void leer(String msj){

        JSONObject jsonObject = new JSONObject(msj);

        for (String key : jsonObject.keySet()) {
            Object value = jsonObject.get(key);
            System.out.println(key + ": " + value);
        }

    }

}
