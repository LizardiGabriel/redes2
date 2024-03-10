#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <jansson.h>
#include <string.h>

char *listarContenido(){
    DIR *dir = opendir(".");
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
    char *jsonString = json_dumps(jsonRoot, JSON_INDENT(4));
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
int main() {
    char *jsonString = listarContenido();
    printf("%s\n", jsonString);
    free(jsonString);
    return 0;
}
