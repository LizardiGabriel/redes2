#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>

#include "List.h"

int iniciarSocket3(int puerto, char *nombreArchivo, int tam, char *path){

    printf("Iniciando socket 3\n");

    int sockfd, newsockfd, portno, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char *buffer = (char *)malloc((tam +1)*sizeof(char));

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

    // Abrir un nuevo archivo para escribir los datos recibidos
    int tamPath = strlen(path) + 1 + strlen(nombreArchivo) + 1 + 4;
    char *pathArchivo = (char *)malloc(tamPath * sizeof(char));
    strcpy(pathArchivo, path);
    strcat(pathArchivo, "/");
    strcat(pathArchivo, nombreArchivo);
    strcat(pathArchivo, ".zip");

    printf("pathArchivo: %s\n", pathArchivo);

    FILE *file = fopen(pathArchivo, "wb");
    if (file == NULL) {
        perror("ERROR opening file");
        exit(1);
    }

    // Recibir los datos del archivo en trozos y escribirlos en el archivo en el servidor
    printf("Recibiendo archivo...\n");
    int totalBytesRead = 0;
    while (totalBytesRead < tam) {
        n = read(newsockfd, buffer, 1024);
        if (n <= 0) {
            break;
        }
        fwrite(buffer, 1, n, file);
        totalBytesRead += n;
    }
    
    fclose(file);

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Archivo recibido\n");

    char *contenido = recibido("recibi el mensaje del archivo zip que me mandaste en el json");
    int taman = strlen(contenido);
    printf("tam: %d\n", taman);
    printf("contenido: %s\n", contenido);
    n = write(newsockfd, contenido, strlen(contenido));

    // Cerrar el archivo y el socket
    fclose(file);
    close(newsockfd);
    close(sockfd);

    return 0;
}