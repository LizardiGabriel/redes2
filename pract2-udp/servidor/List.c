#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

#include "cJSON.c"

#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>

#include "List.h"

#include <zlib.h>

//#region inicio de list0

char *opcJson(char *opcion, char *msj) {
    cJSON *root = cJSON_Parse(msj);

    // Verificar si la carga del JSON fue exitosa
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        exit(1);
    }

    // Acceder al campo deseado
    cJSON *value = cJSON_GetObjectItemCaseSensitive(root, opcion);

    // Verificar si el campo existe y es una cadena
    if (!value || !cJSON_IsString(value)) {
        fprintf(stderr, "Error: el campo %s no existe o no es una cadena.\n", opcion);
        cJSON_Delete(root); // Liberar el JSON
        exit(1);
    }

    // Obtener el valor del campo
    const char *value_str = cJSON_GetStringValue(value);

    // Imprimir el valor del campo (opcional)
    // printf("El valor de %s es: %s\n", opcion, value_str);

    // Asignar memoria para el valor del campo y copiar el valor
    char *ret = (char *)malloc((strlen(value_str) + 1) * sizeof(char));
    strcpy(ret, value_str);

    // Liberar el JSON
    cJSON_Delete(root);

    return ret;
}

char *listarContenido(char *ruta) {
    DIR *dir = opendir(ruta);

    // Verificar si se pudo abrir el directorio
    if (!dir) {
        perror("Error al abrir el directorio");
        exit(1);
    }

    cJSON *jsonRoot = cJSON_CreateObject();
    if (!jsonRoot) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    cJSON *jsonArrayArchivos = cJSON_CreateArray();
    if (!jsonArrayArchivos) {
        fprintf(stderr, "Error al crear el arreglo JSON para los archivos\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    cJSON *jsonArrayCarpetas = cJSON_CreateArray();
    if (!jsonArrayCarpetas) {
        fprintf(stderr, "Error al crear el arreglo JSON para las carpetas\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // carpeta
            cJSON_AddItemToArray(jsonArrayCarpetas, cJSON_CreateString(entry->d_name));
        } else {
            // archivo
            cJSON_AddItemToArray(jsonArrayArchivos, cJSON_CreateString(entry->d_name));
        }
    }
    closedir(dir);

    cJSON_AddItemToObject(jsonRoot, "archivos", jsonArrayArchivos);
    cJSON_AddItemToObject(jsonRoot, "carpetas", jsonArrayCarpetas);

    char *jsonString = cJSON_PrintUnformatted(jsonRoot);
    if (!jsonString) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    cJSON_Delete(jsonRoot);

    return jsonString;
}

char *directorioActual(char *ruta) {
    // Verificar si se proporcionó una ruta
    if (ruta == NULL) {
        perror("Error: no se proporcionó una ruta");
        exit(1);
    }

    cJSON *jsonRoot = cJSON_CreateObject();
    if (!jsonRoot) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    cJSON *jsonString0 = cJSON_CreateString(ruta);
    if (!jsonString0) {
        fprintf(stderr, "Error al crear el string JSON para la ruta\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    cJSON_AddItemToObject(jsonRoot, "path remoto", jsonString0);

    char *jsonString = cJSON_PrintUnformatted(jsonRoot);
    if (!jsonString) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    cJSON_Delete(jsonRoot);

    return jsonString;
}

char *directorio() {
    char *sepRuta = "/";
    char *directorio = getcwd(NULL, 0);
    if (directorio == NULL) {
        perror("Error al obtener el directorio actual");
        exit(1);
    }
    int tam_nuevo = strlen(directorio) + strlen("carpetita") + 1;
    directorio = (char *)realloc(directorio, tam_nuevo);
    strcat(directorio, sepRuta);
    strcat(directorio, "carpetita");

    DIR *dir = opendir(directorio);
    if (dir) {
        closedir(dir);
    } else {
        mkdir(directorio, 0777);
        DIR *dir1 = opendir(directorio);
        if (dir1) {
            closedir(dir1);
        } else {
            perror("Error al crear el directorio");
            exit(1);
        }
    }
    return directorio;
}

char *cambiarDirectorio(char *ruta) {
    DIR *dir = opendir(ruta);
    char *mensaje_ok = "Directorio cambiado";
    char *mensaje_error = "Error al cambiar el directorio";
    char *mensaje;
    
    if (dir) {
        mensaje = mensaje_ok;
        closedir(dir);
    } else {
        mensaje = mensaje_error;
    }

    cJSON *jsonRoot = cJSON_CreateObject();
    if (!jsonRoot) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    cJSON *jsonString0 = cJSON_CreateString(mensaje);
    if (!jsonString0) {
        fprintf(stderr, "Error al crear el string JSON para el mensaje\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    cJSON_AddItemToObject(jsonRoot, "directorio", jsonString0);

    char *jsonString = cJSON_PrintUnformatted(jsonRoot);
    if (!jsonString) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    cJSON_Delete(jsonRoot);

    return jsonString;
}

char *cambiarDirectorioAnterior(char *ruta_completa) {

    char *error = "error";

    if (ruta_completa == NULL || *ruta_completa == '\0') {
        return error;
    }

   
    char *ultimo_slash = ruta_completa;
    char *caracter_actual = ruta_completa;
    while (*caracter_actual != '\0') {
        if (*caracter_actual == '/') {
            ultimo_slash = caracter_actual;
        }
        caracter_actual++;
    }

    while (ultimo_slash > ruta_completa && *ultimo_slash == '/') {
        ultimo_slash--;
    }

    char *directorio_padre = malloc(ultimo_slash - ruta_completa + 2);
    if (directorio_padre == NULL) {
        return error; 
    }
    strncpy(directorio_padre, ruta_completa, ultimo_slash - ruta_completa + 1);
    directorio_padre[ultimo_slash - ruta_completa + 1] = '\0'; 

    return directorio_padre;
}

char *mandarFileFldr(char *mensaje, int typeFile) {
    cJSON *jsonRoot = cJSON_CreateObject();
    if (!jsonRoot) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    cJSON_AddStringToObject(jsonRoot, "mensaje", mensaje);

    const char *type = (typeFile == 0) ? "file" : "fldr";
    cJSON_AddStringToObject(jsonRoot, "typeFile", type);

    char *jsonString = cJSON_PrintUnformatted(jsonRoot);
    if (!jsonString) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    cJSON_Delete(jsonRoot);

    return jsonString;
}

char *recibido(char *mensaje) {
    cJSON *jsonRoot = cJSON_CreateObject();
    if (!jsonRoot) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    cJSON_AddStringToObject(jsonRoot, "mensaje", mensaje);

    char *jsonString = cJSON_PrintUnformatted(jsonRoot);
    if (!jsonString) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        cJSON_Delete(jsonRoot);
        exit(1);
    }

    cJSON_Delete(jsonRoot);

    return jsonString;
}

char *mkdirCarpeta(char *ruta, char *carpeta) {
    char *sepRuta = "/";
    int tam = strlen(ruta) + 1 + strlen(carpeta) + 1;
    char *rutita = (char *)malloc(tam * sizeof(char));
    strcpy(rutita, ruta);
    strcat(rutita, sepRuta);
    strcat(rutita, carpeta);
    printf("ruta de la carpeta nueva: %s\n", rutita);

    int res = mkdir(rutita, 0777);
    char *mensaje;
    if (res == 0) {
        mensaje = "Carpeta creada";
    } else {
        mensaje = "Error al crear la carpeta";
    }

    char *ret = recibido(mensaje);
    return ret;
}

char *rmdirAlgo(char *ruta, char *archivo) {
    char *sepRuta = "/";

    char *mensaje;

    int tam = strlen(ruta) + 1 + strlen(archivo) + 1;
    char *rutita = (char *)malloc(tam * sizeof(char));
    strcpy(rutita, ruta);
    strcat(rutita, sepRuta);
    strcat(rutita, archivo);
    printf("rutita a borrar: %s\n", rutita);

    if (access(rutita, F_OK) != -1) {
        // checae si rutita es un fldr o un file
        struct stat st;
        stat(rutita, &st);
        if (S_ISDIR(st.st_mode)) {
            // es un fldr
            int tamCmnd2 = strlen(rutita) + 8;
            char *comando2 = (char *)malloc(tamCmnd2 * sizeof(char));
            strcpy(comando2, "rm -r ");
            strcat(comando2, rutita);
            system(comando2);
            free(comando2);
            mensaje = "--> Carpeta eliminada";

        } else {
            // es un file
            int res = remove(rutita);
            if (res == 0) {
                mensaje = "--> Archivo eliminado";
            } else {
                mensaje = "Error al eliminar el archivo";
            }
        }
    } else {
        tam = strlen("No existe: ") + strlen(rutita) + 1;
        mensaje = (char *)malloc(tam * sizeof(char));
        strcpy(mensaje, "No existe: ");
        strcat(mensaje, rutita);
        printf("mensaje: %s\n", mensaje);
    }

    char *ret = recibido(mensaje);
    return ret;
}

char *rutaAbsoluta(char *ruta, char *nombre) {
    char *sepRuta = "/";
    int tam = strlen(ruta) + 1 + strlen(nombre) + 1;
    char *rutita = (char *)malloc(tam * sizeof(char));
    strcpy(rutita, ruta);
    strcat(rutita, sepRuta);
    strcat(rutita, nombre);
    printf("rutita de f rutaAbso: %s\n", rutita);
    return rutita;
}

int existe(char *ruta) {

    if (access(ruta, F_OK) != -1) {
        return 1;
    } else {
        return 0;
    }
}

int calcularTamanito(char *ruta) {
    // calcular el tamaño de un archivo con esta ruta

    struct stat st;
    stat(ruta, &st);
    int tam = st.st_size;
    return tam;
}

char *recorrerDirectorio(char *ruta) {
    DIR *dir;
    struct dirent *entry;
    int tam = 0;
    char *retornar = (char *)malloc(1000 * sizeof(char));
    retornar[0] = '\0';

    if (!(dir = opendir(ruta))) {
        perror("opendir");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", ruta, entry->d_name);

        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                strcat(retornar, "{\"fldr\":\"");
                strcat(retornar, entry->d_name);
                strcat(retornar, "\",\"cont\":[");
                char *subdir = recorrerDirectorio(full_path);
                int subdir_len = strlen(subdir);
                if (subdir_len > 0 && subdir[subdir_len - 1] == ',') {
                    subdir[subdir_len - 1] = '\0'; // remove the trailing comma
                }
                strcat(retornar, subdir);
                strcat(retornar, "]},");
            }
        } else {
            strcat(retornar, "{\"file\":\"");
            strcat(retornar, entry->d_name);
            strcat(retornar, "\"},");
        }
    }

    closedir(dir);
    // printf("close dir pipipi\n");
    int len = strlen(retornar);
    if (len > 0 && retornar[len - 1] == ',') {
        retornar[len - 1] = '\0'; // remove the trailing comma
    }
    return retornar;
}

char *generarList1(char *ruta) {

    char *obt = recorrerDirectorio(ruta);
    int tam = strlen(obt);

    int tamRuta = strlen(ruta);
    char *obt2 = (char *)malloc((tam + 50 + tamRuta) * sizeof(char));
    obt2[0] = '\0';
    strcat(obt2, "{");
    strcat(obt2, "\"fldr\":\"");
    strcat(obt2, ruta);
    strcat(obt2, "\",\"cont\":[");
    strcat(obt2, obt);
    strcat(obt2, "]");
    strcat(obt2, "}");

    return obt2;
}

void verificar(char *msj) {
    cJSON *root = cJSON_Parse(msj);
    if (!root) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error antes de la posición %ld: %s\n", error_ptr - msj, error_ptr);
        }
        exit(1);
    }

    // Verificar que la raíz sea un objeto
    if (!cJSON_IsObject(root)) {
        fprintf(stderr, "Error: la raíz no es un objeto JSON\n");
        cJSON_Delete(root);
        exit(1);
    }

    printf("El JSON es válido\n");

    // Liberar la memoria asignada al objeto cJSON
    cJSON_Delete(root);
}