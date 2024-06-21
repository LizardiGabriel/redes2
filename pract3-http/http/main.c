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

short PORT_NO = 8888;
int CONNECTION_NUMBER = 200;
int BUFFER_SIZE = 60000;

int MAX_BODY_SIZE = 60000;
// Keeps the number of the threads working simultaneously.
int thread_count = 0;
// To control thread_counter.
sem_t mutex;
char PATH[] = "/home/lizarpez/Desktop/git/redes2/pract3-http/http";

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
    } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return "image/jpeg";
    } else if (strcmp(ext, ".png") == 0) {
        return "image/png";
    } else if (strcmp(ext, ".gif") == 0) {
        return "image/gif";
    } else if (strcmp(ext, ".ico") == 0) {
        return "image/x-icon";
    } else if (strcmp(ext, ".pdf") == 0) {
        return "application/pdf";
    } else if (strcmp(ext, ".mp4") == 0) {
        return "video/mp4";
    }
    return "application/octet-stream";
}

void handle_request(int socket, char *request_lines[], char *method, char *file_name, char *body) {
    if (strncmp(request_lines[2], "HTTP/1.0", 8) != 0 && strncmp(request_lines[2], "HTTP/1.1", 8) != 0) {
        char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>400 Bad Request</body></html>";
        write(socket, message, strlen(message));
    } else {
        char *p = strstr(request_lines[1], "?");
        if (p) {
            char *lineas[2];
            lineas[0] = strtok(request_lines[1], "?");
            lineas[1] = strtok(NULL, "?");
            char message[1024] = "HTTP/1.0 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body bgcolor=\"#AACCFF\"> Param: ";
            strcat(message, lineas[1]);
            strcat(message, "</body></html>");
            write(socket, message, strlen(message));
        } else {
            if (strcmp(file_name, "/") == 0) {
                strcpy(file_name, "/index.html");
            }

            char full_path[BUFFER_SIZE];
            sprintf(full_path, "%s%s", PATH, file_name);

            if (strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0){
                // Handle GET and HEAD request
                printf(">>>>>>>>>>>>>>>full_path: %s\n", full_path);

                if (strcmp(file_name, "/obtenerArchivos") == 0) {
                    // Devolver una lista de nombres
                    char result[8192]; // Adjust size as needed
                    listFilesAndSizes("./uploads", result);
                    printf(">>>>>>>>>>>>>>>result: %s\n", result);
                    send_response(socket, "HTTP/1.0 200 OK", "text/html", result);
                } else {

                    FILE *fp = fopen(full_path, "rb");
                    if (fp) {
                        const char *content_type = get_content_type(full_path);

                        if (strcmp(method, "GET") == 0) {
                            printf(">>>>>>>>>>>>>>> g e t <<<<<<<<<<<<<<<:\n");
                            send_file(socket, "HTTP/1.0 200 OK", content_type, full_path);
                        } else if (strcmp(method, "HEAD") == 0) {
                            printf(">>>>>>>>>>>>>>> entre a head<<<<<<<<<<<<<<<:\n");
                            fseek(fp, 0, SEEK_END);
                            long file_size = ftell(fp);
                            fclose(fp);
                                                        
                            send_response(socket, "HTTP/1.0 200 OK", content_type, "");

                            // write(socket, header_buffer, strlen(header_buffer));
                        }
                    } else {
                        send_response(socket, "HTTP/1.0 404 Not Found", "text/html", "<!doctype html><html><body>404 File Not Found</body></html>");
                    }
                }
            }else if (strcmp(method, "POST") == 0) {

                if (body) {
                    char response_body[MAX_BODY_SIZE];

                    sprintf(response_body, "<!doctype html><html><body>%s Parameters: %s</body></html>", method, body);
                    send_response(socket, "HTTP/1.0 200 OK", "text/html", response_body);
                } else {
                    send_response(socket, "HTTP/1.0 400 Bad Request", "text/html", "<!doctype html><html><body>400 Bad Request</body></html>");
                }

            } else if (strcmp(method, "PUT") == 0) {
                // Handle PUT request for file upload
                if (body) {
                    // Parse the JSON body to extract file information
                    cJSON *json = cJSON_Parse(body);
                    if (json == NULL) {
                        send_response(socket, "HTTP/1.0 400 Bad Request", "text/html", "<!doctype html><html><body>400 Bad Request - Invalid JSON</body></html>");
                    } else {
                        cJSON *fileName = cJSON_GetObjectItem(json, "fileName");
                        cJSON *fileContent = cJSON_GetObjectItem(json, "fileContent");

                        if (fileName && fileContent) {
                            char file_path[BUFFER_SIZE];
                            sprintf(file_path, "%s/uploads/%s", PATH, fileName->valuestring);

                            // Decode base64 and save to file
                            FILE *file = fopen(file_path, "wb");
                            if (file) {
                                unsigned int output_length;
                                unsigned char *decoded_data = decodeBase64(fileContent->valuestring, strlen(fileContent->valuestring), &output_length);
                                fwrite(decoded_data, 1, output_length, file);
                                fclose(file);
                                free(decoded_data);

                                send_response(socket, "HTTP/1.0 200 OK", "", "{\"status\": \"success\", \"message\": \"File uploaded successfully\", \"ruta\": \"ola\"}");
                            } else {
                                send_response(socket, "HTTP/1.0 500 Internal Server Error", "text/html", "<!doctype html><html><body>500 Internal Server Error - Unable to save file</body></html>");
                            }
                        } else {
                            send_response(socket, "HTTP/1.0 400 Bad Request", "text/html", "<!doctype html><html><body>400 Bad Request - Missing file information</body></html>");
                        }

                        cJSON_Delete(json);
                    }
                } else {
                    send_response(socket, "HTTP/1.0 400 Bad Request", "text/html", "<!doctype html><html><body>400 Bad Request</body></html>");
                }
            } else if (strcmp(method, "DELETE") == 0) {

               if (body) {
                    // borrar el archivo body
                    int result = dropFile(body);
                    printf("-------->>result: %d\n", result);

                    char response_body[MAX_BODY_SIZE];

                    char *okJson = "{\"status\": \"success\", \"message\": \"File uploaded successfully\", \"ruta\": \"ola\"}";
                    send_response(socket, "HTTP/1.0 200 OK", "", okJson);

                    
                } else {
                    send_response(socket, "HTTP/1.0 400 Bad Request", "text/html", "<!doctype html><html><body>400 Bad Request</body></html>");
                }


            } else {
                send_response(socket, "HTTP/1.0 400 Bad Request", "text/html", "<!doctype html><html><body>400 Bad Request</body></html>");
            }
        }
    }
}

void *connection_handler(void *socket_desc) {
    int request;
    char client_reply[BUFFER_SIZE];
    char *request_lines[3];
    char file_name[BUFFER_SIZE];
    memset(file_name, '\0', sizeof(file_name));
    int sock = *((int *)socket_desc);

    request = recv(sock, client_reply, BUFFER_SIZE, 0);

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

    if (request < 0) {
        puts("Recv failed");
    } else if (request == 0) {
        puts("Client disconnected unexpectedly.");
    } else {
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
    sem_init(&mutex, 0, 1);
    int socket_desc, new_socket, c, *new_sock;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        puts("Could not create socket");
        return 1;
    }
    int v = 1;
    int op = setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT_NO);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        puts("Binding failed");
        return 1;
    }

    listen(socket_desc, 20);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))) {
        puts("Connection accepted \n");

        pthread_t sniffer_thread;
        new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0) {
            puts("Could not create thread");
            return 1;
        }
    }

    return 0;
}
