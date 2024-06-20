import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

// Clase ThreadPoolHandler que implementa Runnable para manejar la descarga de archivos y procesamiento de URLs
public class ThreadPoolHandler implements Runnable {
    private ArrayList<URL> urlList = new ArrayList<>();
    private URL url;
    private ExecutorService threadPool;
    private String downloadPath;
    private String currentLocation;
    private String currentPath;
    private int nestingLevel;

    public ThreadPoolHandler(ExecutorService threadPool, String urlString, String downloadPath, String currentPath, int nestingLevel) {
        this.threadPool = threadPool;
        this.downloadPath = downloadPath;
        this.currentPath = currentPath;
        this.nestingLevel = nestingLevel;
        try {
            this.url = new URL(urlString);
        } catch (MalformedURLException e) {
            throw new IllegalArgumentException("Invalid URL: " + urlString, e);
        }
    }

    @Override
    public void run() {
        try {
            // Ruta del archivo a descargar
            String filePath = (downloadPath + "/" + getResourcePath(url)).replaceAll("%20", " ");
            File file = new File(filePath);

            if (!file.exists()) {
                // Crear la ruta del archivo a descargar si no existe
                createFilePath(url);
                file.createNewFile();

                // Conexión al servidor para descargar el archivo
                URLConnection connection = url.openConnection();
                connection.connect();

                // Flujo de entrada y salida para la creación del archivo
                try (InputStream inputStream = connection.getInputStream();
                     OutputStream outputStream = new FileOutputStream(file)) {

                    int byteRead;
                    while ((byteRead = inputStream.read()) != -1) {
                        outputStream.write(byteRead);
                    }
                }

                // Procesar el contenido del archivo si es un HTML o PHP
                if (filePath.endsWith(".html") || filePath.endsWith(".php")) {
                    urlList.addAll(extractUrls(filePath, url.getHost(), currentLocation));
                    if (nestingLevel > 0) {
                        for (URL u : urlList) {
                            this.threadPool.execute(new ThreadPoolHandler(threadPool, u.toString(), downloadPath, currentPath, nestingLevel - 1));
                        }
                    }
                    updateLinks(filePath, url.getHost(), currentLocation);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Obtener la ruta del recurso a partir de la URL
    private String getResourcePath(URL url) {
        String path = url.getPath();
        if (path.isEmpty()) {
            return "index.html";
        } else if (path.endsWith("/")) {
            return path + "index.html";
        } else if (!path.contains(".")) {
            return path + ".html";
        } else {
            return path.startsWith("/") ? path.substring(1) : path;
        }
    }

    // Crear la ruta del archivo
    private void createFilePath(URL url) {
        String path = this.downloadPath + '/' + getRelativePath(url);
        File dir = new File(path);
        this.currentLocation = path.substring(this.downloadPath.length());
        if (!dir.exists()) {
            dir.mkdirs();
        }
    }

    // Extraer URLs de un archivo HTML/PHP
    private ArrayList<URL> extractUrls(String fileName, String domain, String location) {
        ArrayList<URL> urlList = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(new FileReader(fileName))) {
            String line;
            while ((line = reader.readLine()) != null) {
                extractUrlsFromLine(line, domain, location, urlList);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return urlList;
    }

    // Extraer URLs de una línea de texto
    private void extractUrlsFromLine(String line, String domain, String location, ArrayList<URL> urlList) {
        for (int i = 0; i < line.length(); i++) {
            if (i + 7 < line.length() && (line.startsWith("href", i) || line.startsWith("src", i))) {
                i = line.indexOf('=', i) + 1;
                while (line.charAt(i) == ' ') i++;
                char quote = line.charAt(i++);
                int start = i;
                while (line.charAt(i) != quote) i++;
                String urlString = line.substring(start, i);
                addUrlToList(urlString, domain, location, urlList);
            }
        }
    }

    // Añadir una URL a la lista
    private void addUrlToList(String urlString, String domain, String location, ArrayList<URL> urlList) {
        try {
            if (urlString.startsWith("http")) {
                if (!compareDomains(domain, urlString)) {
                    urlList.add(new URL(urlString));
                }
            } else if (!urlString.isEmpty()) {
                urlString = urlString.charAt(0) == '/' ? "http://" + domain + urlString : "http://" + domain + location + urlString;
                urlList.add(new URL(urlString));
            }
        } catch (MalformedURLException e) {
            e.printStackTrace();
        }
    }

    // Actualizar los enlaces en el archivo descargado
    private void updateLinks(String fileName, String domain, String location) {
        File tempFile = new File("temp.txt");
        try (BufferedReader reader = new BufferedReader(new FileReader(fileName));
             BufferedWriter writer = new BufferedWriter(new FileWriter(tempFile))) {

            String line;
            while ((line = reader.readLine()) != null) {
                writer.write(updateLineWithLocalPaths(line, domain, location) + "\n");
            }

            File originalFile = new File(fileName);
            if (originalFile.delete()) {
                tempFile.renameTo(originalFile);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Actualizar una línea con rutas locales
    private String updateLineWithLocalPaths(String line, String domain, String location) {
        StringBuilder updatedLine = new StringBuilder(line);
        for (int i = 0; i < line.length(); i++) {
            if (i + 7 < line.length() && (line.startsWith("href", i) || line.startsWith("src", i))) {
                i = line.indexOf('=', i) + 1;
                while (line.charAt(i) == ' ') i++;
                char quote = line.charAt(i++);
                int start = i;
                while (line.charAt(i) != quote) i++;
                String urlString = line.substring(start, i);
                String newUrlString = getLocalPath(urlString, domain, location);
                updatedLine.replace(start, i, newUrlString);
            }
        }
        return updatedLine.toString();
    }

    // Obtener ruta relativa a partir de la URL
    private String getRelativePath(URL url) {
        String path = url.getPath();
        int lastSlashIndex = path.lastIndexOf('/');
        return lastSlashIndex > 0 ? path.substring(1, lastSlashIndex + 1) : "";
    }

    // Obtener ruta local para actualizar enlaces
    private String getLocalPath(String urlString, String domain, String location) {
        try {
            URL url = urlString.startsWith("http") ? new URL(urlString) : new URL("http://" + domain + (urlString.startsWith("/") ? urlString : location + urlString));
            return currentPath + "/" + downloadPath + "/" + getResourcePath(url);
        } catch (MalformedURLException e) {
            e.printStackTrace();
            return urlString;
        }
    }

    // Comparar dominios para evitar enlaces externos
    private boolean compareDomains(String domain1, String urlString) {
        try {
            String domain2 = new URL(urlString).getHost().replaceFirst("www.", "");
            return domain1.replaceFirst("www.", "").equals(domain2);
        } catch (MalformedURLException e) {
            return false;
        }
    }
}
