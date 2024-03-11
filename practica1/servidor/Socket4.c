

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>

#include "List.h"

int iniciarSocket4(int puerto, char *rutaAbsoluta, int tam){

    printf("Iniciando socket 4 - enviar al cliente\n");

    int sockfd, newsockfd, portno, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // Crear un socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    // Permitir reutilizar el puerto inmediatamente después de cerrar el servidor
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    // Limpiar la estructura de dirección del servidor y configurar el puerto
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    portno = puerto; // Puerto pasado como argumento al programa
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Enlazar el socket a la dirección y puerto especificados
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // Escuchar conexiones entrantes
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Aceptar una conexión entrante y crear un nuevo socket para la comunicación
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    printf("newsockfd: %d\n", newsockfd);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    FILE *file = fopen(rutaAbsoluta, "rb");
    if (file == NULL) {
        perror("ERROR opening file");
        exit(1);
    }

    // enviar el archivo

    fseek(file, 0, SEEK_END); 
    long tamArchivo = ftell(file); 
    rewind(file); 

    char buffer[1024];
    size_t bytesLeidos;

    printf("enviando...\n");

    // Envía el contenido del archivo al cliente en bloques
    while ((bytesLeidos = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(newsockfd, buffer, bytesLeidos, 0) == -1) {
            perror("Error al enviar datos del archivo");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    close(newsockfd);
    close(sockfd);

    return 0;


}