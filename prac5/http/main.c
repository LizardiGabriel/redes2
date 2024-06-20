#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <dirent.h>
#include <math.h>
#include <sys/stat.h>

#include "cJSON.c"
#include "cJSON.h"

short PORT_NO = 1607;
int CONNECTION_NUMBER = 200;
int BUFFER_SIZE = 60000;

int MAX_BODY_SIZE = 60000;
// Keeps the number of the threads working simultaneously.
int thread_count = 0;
// To control thread_counter.
sem_t mutex;
char PATH[] = "/Users/gabriellizardi/home/redes2/prac5/http";

const unsigned char decodeTable[] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
                                     0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
                                     0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x3e, 0x40, 0x40, 0x40, 0x3f,
                                     0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
                                     0x40, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
                                     0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x40, 0x40, 0x40, 0x40, 0x40,
                                     0x40, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
                                     0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x40, 0x40, 0x40, 0x40, 0x40};

unsigned char *decodeBase64(unsigned char *keyBase64, unsigned int length, unsigned int *out_len) {
    *out_len = ceil(length * 3 / 4);
    unsigned char *data = (char *)calloc(*out_len, sizeof(char));
    unsigned int i = 0;
    for (unsigned int k = 0; k < length; k += 4) {
        data[i] = decodeTable[keyBase64[k]] << 2 | (decodeTable[keyBase64[k + 1]] >> 4);
        data[i + 1] = (decodeTable[keyBase64[k + 1]] & 15) << 4 | decodeTable[keyBase64[k + 2]] >> 2;
        data[i + 2] = (decodeTable[keyBase64[k + 2]] & 3) << 6 | decodeTable[keyBase64[k + 3]];
        i += 3;
    }
    return data;
}

void formatSize(char *buffer, size_t size, long long bytes) {
    const char *suffixes[] = {"B", "KB", "MB", "GB"};
    size_t i = 0;
    double dblBytes = bytes;

    while (dblBytes >= 1024 && i < 3) {
        dblBytes /= 1024;
        i++;
    }
    snprintf(buffer, size, "%.2f%s", dblBytes, suffixes[i]);
}

void listFilesAndSizes(const char *path, char *result) {
    struct dirent *entry;
    struct stat fileStat;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("Unable to open directory");
        return;
    }

    strcpy(result, "{");

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0')))
            continue;

        // Construct full path
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        // Get file stats
        if (stat(fullPath, &fileStat) == -1) {
            perror("Unable to get file stats");
            continue;
        }

        // Format size
        char sizeBuffer[32];
        formatSize(sizeBuffer, sizeof(sizeBuffer), (long long)fileStat.st_size);

        // Append to result string
        char temp[2048];
        snprintf(temp, sizeof(temp), "\"%s\": \"%s\", ", entry->d_name, sizeBuffer);
        strcat(result, temp);
    }

    // Remove trailing comma and space, and add closing brace
    size_t len = strlen(result);
    if (len > 1) {
        result[len - 2] = '\0';
    }
    strcat(result, "}");

    closedir(dir);
}

void send_response(int socket, const char *header, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    sprintf(response, "%s\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n%s", header, content_type, strlen(body), body);
    
    printf("respuesta>>>:\n%s\nfin respuesta<<<<<<<<<<\n", response);
    write(socket, response, strlen(response));
}

int dropFile(const char *path) {
    char *fullPath = malloc(strlen(PATH) + strlen("/uploads/") + strlen(path) + 1);
    sprintf(fullPath, "%s/uploads/%s", PATH, path);
    printf(">>>>>>>>>>>>>>>fullPath: %s\n", fullPath);

    if (remove(fullPath) == 0) {
        free(fullPath);
        return 0;
    } else {
        free(fullPath);
        return -1;
    }
}

void send_file(int socket, const char *header, const char *content_type, const char *file_path) {

    printf(">>>>>>>>>>>>>>>file_path: %s\n", file_path);
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        send_response(socket, "HTTP/1.0 404 Not Found", "text/html", "<!doctype html><html><body>404 File Not Found</body></html>");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_buffer = malloc(file_size);
    fread(file_buffer, 1, file_size, file);
    fclose(file);

    char header_buffer[BUFFER_SIZE];
    sprintf(header_buffer, "%s\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", header, content_type, file_size);
    write(socket, header_buffer, strlen(header_buffer));
    write(socket, file_buffer, file_size);

    free(file_buffer);
}

const char *get_content_type(const char *file_path) {
    const char *ext = strrchr(file_path, '.');
    if (ext == NULL) {
        return "application/octet-stream";
    }
    if (strcmp(ext, ".html") == 0) {
        return "text/html";
    } else if (strcmp(ext, ".css") == 0) {
        return "text/css";
    } else if (strcmp(ext, ".js") == 0) {
        return "application/javascript";
    } else if (strcmp(ext, ".jpg") == 0) {
        return "image/jpeg";
    } else if (strcmp(ext, ".png") == 0) {
        return "image/png";
    } else if (strcmp(ext, ".gif") == 0) {
        return "image/gif";
    } else if (strcmp(ext, ".txt") == 0) {
        return "text/plain";
    } else if (strcmp(ext, ".json") == 0) {
        return "application/json";
    } else if (strcmp(ext, ".pdf") == 0) {
        return "application/pdf";
    } else if (strcmp(ext, ".xml") == 0) {
        return "application/xml";
    } else if (strcmp(ext, ".ico") == 0) {
        return "image/x-icon";
    } else if (strcmp(ext, ".csv") == 0) {
        return "text/csv";
    } else if (strcmp(ext, ".mp4") == 0) {
        return "video/mp4";
    } else if (strcmp(ext, ".woff") == 0) {
        return "font/woff";
    } else if (strcmp(ext, ".woff2") == 0) {
        return "font/woff2";
    } else {
        return "application/octet-stream";
    }
}

void handle_request(int sock, char *request_lines[], char *method, char *file_path, char *body) {
    char response_body[BUFFER_SIZE];
    memset(response_body, 0, sizeof(response_body));
    char full_path[BUFFER_SIZE];
    if (strcmp(file_path, "/") == 0) {
        file_path = "/index.html";
    }

    if (strncmp(file_path, "/cgi-bin/ls", 11) == 0) {
        char files_and_sizes[2048];
        listFilesAndSizes(PATH, files_and_sizes);
        send_response(sock, "HTTP/1.0 200 OK", "application/json", files_and_sizes);
        return;
    } else if (strncmp(file_path, "/uploads/", 9) == 0) {
        sprintf(full_path, "%s%s", PATH, file_path);
    } else {
        sprintf(full_path, "%s%s", PATH, file_path);
    }

    if (strcmp(method, "GET") == 0) {
        const char *content_type = get_content_type(full_path);
        send_file(sock, "HTTP/1.0 200 OK", content_type, full_path);
    } else if (strcmp(method, "POST") == 0) {
        printf(">>>>>>>>>>body:%s\n", body);
        char *decoded_body;
        unsigned int decode_len;
        char file_size_buffer[32];

        cJSON *root = cJSON_Parse(body);
        char *base64 = cJSON_GetObjectItem(root, "data")->valuestring;
        char *file_name = cJSON_GetObjectItem(root, "fileName")->valuestring;

        decoded_body = (char *)decodeBase64((unsigned char *)base64, strlen(base64), &decode_len);

        FILE *file = fopen(full_path, "wb");
        fwrite(decoded_body, sizeof(char), decode_len, file);
        fclose(file);

        free(decoded_body);

        long long file_size = decode_len;
        formatSize(file_size_buffer, sizeof(file_size_buffer), file_size);
        cJSON_AddStringToObject(root, "size", file_size_buffer);
        char *new_body = cJSON_Print(root);
        send_response(sock, "HTTP/1.0 200 OK", "application/json", new_body);
        free(new_body);
        cJSON_Delete(root);

    } else if (strcmp(method, "PUT") == 0) {
        printf(">>>>>>>>>>body:%s\n", body);
        char *decoded_body;
        unsigned int decode_len;
        char file_size_buffer[32];

        cJSON *root = cJSON_Parse(body);
        char *base64 = cJSON_GetObjectItem(root, "data")->valuestring;
        char *file_name = cJSON_GetObjectItem(root, "fileName")->valuestring;

        decoded_body = (char *)decodeBase64((unsigned char *)base64, strlen(base64), &decode_len);

        FILE *file = fopen(full_path, "wb");
        fwrite(decoded_body, sizeof(char), decode_len, file);
        fclose(file);

        free(decoded_body);

        long long file_size = decode_len;
        formatSize(file_size_buffer, sizeof(file_size_buffer), file_size);
        cJSON_AddStringToObject(root, "size", file_size_buffer);
        char *new_body = cJSON_Print(root);
        send_response(sock, "HTTP/1.0 200 OK", "application/json", new_body);
        free(new_body);
        cJSON_Delete(root);

    } else if (strcmp(method, "HEAD") == 0) {
        const char *content_type = get_content_type(full_path);
        send_file(sock, "HTTP/1.0 200 OK", content_type, full_path);

    } else if (strcmp(method, "DELETE") == 0) {
        printf(">>>>>>>>>>body:%s\n", body);
        char *decoded_body;
        unsigned int decode_len;
        char file_size_buffer[32];

        cJSON *root = cJSON_Parse(body);
        char *file_name = cJSON_GetObjectItem(root, "fileName")->valuestring;

        long long file_size = decode_len;
        formatSize(file_size_buffer, sizeof(file_size_buffer), file_size);
        cJSON_AddStringToObject(root, "size", file_size_buffer);
        char *new_body = cJSON_Print(root);

        int result = dropFile(file_name);
        if (result == 0) {
            send_response(sock, "HTTP/1.0 200 OK", "application/json", new_body);
        } else {
            send_response(sock, "HTTP/1.0 500 Internal Server Error", "application/json", "{}");
        }

        free(new_body);
        cJSON_Delete(root);

    } else {
        send_response(sock, "HTTP/1.0 405 Method Not Allowed", "text/html", "<!doctype html><html><body>405 Method Not Allowed</body></html>");
    }
}

void *connection_handler(void *socket_desc) {
    int request;
    char client_reply[BUFFER_SIZE];
    char *request_lines[3];
    char file_name[BUFFER_SIZE];
    memset(file_name, '\lo', sizeof(file_name));
    int sock = *((int *)socket_desc);

    while (1) {
        request = recv(sock, client_reply, BUFFER_SIZE, 0);
        if (request < 0) {
            if (errno == EWOULDBLOCK) {
                // No hay datos disponibles, esperar un poco antes de intentar nuevamente
                usleep(100000);
                continue;
            } else {
                perror("Recv failed");
                break;
            }
        } else if (request == 0) {
            puts("Client disconnected unexpectedly.");
            break;
        }

        sem_wait(&mutex);
        thread_count++;

        if (thread_count > 10) {
            char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>System is busy right now.</body></html>";
            write(sock, message, strlen(message));
            thread_count--;
            sem_post(&mutex);
            free(socket_desc);
            shutdown(sock, SHUT_RDWR);
            close(sock);
            pthread_exit(NULL);
        }
        sem_post(&mutex);

        printf("%s\n", client_reply);

        char *aux = client_reply;
        char *line_end = strstr(aux, "\r\n");
        char *body = strstr(aux, "\r\n\r\n");

        if (body) {
            body += 4;
        }

        if (line_end) {
            *line_end = '\0';
        }

        request_lines[0] = strtok(client_reply, " \t\n");

        if (request_lines[0]) {
            request_lines[1] = strtok(NULL, " \t");
            request_lines[2] = strtok(NULL, " \t\n");

            if (strncmp(request_lines[0], "GET", 3) == 0) {
                handle_request(sock, request_lines, "GET", request_lines[1], NULL);
            } else if (strncmp(request_lines[0], "POST", 4) == 0) {
                printf(">>>>>>>> POST\n");
                handle_request(sock, request_lines, "POST", request_lines[1], body);
            } else if (strncmp(request_lines[0], "PUT", 3) == 0) {
                handle_request(sock, request_lines, "PUT", request_lines[1], body);
            } else if (strncmp(request_lines[0], "HEAD", 4) == 0) {
                printf(">>>>>>> mandar a comparar head <<<<<<<<<<<\n");
                handle_request(sock, request_lines, "HEAD", request_lines[1], NULL);
            } else if (strncmp(request_lines[0], "DELETE", 4) == 0) {
                handle_request(sock, request_lines, "DELETE", request_lines[1], body);
            } else {
                send_response(sock, "HTTP/1.0 405 Method Not Allowed", "text/html", "<!doctype html><html><body>405 Method Not Allowed</body></html>");
            }
        }
    }

    free(socket_desc);
    shutdown(sock, SHUT_RDWR);
    close(sock);
    sem_wait(&mutex);
    thread_count--;
    sem_post(&mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket, c, *new_sock;
    struct sockaddr_in server, client;

    sem_init(&mutex, 0, 1);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        return 1;
    }

    // Configurar el socket en modo no bloqueante
    int flags = fcntl(server_socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return 1;
    }

    if (fcntl(server_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL O_NONBLOCK");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NO);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed. Error");
        return 1;
    }

    listen(server_socket, CONNECTION_NUMBER);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c)) >= 0) {
        if (client_socket == -1) {
            if (errno == EWOULDBLOCK) {
                // No hay conexiones entrantes, esperar un poco antes de intentar nuevamente
                usleep(100000);
                continue;
            } else {
                perror("Accept failed");
                break;
            }
        }

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_socket;

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0) {
            perror("Could not create thread");
            return 1;
        }

        pthread_detach(sniffer_thread);
    }

    if (client_socket < 0) {
        perror("Accept failed");
        return 1;
    }

    close(server_socket);
    return 0;
}
