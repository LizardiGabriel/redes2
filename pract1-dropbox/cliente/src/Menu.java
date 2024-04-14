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
            System.out.println("1. (list) Listar el contenido del folder local o remotamente");
            System.out.println("2. (mkdir) Crear carpetas Local o remotamente");
            System.out.println("3. (rmdir) Eliminar carpetas/archivos local o remotamente");
            System.out.println("4. (cd) Cambiar la ruta de directorio local o remoto");
            System.out.println("5. (put) Enviar archivos o carpetas al servidor");
            System.out.println("6. (get) Recibir archivos o carpetas desde el servidor");
            System.out.println("7. (quit) Salir de la aplicacion");
            System.out.println("8. (clear)");
            System.out.println("=============================");
            System.out.print("Por favor, selecciona una opcion: ");
            try {
                choice = scanner.nextInt();
            } catch (Exception e) {
                System.out.println("Opcion no valida!!!");
                exit(1);
            }

            if((choice != 7) && (choice != 5) && (choice != 6) && (choice != 8)){
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
                        local.listarContenido(localPath);

                    } else if (choiceList == 2) {
                        System.out.println("remotamente");
                        remoto.listarContenido();

                    }
                    break;
                case 2:
                    System.out.print("(mkdir): ");

                    if (choiceList == 1) {
                        System.out.println("local");
                        local.crearCarpeta(localPath);

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
                        local.eliminarCarpeta(localPath);

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
                    break;



                case 6:
                    System.out.println("(get): ");
                    // Logica para recibir archivos o carpetas
                    remoto.recibirArchivo(localPath);
                    System.out.println("Archivo recibido exitosamente!!!!!");
                    break;

                case 7:
                    System.out.println("Saliendo de la aplicacion...");
                    break;
                case 8:
                    final String os = System.getProperty("os.name");
                    System.out.println("sistem: " + os);
                    if(os.contains("Windows")){
                        Runtime.getRuntime().exec("cls");
                    }else{
                        Runtime.getRuntime().exec("clear");
                    }
                    
                    break;
                default:
                    System.out.println("Opcion no valida. Por favor, selecciona una opcion del menu.");
                    break;
            }

        } while (choice != 7);

        scanner.close();

        return 0;

    }
}
