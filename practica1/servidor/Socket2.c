#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <jansson.h>
#include <string.h>

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "List.h"
#include "Socket3.h"

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



    // Aceptar múltiples conexiones y manejarlas en bucle
    while (1) {

        // Aceptar una conexión entrante y crear un nuevo socket para la comunicación
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            printf("ERROR on accept\n");

        // leer tam del mensaje
        memset(buffer, 0, 256);
        n = read(newsockfd, buffer, 255);
        long int tam = atoi(buffer);
        printf("recibi el tam: %ld\n", tam);

        memset(buffer, 0, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            printf("ERROR reading from socket\n");
        printf("Mensaje recibido del cliente: %s\n", buffer);

        char *mensaje = buffer;
        char *clave = opcJson("opcion", mensaje);

        if (strcmp(clave, "list1") == 0) {
            char *contenido = listarContenido(ruta);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

        } else if (strcmp(clave, "mkdir2") == 0) {
            char *carpetaNueva = opcJson("carpeta", mensaje);
            char *resMkdir = mkdirCarpeta(ruta, carpetaNueva);

            int tam = strlen(resMkdir);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", resMkdir);
            n = write(newsockfd, resMkdir, strlen(resMkdir));

        } else if (strcmp(clave, "rmdir3") == 0) {
            char *borrarAlgo = opcJson("archivo", mensaje);
            char *resRmdir = rmdirAlgo(ruta, borrarAlgo);

            int tam = strlen(resRmdir);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", resRmdir);
            n = write(newsockfd, resRmdir, strlen(resRmdir));

        } else if (strcmp(clave, "cd4") == 0) {
            char *ruta_nueva = opcJson("carpeta", mensaje);
            int tam_nuevo = strlen(ruta) + strlen(ruta_nueva) + 1;
            ruta = (char *)realloc(ruta, tam_nuevo);
            strcat(ruta, "/");
            strcat(ruta, ruta_nueva);
            printf("Ruta nuevecita: %s\n", ruta);

            char *contenido = cambiarDirectorio(ruta);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

        } else if (strcmp(clave, "put5") == 0) {
            char *nombreOriginal = opcJson("nombreOrig", mensaje);
            char *tama = opcJson("tam", mensaje);

            printf("tam de archivo: %s bytes\n", tama);

            char *contenido = recibido("recibi el mensaje del archivo zip");
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

            int pasarTam = atoi(tama);
            printf("tam para pasar: %d\n", pasarTam);

            close(newsockfd);

            int recibirAlgo = iniciarSocket3(8081, nombreOriginal, pasarTam, ruta);



        } else if (strcmp(clave, "get6") == 0) {
            char *nombreOriginal = opcJson("nombreOrig", mensaje);

            char *rutaAbs = rutaAbsoluta(ruta, nombreOriginal);

            int existeRuta = existe(rutaAbs);
            int tam_archivo = 0;
            if (existeRuta) {
                tam_archivo = calcularTamanito(rutaAbs);
            }
            char env_tam[20];
            sprintf(env_tam, "%d", tam_archivo);
            char *contenido = recibido(env_tam);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

        } else if (strcmp(clave, "list0") == 0) {
            char *contenido = directorioActual(ruta);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));
        } else {
            printf("---------->Comando no reconocido :c <----------\n");
            n = write(newsockfd, "Comando no reconocido", 21);
        }

        if (n < 0)
            printf("ERROR writing to socket\n");
        printf("Mensaje enviado al cliente\n\n");
        printf("===========================\n");

        // Cerrar el socket de comunicación con el cliente

 
            close(newsockfd);
        
    }

    close(sockfd);
    return 0;
}
