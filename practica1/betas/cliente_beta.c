#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int sd, rv;
    struct addrinfo hints, *res;

    const char *host = "localhost"; // Dirección del servidor
    const char *port = "8080";      // Puerto del servidor

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // Permitir IPv4 o IPv6
    hints.ai_socktype = SOCK_STREAM; // Tipo de socket TCP

    // Obtener información de dirección del servidor
    if ((rv = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Crear un socket
    if ((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        perror("client: socket");
        return 2;
    }

    // Conectar al servidor
    if (connect(sd, res->ai_addr, res->ai_addrlen) == -1) {
        close(sd);
        perror("client: connect");
        return 3;
    }

    // Leer datos del servidor
    char buf[100];
    int n;

    memset(buf, 0, sizeof(buf));
    n = read(sd, buf, sizeof(buf));
    if (n < 0)
        perror("Error en la función read()\n");
    else if (n == 0) {
        perror("Socket cerrado\n");
        exit(1);
    }
    printf("Mensaje del servidor: %s\n", buf);

    int v;
    n = read(sd, &v, sizeof(v));
    if (n < 0)
        perror("Error en la función read()\n");
    else if (n == 0) {
        perror("Socket cerrado\n");
        exit(1);
    }
    printf("Numerito servidor: %d\n", v);



    char *msj = "un mensaje soy cliente0";
    n = write(sd, msj, strlen(msj) + 1);
    if (n < 0)
        perror("Error en la función write\n");
    else if (n == 0) {
        perror("Socket cerrado\n");
        exit(1);
    }

    // Cerrar el socket
    close(sd);

    return 0;
}
