
#ifndef SERVIDOR_LIST_H
#define SERVIDOR_LIST_H

#include "List.c"


char *listarContenido(char *ruta);

char *directorioActual(char *ruta);

char *directorio();

char *cambiarDirectorio(char *ruta);

char *recibido(char *mensaje);

char *opcJson(char *opc, char *mensaje);

char *mkdirCarpeta(char *ruta, char *carpeta);

char *rmdirAlgo(char *ruta, char *archivo);

char *rutaAbsoluta(char *ruta, char *nombre);

int existe(char *ruta);

int calcularTamanito(char *ruta);



#endif //SERVIDOR_LIST_H
