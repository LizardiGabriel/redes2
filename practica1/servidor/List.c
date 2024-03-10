//
// Created by lizardi on 09/03/24.
//

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <jansson.h>
#include <string.h>

#include "List.h"


char *listarContenido(char *ruta) {

    DIR *dir = opendir(ruta);

    char **archivos;
    char **carpetas;
    int num_elementos_arch;
    int num_elementos_car;

    // Verificar si se pudo abrir el directorio
    if (dir) {
        int max_elementos = 100;
        archivos = malloc(max_elementos * sizeof(char *));
        carpetas = malloc(max_elementos * sizeof(char *));

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
                carpetas[num_elementos_car] = malloc(strlen(entry->d_name) + 1);
                strcpy(carpetas[num_elementos_car], entry->d_name);
                num_elementos_car++;

            } else {
                // archivo
                archivos[num_elementos_arch] = malloc(strlen(entry->d_name) + 1);
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

char *directorio() {
    char *directorio = getcwd(NULL, 0);
    if (directorio == NULL) {
        perror("Error al obtener el directorio actual");
        exit(1);
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
        directorio = malloc(len + 1);
        strcpy(directorio, mensaje_ok);
        closedir(dir);

    } else {
        len = strlen(mensaje_error);
        directorio = malloc(len + 1);
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

char *recibido() {
    char *mensaje = "Mensaje recibido";

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

