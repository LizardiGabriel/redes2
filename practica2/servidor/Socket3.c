#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include <unistd.h>

#include "List.h"

#define BUFFER_SIZE 1024

int iniciarSocket3(int puerto, char *nombreArchivo, int tam, char *path, char *typeFile) {

    int sockfd, newsockfd, portno, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[BUFFER_SIZE];
    socklen_t len = sizeof(cli_addr);


    // Crear un socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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

    // Abrir un nuevo archivo para escribir los datos recibidos
    int tamPath = strlen(path) + 1 + strlen(nombreArchivo) + 1 + 4;
    char *pathArchivo = (char *)malloc(tamPath * sizeof(char));
    strcpy(pathArchivo, path);
    strcat(pathArchivo, "/");
    strcat(pathArchivo, nombreArchivo);
    strcat(pathArchivo, ".zip");

    char *pathArchivoUnzip = (char *)malloc(tamPath * sizeof(char));
    strcpy(pathArchivoUnzip, path);
    strcat(pathArchivoUnzip, "/");
    strcat(pathArchivoUnzip, nombreArchivo);

    FILE *file = fopen(pathArchivo, "wb");
    if (file == NULL) {
        perror("ERROR opening file");
        exit(1);
    }

    printf("Recibiendo archivo...\n");

    int totalBytesReceived = 0;
    while (1) {
        // Recibir paquete de datos del cliente
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cli_addr, &len);
        if (n == -1) {
            perror("Error al recibir datos");
            exit(EXIT_FAILURE);
        }
        totalBytesReceived += n;

        // Escribir datos en el archivo
        fwrite(buffer, 1, n, file);

        // Enviar respuesta al cliente (ACK)
        sendto(sockfd, "ACK", 3, 0, (struct sockaddr *)&cli_addr, len);

        // Si se recibe un paquete vacío, se considera el fin de la transmisión
        if (n == 0) {
            break;
        }
    }

    printf("Archivo recibido correctamente. Tamaño total: %d bytes\n", totalBytesReceived);

    fclose(file);

    // archivo recibido, descomprimir si typeFile = fldr || typeFile = fldr

    if (strcmp(typeFile, "fldr") == 0) {

        char *comando = (char *)malloc((tamPath * 2) * sizeof(char));
        strcpy(comando, "unzip -o ");
        strcat(comando, pathArchivo);
        strcat(comando, " -d ");
        strcat(comando, pathArchivoUnzip);

        system(comando);
        free(comando);

        int tamCmnd2 = strlen(pathArchivo) + 4;
        char *comando2 = (char *)malloc(tamCmnd2 * sizeof(char));
        strcpy(comando2, "rm ");
        strcat(comando2, pathArchivo);
        system(comando2);
        free(comando2);

    } else if (strcmp(typeFile, "file") == 0) {
        char *comando = (char *)malloc((tamPath * 2) * sizeof(char));
        strcpy(comando, "unzip -o ");
        strcat(comando, pathArchivo);
        strcat(comando, " -d ");
        strcat(comando, path);

        system(comando);
        free(comando);

        int tamCmnd2 = strlen(pathArchivo) + 4;
        char *comando2 = (char *)malloc(tamCmnd2 * sizeof(char));
        strcpy(comando2, "rm ");
        strcat(comando2, pathArchivo);
        system(comando2);
        free(comando2);
    }

    char *contenido = recibido("recibi el mensaje del archivo zip que me mandaste en el json");
    printf("contenido: %s\n", contenido);
    n = write(newsockfd, contenido, strlen(contenido));

    // Cerrar el socket

    close(newsockfd);
    close(sockfd);

    return 0;
}