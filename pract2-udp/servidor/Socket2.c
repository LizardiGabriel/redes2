#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

#include <string.h>

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "List.h"
#include "Socket3.h"
#include "Socket4.h"



int iniciarSocket(int puerto) {

    int puerto_num = puerto;
    char buffer[256];
    int sockfd, newsockfd, portno, sockfd1, newsockfd1;
    socklen_t clilen;

    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // Crear un socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        printf("ERROR opening socket\n");

    // Permitir la reutilización del puerto
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    // Limpiar la estructura de dirección del servidor y configurar el puerto
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    portno = puerto_num;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Enlazar el socket a la dirección y puerto especificados
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        printf("ERROR on binding\n");

    // Escuchar conexiones entrantes
    listen(sockfd, 5);

    // ruta directorio
    char *ruta = directorio();
    printf("Ruta inicial: %s\n", ruta);

    int solicitud = 0;

    int cerrarYN = 0;
    int cerradito = 1;

    // Aceptar múltiples conexiones y manejarlas en bucle
    while (1) {
        printf("===========================\n");
        printf("soliciutd: %d\n", solicitud);

        // Aceptar una conexión entrante y crear un nuevo socket para la comunicación
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            printf("ERROR on accept\n");

        memset(buffer, 0, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            printf("ERROR reading from socket\n");
        // printf("Mensaje recibido del cliente: %s\n", buffer);

        char *mensaje = buffer;
        char *clave = opcJson("opcion", mensaje);
        printf("Clave: %s\n", clave);

        if (strcmp(clave, "list1") == 0) {
            char *contenido = generarList1(ruta);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

        } else if (strcmp(clave, "mkdir2") == 0) {
            char *carpetaNueva = opcJson("carpeta", mensaje);

            char *resMkdir = mkdirCarpeta(ruta, carpetaNueva);
            printf("contenido: %s\n", resMkdir);
            n = write(newsockfd, resMkdir, strlen(resMkdir));

        } else if (strcmp(clave, "rmdir3") == 0) {
            char *borrarAlgo = opcJson("archivo", mensaje);

            char *resRmdir = rmdirAlgo(ruta, borrarAlgo);
            printf("contenido: %s\n", resRmdir);
            n = write(newsockfd, resRmdir, strlen(resRmdir));

        } else if (strcmp(clave, "cd4") == 0) {
            char *ruta_nueva = opcJson("carpeta", mensaje);

            if (strcmp(ruta_nueva, "..") == 0) {
                // cambiar a directorio anterior
                printf("--->Ruta anterior\n");
                char *dirPadre = cambiarDirectorioAnterior(ruta);
                printf("dirPadre: %s\n", dirPadre);
                int tam_nuevo = strlen(dirPadre) + 1;
                ruta = (char *)realloc(ruta, tam_nuevo);
                strcpy(ruta, dirPadre);
                printf("Ruta nuevecita: %s\n", ruta);

                char *contenido = cambiarDirectorio(dirPadre);
                printf("contenido: %s\n", contenido);
                n = write(newsockfd, contenido, strlen(contenido));

            
            } else if(strcmp(ruta_nueva, ".") == 0){
                printf("--->Ruta actual\n");
                char *contenido = cambiarDirectorio(ruta);
                printf("contenido: %s\n", contenido);
                n = write(newsockfd, contenido, strlen(contenido));

            }else{
                char *sepRuta = "/";
                int tam_nuevo = strlen(ruta) + strlen(ruta_nueva) + 1;
                ruta = (char *)realloc(ruta, tam_nuevo);
                strcat(ruta, sepRuta);
                strcat(ruta, ruta_nueva);
                printf("Ruta nuevecita: %s\n", ruta);

                char *contenido = cambiarDirectorio(ruta);
                printf("contenido: %s\n", contenido);
                n = write(newsockfd, contenido, strlen(contenido));
            }

        } else if (strcmp(clave, "put5") == 0) {
            char *typeFile = opcJson("type", mensaje);
            if (strcmp(typeFile, "file") == 0) {
                printf("Es un archivo\n");
            }
            if (strcmp(typeFile, "fldr") == 0) {
                printf("Es un folder\n");
            }

            char *nombreOriginal = opcJson("nombreOrig", mensaje);
            char *tama = opcJson("tam", mensaje);
            printf("tam de archivo: %s bytes\n", tama);

            char *contenido = recibido("recibi el msj del archivo");
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

            int pasarTam = atoi(tama);
            // printf("tam para pasar: %d\n", pasarTam);

            char *tamBuffer = opcJson("tamBuffer", mensaje);
            int tamBufferInt = atoi(tamBuffer);

            char *tamVentana = opcJson("tamVentana", mensaje);
            int tamVentanaInt = atoi(tamVentana);

            printf("\t\t===>tamBufferInt: %d\n", tamBufferInt);
            printf("\t\t===>numPaquetesInt: %d\n", tamVentanaInt);

            cerradito = close(newsockfd);
            if (cerradito == 0)
                cerrarYN = 1;

            // pasar el parametro typeFile para saber si es un archivo o carpeta
            // cambiar para usar tco / udp

            int recibirAlgo = iniciarSocket3(1235, nombreOriginal, pasarTam, ruta, typeFile, tamBufferInt, tamVentanaInt);
            //int recibirAlgo = iniciarSocket3UDP(1235, nombreOriginal, pasarTam, ruta, typeFile, tamBufferInt, tamVentanaInt);

            if (recibirAlgo == 0) {
                printf("Archivo recibido c:\n");
            }

        } else if (strcmp(clave, "get6") == 0) {
            char *nombreOriginal = opcJson("nombreOrig", mensaje);
            char *rutaAbs = rutaAbsoluta(ruta, nombreOriginal);

            int existeRuta = existe(rutaAbs);

            // ver si la ruta es de un archivo o de un folder
            struct stat path_stat;
            stat(rutaAbs, &path_stat);
            int is_file = S_ISREG(path_stat.st_mode);

            int isFolder = 0;

            if (is_file) {
                printf("rutaAbs is a file\n");
            } else {
                isFolder = 1;
                printf("rutaAbs is a directory\n");
                // hacer zip de la carpeta
                char zip_command[256];
                sprintf(zip_command, "zip -jr %s.zip %s", rutaAbs, rutaAbs);
                system(zip_command);
                printf("comando: %s\n", zip_command);
                printf("Folder zipped successfully.\n");

                int lenRuta = strlen(rutaAbs) + 4;
                rutaAbs = (char *)realloc(rutaAbs, lenRuta);
                strcat(rutaAbs, ".zip");
            }

            int tam_archivo = 0;
            if (existeRuta) {
                tam_archivo = calcularTamanito(rutaAbs);
            }
            char env_tam[20];
            sprintf(env_tam, "%d", tam_archivo);

            char *contenido = mandarFileFldr(env_tam, isFolder);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

            cerradito = close(newsockfd);
            if (cerradito == 0)
                cerrarYN = 1;

            int envAlgo = iniciarSocket4(1236, rutaAbs, tam_archivo);
            if (envAlgo == 0) {
                printf("Archivo enviado c:\n");
            }

            if (isFolder == 1) {
                int tamCmnd2 = strlen(rutaAbs) + 4;
                char *comando2 = (char *)malloc(tamCmnd2 * sizeof(char));
                strcpy(comando2, "rm ");
                strcat(comando2, rutaAbs);
                system(comando2);
                free(comando2);
            }

        } else if (strcmp(clave, "list0") == 0) {
            char *contenido = directorioActual(ruta);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

        } else {
            printf("---------->Comando no reconocido :c <----------\n");
            n = write(newsockfd, "Comando no reconocido", 21);
        }

        if (n < 0)
            printf("ERROR writing to socket\n");
        printf("Mensaje enviado al cliente\n\n");

        // Cerrar el socket de comunicación con el cliente

        if (cerrarYN == 0)
            close(newsockfd);
        cerradito = 1;
        cerrarYN = 0;

        solicitud++;
    }

    close(sockfd);
    return 0;
}
