#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PORT_NO 8888
#define CONNECTION_NUMBER 10
#define BUFFER_SIZE 1024

int thread_count = 0;
sem_t mutex;
char PATH[] = "/Users/gabriellizardi/home/redes2/pract3-http/http";

// Manejo de archivos JPEG
void jpeg_handler(int socket, char *file_name) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    char full_path[strlen(PATH) + strlen(file_name) + 1];
    snprintf(full_path, sizeof(full_path), "%s%s", PATH, file_name);
    int fp = open(full_path, O_RDONLY);

    if (fp > 0) {
        int bytes;
        send(socket, "HTTP/1.0 200 OK\r\nContent-Type: image/jpeg\r\n\r\n", 45, 0);
        while ((bytes = read(fp, buffer, BUFFER_SIZE)) > 0) {
            write(socket, buffer, bytes);
            memset(buffer, '\0', sizeof(buffer));
        }
        close(fp);
    } else {
        write(socket, "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
    }
}

// Manejo de archivos HTML
void html_handler(int socket, char *file_name) {
    char *full_path = (char *)malloc((strlen(PATH) + strlen(file_name) + 1) * sizeof(char));
    snprintf(full_path, strlen(PATH) + strlen(file_name) + 1, "%s%s", PATH, file_name);
    FILE *fp = fopen(full_path, "r");

    if (fp) {
        fseek(fp, 0, SEEK_END);
        long bytes_read = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *buffer = (char *)malloc(bytes_read * sizeof(char));

        send(socket, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", 44, 0);
        fread(buffer, bytes_read, 1, fp);
        write(socket, buffer, bytes_read);
        free(buffer);
        fclose(fp);
    } else {
        write(socket, "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
    }
    free(full_path);
}

// Manejo de archivos ICO
void ico_handler(int socket, char *file_name) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    char full_path[strlen(PATH) + strlen(file_name) + 1];
    snprintf(full_path, sizeof(full_path), "%s%s", PATH, file_name);
    int fp = open(full_path, O_RDONLY);

    if (fp > 0) {
        int bytes;
        send(socket, "HTTP/1.0 200 OK\r\nContent-Type: image/x-icon\r\n\r\n", 47, 0);
        while ((bytes = read(fp, buffer, BUFFER_SIZE)) > 0) {
            write(socket, buffer, bytes);
            memset(buffer, '\0', BUFFER_SIZE);
        }
        close(fp);
    } else {
        write(socket, "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
    }
}

// Manejo de la petición POST
void post_handler(int socket, char *file_name, char *content) {
    char full_path[strlen(PATH) + strlen(file_name) + 1];
    snprintf(full_path, sizeof(full_path), "%s%s", PATH, file_name);
    FILE *fp = fopen(full_path, "w");

    if (fp) {
        fwrite(content, sizeof(char), strlen(content), fp);
        fclose(fp);
        char *message = "HTTP/1.0 201 Created\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>File created</body></html>";
        write(socket, message, strlen(message));
    } else {
        char *message = "HTTP/1.0 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>Error creating file</body></html>";
        write(socket, message, strlen(message));
    }
}

// Manejo de la petición PUT
void put_handler(int socket, char *file_name, char *content) {
    char full_path[strlen(PATH) + strlen(file_name) + 1];
    snprintf(full_path, sizeof(full_path), "%s%s", PATH, file_name);
    FILE *fp = fopen(full_path, "w");

    if (fp) {
        fwrite(content, sizeof(char), strlen(content), fp);
        fclose(fp);
        char *message = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>File updated</body></html>";
        write(socket, message, strlen(message));
    } else {
        char *message = "HTTP/1.0 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>Error updating file</body></html>";
        write(socket, message, strlen(message));
    }
}

// Manejo de la petición HEAD
void head_handler(int socket, char *file_name) {
    char full_path[strlen(PATH) + strlen(file_name) + 1];
    snprintf(full_path, sizeof(full_path), "%s%s", PATH, file_name);

    if (access(full_path, F_OK) != -1) {
        char *message = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
        write(socket, message, strlen(message));
    } else {
        char *message = "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        write(socket, message, strlen(message));
    }
}

// Manejo de conexiones
void *connection_handler(void *socket_desc) {
    int sock = *((int *)socket_desc);
    char client_reply[BUFFER_SIZE];
    int request = recv(sock, client_reply, BUFFER_SIZE, 0);

    sem_wait(&mutex);
    thread_count++;
    if (thread_count > CONNECTION_NUMBER) {
        char *message = "HTTP/1.0 503 Service Unavailable\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>Server is busy</body></html>";
        write(sock, message, strlen(message));
        thread_count--;
        sem_post(&mutex);
        free(socket_desc);
        close(sock);
        pthread_exit(NULL);
    }
    sem_post(&mutex);

    if (request < 0) {
        puts("Recv failed");
    } else if (request == 0) {
        puts("Client disconnected unexpectedly.");
    } else {
        printf("Request:\n%s\n", client_reply);
        char *method = strtok(client_reply, " \t\r\n");
        char *uri = strtok(NULL, " \t");
        char *version = strtok(NULL, " \t\r\n");
        char *content = strstr(client_reply, "\r\n\r\n");

        if (content) {
            content += 4;
        }

        if (!method || !uri || !version) {
            char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>400 Bad Request</body></html>";
            write(sock, message, strlen(message));
        } else {
            char file_name[BUFFER_SIZE];
            snprintf(file_name, sizeof(file_name), "%s", uri);
            char *ext = strrchr(file_name, '.');

            if (ext && strcmp(ext, ".html") == 0) {
                if (strcmp(method, "GET") == 0) {
                    html_handler(sock, file_name);
                } else if (strcmp(method, "HEAD") == 0) {
                    head_handler(sock, file_name);
                } else {
                    char *message = "HTTP/1.0 405 Method Not Allowed\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>405 Method Not Allowed</body></html>";
                    write(sock, message, strlen(message));
                }
            } else if (ext && strcmp(ext, ".jpeg") == 0) {
                if (strcmp(method, "GET") == 0) {
                    jpeg_handler(sock, file_name);
                } else {
                    char *message = "HTTP/1.0 405 Method Not Allowed\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>405 Method Not Allowed</body></html>";
                    write(sock, message, strlen(message));
                }
            } else if (ext && strcmp(ext, ".ico") == 0) {
                if (strcmp(method, "GET") == 0) {
                    ico_handler(sock, file_name);
                } else {
                    char *message = "HTTP/1.0 405 Method Not Allowed\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>405 Method Not Allowed</body></html>";
                    write(sock, message, strlen(message));
                }
            } else if (strcmp(method, "POST") == 0) {
                post_handler(sock, file_name, content);
            } else if (strcmp(method, "PUT") == 0) {
                put_handler(sock, file_name, content);
            } else {
                char *message = "HTTP/1.0 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>501 Not Implemented</body></html>";
                write(sock, message, strlen(message));
            }
        }
    }

    sem_wait(&mutex);
    thread_count--;
    sem_post(&mutex);

    free(socket_desc);
    close(sock);
    pthread_exit(NULL);
}

// Inicialización del socket del servidor
int server_socket_init(void) {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        puts("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NO);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        puts("Bind failed");
        return 1;
    }

    listen(socket_desc, CONNECTION_NUMBER);
    c = sizeof(struct sockaddr_in);

    sem_init(&mutex, 0, 1);

    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))) {
        int *new_sock = (int *)malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&thread_id, NULL, connection_handler, (void *)new_sock) < 0) {
            puts("Thread creation failed");
            return 1;
        }
        pthread_detach(thread_id);
    }

    if (client_sock < 0) {
        puts("Accept failed");
        return 1;
    }

    return 0;
}

int main(void) {
    return server_socket_init();
}
