#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "List.h"

#include <dirent.h>
#include <jansson.h>


int main() {
    int puerto_num = 8080;

    int sockfd, newsockfd, portno, sockfd1, newsockfd1;
    socklen_t clilen;
    char buffer[256];
    char buffer1[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // Crear un socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

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
        error("ERROR on binding");

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
            error("ERROR on accept");

        // Leer datos del cliente
        memset(buffer, 0, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        int opcion = atoi(buffer);
        printf("Peticion del cliente: %d\n", opcion);

        // Determinar la respuesta basada en el mensaje recibido
        if(opcion == 0){
            char *contenido = directorioActual(ruta);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

        }else if(opcion == 1) {
            char *contenido = listarContenido(ruta);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

        }else if(opcion == 2) {
            n = write(newsockfd, "Adios", strlen("Adios"));

        }else if(opcion == 3){

        }else if(opcion == 4){
            // respuesta simple
            char *recib = recibido();
            int taman = strlen(recib);
            printf("tam: %d\n", taman);
            printf("contenido: %s\n", recib);
            n = write(newsockfd, recib, taman);

            




            /*
            
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
            if (newsockfd < 0)
                error("ERROR on accept");


            memset(buffer1, 0, 256);
            n = read(newsockfd, buffer1, 255);
            if (n < 0)
                error("ERROR reading from socket");
            printf("Mensaje recibido del cliente: %s\n", buffer1);

            int tam_nuevo =  strlen(ruta) + strlen(buffer1) + 1;
            ruta = (char *) realloc(ruta, tam_nuevo);
            strcat(ruta, "/");
            strcat(ruta, buffer1);

            printf("Ruta nuevecita: %s\n", ruta); 

            char *contenido = cambiarDirectorio(ruta);
            int tam = strlen(contenido);
            printf("tam: %d\n", tam);
            printf("contenido: %s\n", contenido);
            n = write(newsockfd, contenido, strlen(contenido));

            */

        }else {
            n = write(newsockfd, "Mensaje no reconocido", strlen("Mensaje no reconocido"));
        }
        if (n < 0)
            error("ERROR writing to socket");
        printf("Mensaje enviado al cliente\n\n");
        printf("===========================\n");

        // Cerrar el socket de comunicación con el cliente
        close(newsockfd);
    }

    // Cerrar el socket del servidor (esto no se ejecutará en este ejemplo)
    close(sockfd);

    return 0;
}
