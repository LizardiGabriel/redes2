import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class JsonToString {
    public String leer(String msj){

        try {
            JSONObject jsonObject = new JSONObject(msj);
            String prettyJson = (jsonObject.toString(4));

            if(jsonObject.has("path remoto")) {
                return jsonObject.getString("path remoto");
            }
            return prettyJson;
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return "error";
    }

    public String list(String a){

        try {
            JSONObject json = new JSONObject(a);
            String ret = listado(json, 0);
            return ret;



        } catch (JSONException e) {
            e.printStackTrace();
        }
        return "error 1";

    }


    public String listado(JSONObject json, int nivel) throws JSONException{
    StringBuilder resultado = new StringBuilder();

    if (json.has("fldr")) {
        resultado.append("    ".repeat(nivel)).append(json.getString("fldr")).append("/\n");
    }
    if (json.has("cont")) {
        JSONArray cont = json.getJSONArray("cont");
        for (int i = 0; i < cont.length(); i++) {
            JSONObject item = cont.getJSONObject(i);
            if (item.has("file")) {
                resultado.append("  ".repeat(nivel + 1)).append(item.getString("file")).append("\n");
            } else {
                resultado.append(listado(item, nivel + 1));
            }
        }
    }

    return resultado.toString();
}
}
