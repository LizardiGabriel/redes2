import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class JsonToString {
    public void leer(String msj){

        try {
            JSONObject jsonObject = new JSONObject(msj);
            //String prettyJson = formatJson(jsonObject.toString(4));
            String prettyJson = (jsonObject.toString(4));

            System.out.println(prettyJson);

        } catch (JSONException e) {
            e.printStackTrace();
        }


    }

    private static String formatJson(String jsonString) {
        String json = "";
        int numChars = jsonString.length();


        for (int i = 0; i < numChars; i++) {
            char charAt = jsonString.charAt(i);
            char charAt1 = jsonString.charAt(i);
            if(i < numChars-2){
                charAt1 = jsonString.charAt(i+1);
            }

            if (charAt == '{' || charAt == '[') {
                if(charAt1 == '\n'){
                }

            }else

            if (charAt == '}' || charAt == ']') {
                if(charAt1 == '\n'){
                }
            }else
            if (charAt == ',') {
                if(charAt1 == '\n'){
                }
            }
             else {
                json += charAt;
            }
        }

        return json;



    }

    public void list(String a){

        try {
            JSONObject json = new JSONObject(a);
            listado(json, 0);
        } catch (JSONException e) {
            e.printStackTrace();
        }

    }


    public void listado(JSONObject json, int nivel) throws JSONException{

        if (json.has("fldr")) {
            System.out.println("  ".repeat(nivel) + json.getString("fldr") + "/");
        }
        if (json.has("cont")) {
            JSONArray cont = json.getJSONArray("cont");
            for (int i = 0; i < cont.length(); i++) {
                JSONObject item = cont.getJSONObject(i);
                if (item.has("file")) {
                    System.out.println("  ".repeat(nivel + 1) + item.getString("file"));
                } else {
                    listado(item, nivel + 1);
                }
            }
        }

    }
}
