import java.io.IOException;
import java.util.Scanner;

import static java.lang.System.exit;

public class Menu {

    public int Menu() throws IOException {


        Scanner scanner = new Scanner(System.in);
        int choice = 0;
        int choiceList = 0;

        Local local = new Local();
        Remoto remoto = new Remoto();
        String localPath = System.getProperty("user.dir");

        do {
            choice = 0;
            choiceList = 0;
            remoto.getDirectorio();
            System.out.println("path local: " + localPath);

            System.out.println("=========== Menu ===========");
            System.out.println("1. Listar el contenido del folder local o remotamente (list)");
            System.out.println("2. Crear carpetas Local o remotamente (mkdir)");
            System.out.println("3. Eliminar carpetas/archivos local o remotamente (rmdir)");
            System.out.println("4. Cambiar la ruta de directorio local o remoto (cd)");
            System.out.println("5. Enviar archivos o carpetas al servidor (put)");
            System.out.println("6. Recibir archivos o carpetas desde el servidor(get)");
            System.out.println("7. Salir de la aplicacion (quit)");
            System.out.println("=============================");
            System.out.print("Por favor, selecciona una opcion: ");
            try {
                choice = scanner.nextInt();
            } catch (Exception e) {
                System.out.println("Opcion no valida!!!");
                exit(1);
            }

            if((choice != 7) && (choice != 5) && (choice != 6)){
                System.out.println("1. Local");
                System.out.println("2. Remoto");
                System.out.print("Por favor, selecciona una opcion: ");
                try {
                    choiceList = scanner.nextInt();
                } catch (Exception e) {
                    System.out.println("Opcion no valida!!!");
                    exit(1);

                }

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
                        remoto.eliminarAlgo();

                    }
                    break;
                case 4:
                    System.out.print("(cd): ");
                    // Logica para cambiar la ruta de directorio
                    if (choiceList == 1) {
                        System.out.println("local");
                        localPath = local.cambiarDirectorio(localPath);

                    } else if (choiceList == 2) {
                        System.out.println("remotamente");
                        remoto.cambiarDirectorio();

                    }
                    break;
                case 5:
                    System.out.println("(put): ");
                    // Logica para enviar archivos o carpetas
                    remoto.enviarArchivo(localPath);
                    System.out.println("Archivo enviado exitosamente!!!!!");
                    exit(5);



                case 6:
                    System.out.println("(get): ");
                    // Logica para recibir archivos o carpetas
                    remoto.recibirArchivo(localPath);
                    System.out.println("Archivo recibido exitosamente!!!!!");
                    exit(6);

                case 7:
                    System.out.println("Saliendo de la aplicacion...");



                    break;
                default:
                    System.out.println("Opcion no valida. Por favor, selecciona una opcion del menu.");
                    exit(0);
                    break;
            }

        } while (choice != 7);

        scanner.close();

        return 0;

    }
}
