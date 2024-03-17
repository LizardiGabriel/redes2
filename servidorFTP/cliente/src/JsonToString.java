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




}
