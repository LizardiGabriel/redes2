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

    public String list(String a) {
        try {
            JSONObject json = new JSONObject(a);
            StringBuilder resultado = new StringBuilder();
            resultado.append(listado(json, 0, ""));
            return resultado.toString();
        } catch (JSONException e) {
            e.printStackTrace();
            return "error 1";
        }
    }

    public String listado(JSONObject json, int nivel, String ruta) throws JSONException {
        String space = "        ";
        StringBuilder resultado = new StringBuilder();
        String rutaActual = "";
        if (json.has("fldr")) {
            String carpetaActual = json.getString("fldr");
            rutaActual = carpetaActual + "/";
            resultado.append("│        ".repeat(nivel)).append("├─── ").append(rutaActual).append("\n");
        }
        if (json.has("cont")) {
            JSONArray cont = json.getJSONArray("cont");
            for (int i = 0; i < cont.length(); i++) {
                JSONObject item = cont.getJSONObject(i);
                if (item.has("fldr")) {
                    resultado.append(listado(item, nivel + 1, rutaActual));
                } else if (item.has("file")) {
                    resultado.append("│        ".repeat(nivel + 1)).append("└─── ").append(item.getString("file")).append("\n");
                }
            }
        }
        return resultado.toString();
    }




}
