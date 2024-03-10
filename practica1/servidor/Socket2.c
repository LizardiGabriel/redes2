#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <jansson.h>
#include <string.h>

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "List.h"

int iniciarSocket(int puerto) {

    int puerto_num = puerto;

    int sockfd, newsockfd, portno, sockfd1, newsockfd1;
    socklen_t clilen;
    char buffer[256];
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

        // Leer datos del cliente
        memset(buffer, 0, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            printf("ERROR reading from socket\n");

        char *mensaje = buffer;
        printf("Mensaje recibido del cliente: %s\n", mensaje);

        if (strcmp(mensaje, "list1")) {
            printf("soy list 1 ================================================================");
            char *contenido = listarContenido(ruta);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

        } else if (strcmp(mensaje, "mkdir2")) {

        } else if (strcmp(mensaje, "rmdir3")) {

        } else if (strcmp(mensaje, "cd4")) {
            char *recib = recibido();
            int taman = strlen(recib);
            printf("tam: %d\n", taman);
            printf("contenido: %s\n", recib);

        } else if (strcmp(mensaje, "put5")) {

        } else if (strcmp(mensaje, "get6")) {

        } else if (strcmp(mensaje, "list0")) {
            printf("soy list 0 ================================================================");
            char *contenido = directorioActual(ruta);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));
        }else{
            printf("---------->Comando no reconocido :c <----------\n");
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
