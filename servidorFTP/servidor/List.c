//
// Created by lizardi on 09/03/24.
//

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <jansson.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>

#include "List.h"

#include <zlib.h>

char *opcJson(char *opcion, char *msj) {

    json_error_t error;
    json_t *root = json_loads(msj, 0, &error);

    // Verificar si la carga del JSON fue exitosa
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        exit(1);
    }

    // Acceder al campo "nombre"
    json_t *nombre = json_object_get(root, opcion);

    // Verificar si el campo "nombre" existe y es un string
    if (!nombre || !json_is_string(nombre)) {
        fprintf(stderr, "Error: por que no lee este json.\n");
        json_decref(root); // Liberar el JSON
        exit(1);
    }

    // Obtener el valor del campo "nombre"
    const char *nombre_value = json_string_value(nombre);

    // Imprimir el valor del campo "nombre"
    // printf("El nombre es: %s\n", nombre_value);

    char *ret = (char *)malloc((strlen(nombre_value) + 1)*sizeof(char));
    strcpy(ret, nombre_value);

    // Liberar el JSON
    json_decref(root);

    return ret;
}

char *listarContenido(char *ruta) {

    DIR *dir = opendir(ruta);

    char **archivos;
    char **carpetas;
    int num_elementos_arch;
    int num_elementos_car;

    // Verificar si se pudo abrir el directorio
    if (dir) {
        int max_elementos = 100;
        archivos = (char **)malloc(max_elementos * sizeof(char *));
        carpetas = (char **)malloc(max_elementos * sizeof(char *));

        if (archivos == NULL) {
            perror("Error al asignar memoria");
            exit(1);
        }

        num_elementos_arch = 0;
        num_elementos_car = 0;
        // Iterar sobre los elementos del directorio
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {

            if (entry->d_type == DT_DIR) {
                // carpeta
                carpetas[num_elementos_car] = (char *)malloc((strlen(entry->d_name) + 1)*sizeof(char));
                strcpy(carpetas[num_elementos_car], entry->d_name);
                num_elementos_car++;

            } else {
                // archivo
                archivos[num_elementos_arch] = (char *)malloc((strlen(entry->d_name) + 1)*sizeof(char));
                strcpy(archivos[num_elementos_arch], entry->d_name);
                num_elementos_arch++;
            }
        }
        closedir(dir);

    } else {
        perror("Error al abrir el directorio");
        exit(1);
    }

    // Crear un objeto JSON raíz
    json_t *jsonRoot = json_object();
    if (jsonRoot == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    // Crear un objeto JSON de tipo arreglo para almacenar los nombres de los archivos
    json_t *jsonArray = json_array();
    if (jsonArray == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON\n");
        json_decref(jsonRoot);
        exit(1);
    }
    // Crear un objeto JSON de tipo arreglo para almacenar los nombres de las carpetas
    json_t *jsonArrayCar = json_array();
    if (jsonArrayCar == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON\n");
        json_decref(jsonRoot);
        exit(1);
    }

    // Iterar sobre el arreglo de strings y agregar cada elemento al arreglo JSON
    for (int i = 0; i < num_elementos_arch; i++) {
        json_t *jsonString = json_string(archivos[i]);
        if (jsonString == NULL) {
            fprintf(stderr, "Error al crear el string JSON\n");
            json_decref(jsonArray);
            exit(1);
        }
        json_array_append(jsonArray, jsonString);
    }

    // Añadir el arreglo JSON al objeto raíz
    json_object_set_new(jsonRoot, "archivos", jsonArray);

    // Iterar sobre el arreglo de strings y agregar cada elemento al arreglo JSON
    for (int i = 0; i < num_elementos_car; i++) {
        json_t *jsonString = json_string(carpetas[i]);
        if (jsonString == NULL) {
            fprintf(stderr, "Error al crear el string JSON\n");
            json_decref(jsonArrayCar);
            exit(1);
        }
        json_array_append(jsonArrayCar, jsonString);
    }
    // Añadir el arreglo JSON al objeto raíz
    json_object_set_new(jsonRoot, "carpetas", jsonArrayCar);

    // Convertir el objeto JSON en una cadena JSON formateada
    char *jsonString = json_dumps(jsonRoot, JSON_COMPACT);
    if (jsonString == NULL) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        json_decref(jsonRoot);
        exit(1);
    }

    return jsonString;

    // Liberar la memoria asignada para el objeto JSON y la cadena JSON
    json_decref(jsonRoot);
    json_decref(jsonArray);
    free(jsonString);

    for (int i = 0; i < num_elementos_arch; i++) {
        free(archivos[i]);
    }
    free(archivos);
    for (int i = 0; i < num_elementos_car; i++) {
        free(carpetas[i]);
    }
    free(carpetas);
}

char *directorioActual(char *ruta) {
    char *directorio = ruta;
    if (directorio == NULL) {
        perror("Error al obtener el directorio actual");
        exit(1);
    }

    // Crear un objeto JSON raíz
    json_t *jsonRoot = json_object();
    if (jsonRoot == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    // Crear un objeto JSON de tipo arreglo para almacenar los nombres de los archivos
    json_t *jsonArray = json_array();
    if (jsonArray == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON\n");
        json_decref(jsonRoot);
        exit(1);
    }

    // Crear un objeto JSON para almacenar la ruta del directorio
    json_t *jsonString0 = json_string(directorio);
    if (jsonString0 == NULL) {
        fprintf(stderr, "Error al crear el string JSON\n");
        json_decref(jsonArray);
        exit(1);
    }
    // Añadir el arreglo JSON al objeto raíz
    json_object_set_new(jsonRoot, "path remoto", jsonString0);

    // Convertir el objeto JSON en una cadena JSON formateada
    char *jsonString = json_dumps(jsonRoot, JSON_COMPACT);
    if (jsonString == NULL) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        json_decref(jsonRoot);
        exit(1);
    }

    return jsonString;

    // Liberar la memoria asignada para el objeto JSON y la cadena JSON
    json_decref(jsonRoot);
    json_decref(jsonArray);
    free(jsonString);
}

char *directorio() {
    char *directorio = getcwd(NULL, 0);
    if (directorio == NULL) {
        perror("Error al obtener el directorio actual");
        exit(1);
    }
    int tam_nuevo = strlen(directorio) + strlen("carpetita") + 1;
    directorio = (char *)realloc(directorio, tam_nuevo);
    strcat(directorio, "/");
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
    int len = 0;
    char *directorio;

    if (dir) {
        len = strlen(mensaje_ok);
        directorio = (char *)malloc((len + 1)*sizeof(char));
        strcpy(directorio, mensaje_ok);
        closedir(dir);

    } else {
        len = strlen(mensaje_error);
        directorio = (char *)malloc((len + 1)*sizeof(char));
        strcpy(directorio, mensaje_error);
    }

    // Crear un objeto JSON raíz
    json_t *jsonRoot = json_object();
    if (jsonRoot == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    // Crear un objeto JSON de tipo arreglo para almacenar los nombres de los archivos
    json_t *jsonArray = json_array();
    if (jsonArray == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON\n");
        json_decref(jsonRoot);
        exit(1);
    }

    // Crear un objeto JSON para almacenar la ruta del directorio
    json_t *jsonString0 = json_string(directorio);
    if (jsonString0 == NULL) {
        fprintf(stderr, "Error al crear el string JSON\n");
        json_decref(jsonArray);
        exit(1);
    }
    // Añadir el arreglo JSON al objeto raíz
    json_object_set_new(jsonRoot, "directorio", jsonString0);

    // Convertir el objeto JSON en una cadena JSON formateada
    char *jsonString = json_dumps(jsonRoot, JSON_COMPACT);
    if (jsonString == NULL) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        json_decref(jsonRoot);
        exit(1);
    }

    return jsonString;

    // Liberar la memoria asignada para el objeto JSON y la cadena JSON
    json_decref(jsonRoot);
    json_decref(jsonArray);
    free(jsonString);
}

char *mandarFileFldr(char *mensaje, int typeFile) {
    // Crear un objeto JSON raíz
    json_t *jsonRoot = json_object();
    if (jsonRoot == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    // Crear un objeto JSON de tipo arreglo para almacenar los nombres de los archivos
    json_t *jsonArray = json_array();
    if (jsonArray == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON\n");
        json_decref(jsonRoot);
        exit(1);
    }

    // Crear un objeto JSON para almacenar la ruta del directorio
    json_t *jsonString0 = json_string(mensaje);
    if (jsonString0 == NULL) {
        fprintf(stderr, "Error al crear el string JSON\n");
        json_decref(jsonArray);
        exit(1);
    }
    // Añadir el arreglo JSON al objeto raíz
    json_object_set_new(jsonRoot, "mensaje", jsonString0);

    if (typeFile == 0) {
        json_t *jsonString1 = json_string("file");
        if (jsonString1 == NULL) {
            fprintf(stderr, "Error al crear el string JSON\n");
            json_decref(jsonArray);
            exit(1);
        }
        // Añadir el arreglo JSON al objeto raíz
        json_object_set_new(jsonRoot, "typeFile", jsonString1);
    } else {
        json_t *jsonString1 = json_string("fldr");
        if (jsonString1 == NULL) {
            fprintf(stderr, "Error al crear el string JSON\n");
            json_decref(jsonArray);
            exit(1);
        }
        // Añadir el arreglo JSON al objeto raíz
        json_object_set_new(jsonRoot, "typeFile", jsonString1);
    }

    // Convertir el objeto JSON en una cadena JSON formateada
    char *jsonString = json_dumps(jsonRoot, JSON_COMPACT);
    if (jsonString == NULL) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        json_decref(jsonRoot);
        exit(1);
    }

    return jsonString;

    // Liberar la memoria asignada para el objeto JSON y la cadena JSON
    json_decref(jsonRoot);
    json_decref(jsonArray);
    free(jsonString);
}

char *recibido(char *mensaje) {
    // Crear un objeto JSON raíz
    json_t *jsonRoot = json_object();
    if (jsonRoot == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON raíz\n");
        exit(1);
    }

    // Crear un objeto JSON de tipo arreglo para almacenar los nombres de los archivos
    json_t *jsonArray = json_array();
    if (jsonArray == NULL) {
        fprintf(stderr, "Error al crear el objeto JSON\n");
        json_decref(jsonRoot);
        exit(1);
    }

    // Crear un objeto JSON para almacenar la ruta del directorio
    json_t *jsonString0 = json_string(mensaje);
    if (jsonString0 == NULL) {
        fprintf(stderr, "Error al crear el string JSON\n");
        json_decref(jsonArray);
        exit(1);
    }
    // Añadir el arreglo JSON al objeto raíz
    json_object_set_new(jsonRoot, "mensaje", jsonString0);

    // Convertir el objeto JSON en una cadena JSON formateada
    char *jsonString = json_dumps(jsonRoot, JSON_COMPACT);
    if (jsonString == NULL) {
        fprintf(stderr, "Error al convertir el objeto JSON raíz a cadena\n");
        json_decref(jsonRoot);
        exit(1);
    }

    return jsonString;

    // Liberar la memoria asignada para el objeto JSON y la cadena JSON
    json_decref(jsonRoot);
    json_decref(jsonArray);
    free(jsonString);
}

char *mkdirCarpeta(char *ruta, char *carpeta) {
    int tam = strlen(ruta) + 1 + strlen(carpeta) + 1;
    char *rutita = (char *)malloc(tam*sizeof(char));
    strcpy(rutita, ruta);
    strcat(rutita, "/");
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

    char *mensaje;

    int tam = strlen(ruta) + 1 + strlen(archivo) + 1;
    char *rutita = (char *)malloc(tam*sizeof(char));
    strcpy(rutita, ruta);
    strcat(rutita, "/");
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
        mensaje = (char *)malloc(tam*sizeof(char));
        strcpy(mensaje, "No existe: ");
        strcat(mensaje, rutita);
        printf("mensaje: %s\n", mensaje);
    }

    char *ret = recibido(mensaje);
    return ret;
}

char *rutaAbsoluta(char *ruta, char *nombre) {
    int tam = strlen(ruta) + 1 + strlen(nombre) + 1;
    char *rutita = (char *)malloc(tam*sizeof(char));
    strcpy(rutita, ruta);
    strcat(rutita, "/");
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

    json_error_t error;
    json_t *root = json_loads(msj, 0, &error);

    // Verificar que la cadena sea u njson sin errores

    if (!root) {
        fprintf(stderr, "error: en la linea %d: %s\n", error.line, error.text);
        exit(1);
    }

    // Verificar que la raíz sea un objeto
    if (!json_is_object(root)) {
        fprintf(stderr, "error: raíz no es un objeto\n");
        json_decref(root);
        exit(1);
    }

    printf("soy valido\n");
}
