import java.io.IOException;
import java.util.Scanner;

public class Menu {

    public void Menu() throws IOException {


        Scanner scanner = new Scanner(System.in);
        int choice;
        int choiceList = 0;

        Local local = new Local();
        Remoto remoto = new Remoto();

        do {
            remoto.getDirectorio();

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

            if(choice != 7 && choiceList != 5 && choiceList != 6){
                System.out.println("1. Local");
                System.out.println("2. Remoto");
                System.out.print("Por favor, selecciona una opcion: ");
                choiceList = scanner.nextInt();
            }

            switch (choice) {
                case 1:
                    System.out.print("(List): ");

                    if (choiceList == 1) {
                        System.out.println("local");
                        local.listarContenido();

                    } else if (choiceList == 2) {
                        System.out.println("remotamente");
                        remoto.listarContenido();

                    }
                    break;
                case 2:
                    System.out.print("(mkdir): ");

                    if (choiceList == 1) {
                        System.out.println("local");
                        local.crearCarpeta();

                    } else if (choiceList == 2) {
                        System.out.println("remotamente");
                        remoto.crearCarpeta();

                    } else {
                        break;
                    }
                    break;
                case 3:
                    System.out.print("(rmdir): ");
                    // Logica para eliminar carpetas/archivos
                    if (choiceList == 1) {
                        System.out.println("local");
                        local.eliminarCarpeta();

                    } else if (choiceList == 2) {
                        System.out.println("remotamente");
                        remoto.eliminarCarpeta();

                    }
                    break;
                case 4:
                    System.out.print("(cd): ");
                    // Logica para cambiar la ruta de directorio
                    if (choiceList == 1) {
                        System.out.println("local");
                        local.cambiarDirectorio();

                    } else if (choiceList == 2) {
                        System.out.println("remotamente");
                        remoto.cambiarDirectorio();

                    }
                    break;
                case 5:
                    System.out.println("(put): ");
                    // Logica para enviar archivos o carpetas
                    remoto.enviarArchivo();
                    break;
                case 6:
                    System.out.println("(get): ");
                    // Logica para recibir archivos o carpetas
                    remoto.recibirArchivo();
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
