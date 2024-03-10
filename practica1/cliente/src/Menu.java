import java.io.IOException;
import java.util.Scanner;

public class Menu {

    public void Menu() throws IOException {


        Scanner scanner = new Scanner(System.in);
        int choice;

        Local local = new Local();
        Remoto remoto = new Remoto();

        do {
            System.out.println("=========== Menu ===========");
            System.out.println("1. Listar el contenido de las carpetas almacenadas local o remotamente (list)");
            System.out.println("2. Crear carpetas Local o remotamente (mkdir)");
            System.out.println("3. Eliminar carpetas/archivos local o remotamente (rmdir)");
            System.out.println("4. Cambiar la ruta de directorio local o remoto (cd)");
            System.out.println("5. Enviar archivos o carpetas desde una carpeta local (cliente) hacia una carpeta remota (servidor) (put)");
            System.out.println("6. Enviar archivos o carpetas desde una carpeta remota -servidor- hacia una carpeta local -cliente- (get)");
            System.out.println("7. Salir de la aplicacion (quit)");
            System.out.println("=============================");
            System.out.print("Por favor, selecciona una opcion: ");

            choice = scanner.nextInt();

            switch (choice) {
                case 1:
                    System.out.println("Listar el contenido de las carpetas almacenadas");
                    System.out.println("1. Local");
                    System.out.println("2. Remoto");
                    System.out.print("Por favor, selecciona una opcion: ");
                    int choiceList = scanner.nextInt();
                    if (choiceList == 1) {
                        // listar contenido local
                        local.listarContenido();

                    } else if (choiceList == 2) {
                        // listar contenido remoto
                        remoto.listarContenido();

                    } else {
                        break;
                    }
                    break;
                case 2:
                    System.out.println("Crear carpetas Local o remotamente (mkdir)");
                    // Logica para crear carpetas
                    System.out.println("Crear carpeta");
                    System.out.println("1. Local");
                    System.out.println("2. Remoto");
                    System.out.print("Por favor, selecciona una opcion: ");
                    int choiceMkdir = scanner.nextInt();
                    if (choiceMkdir == 1) {
                        // crear carpeta local
                        local.crearCarpeta();

                    } else if (choiceMkdir == 2) {
                        // crear carpeta remota
                        remoto.crearCarpeta();

                    } else {
                        break;
                    }
                    break;
                case 3:
                    System.out.println("Eliminar carpetas/archivos local o remotamente (rmdir)");
                    // Logica para eliminar carpetas/archivos
                    break;
                case 4:
                    System.out.println("Cambiar la ruta de directorio local o remoto (cd)");
                    // Logica para cambiar la ruta de directorio
                    break;
                case 5:
                    System.out.println("Enviar archivos o carpetas desde una carpeta local (cliente) hacia una carpeta remota (servidor) (put)");
                    // Logica para enviar archivos o carpetas
                    break;
                case 6:
                    System.out.println("Enviar archivos o carpetas desde una carpeta remota -servidor- hacia una carpeta local -cliente- (get)");
                    // Logica para recibir archivos o carpetas
                    break;
                case 7:
                    System.out.println("Saliendo de la aplicacion...");


                    break;
                default:
                    System.out.println("Opcion no valida. Por favor, selecciona una opcion del menu.");
                    break;
            }

        } while (choice != 7);

        scanner.close();

    }
}
