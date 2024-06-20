import java.io.File;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

// Clase Cliente que implementa Runnable para ejecutar tareas en hilos
public class Cliente implements Runnable {
    private final int port = 8000;
    private final int nestingLevel;
    private Thread runningThread = null;
    private final ExecutorService threadPool = Executors.newFixedThreadPool(100);
    private final String urlString;
    private final String downloadPath;
    private final String currentPath;

    public Cliente(String urlString, String downloadPath, String currentPath, int nestingLevel) {
        this.urlString = urlString;
        this.downloadPath = downloadPath;
        this.currentPath = currentPath;
        this.nestingLevel = nestingLevel;
    }

    @Override
    public void run() {
        synchronized (this) {
            this.runningThread = Thread.currentThread();
        }
        this.threadPool.execute(new ThreadPoolHandler(threadPool, urlString, downloadPath, currentPath, nestingLevel));
    }

    public static void main(String[] args) {
        // Definición de variables iniciales para el programa
        String urlString = "http://148.204.58.221/axel/aplicaciones/sockets/java/udp/ejemplo/";
        String downloadPath = "downloads";
        int nestingLevel = 1;
        String currentPath = new File("").getAbsolutePath().replace('\\', '/');
        
        Cliente client = new Cliente(urlString, downloadPath, currentPath, nestingLevel);
        
        // Inicio de los hilos
        new Thread(client).start();
    }
}
