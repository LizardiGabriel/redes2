#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <jansson.h>

int main() {
    int sd, cd, rv;
    struct addrinfo i, *r, *p;
    const char *pto = "8080"; // Puerto que deseas utilizar
    memset(&i, 0, sizeof(i)); // indicio
    i.ai_family = AF_INET6;   /* Permite IPv4 or IPv6 */
    i.ai_socktype = SOCK_STREAM;
    i.ai_flags = AI_PASSIVE; // utilizado para hacer el bind
    i.ai_protocol = 0;       /* Any protocol */
    i.ai_canonname = NULL;
    i.ai_addr = NULL;
    i.ai_next = NULL;

    if ((rv = getaddrinfo(NULL, pto, &i, &r)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = r; p != NULL; p = p->ai_next) {
        if ((sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (bind(sd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sd);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(r);

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    if (listen(sd, 128) == -1) {
        perror("error en func. Listen()\n");
        close(sd);
        exit(1);
    }

    for (;;) {
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        struct sockaddr_storage cdir;
        socklen_t ctam = sizeof(cdir);
        cd = accept(sd, (struct sockaddr *)&cdir, &ctam);
        if (cd == -1) {
            perror("accept");
            continue;
        }
        if (getnameinfo((struct sockaddr *)&cdir, sizeof(cdir), hbuf,
                        sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0)
            printf("cliente conectado desde %s:%s\n", hbuf, sbuf);

        int n;
        char buf[100];

        
        char *msj = "un mensaje soy server";
        n = write(cd, msj, strlen(msj) + 1);
        if (n < 0)
            perror("Error en la función write\n");
        else if (n == 0) {
            perror("Socket cerrado\n");
            exit(1);
        }

        int v = 2;
        n = write(cd, &v, sizeof(v));
        if (n < 0)
            perror("Error en la función write\n");
        else if (n == 0) {
            perror("Socket cerrado\n");
            exit(1);
        }
        
        
        memset(buf, 0, sizeof(buf));
        n = read(cd, buf, sizeof(buf));
        if (n < 0)
            perror("Error en la función read()\n");
        else if (n == 0) {
            perror("Socket cerrado\n");
            exit(1);
        }
        printf("Mensaje del cliente: %s\n", buf);

        if (shutdown(cd, SHUT_RD) != 0)
            perror("No fue posible deshabilitar lectura");
    }
    close(cd);
    close(sd);

    return 0;
}
