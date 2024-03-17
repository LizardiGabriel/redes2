
#ifndef SERVIDOR_LIST_H
#define SERVIDOR_LIST_H

#include "List.c"
//list0
char *directorio();

//

char *listarContenido(char *ruta);

char *directorioActual(char *ruta);


char *cambiarDirectorio(char *ruta);

char *recibido(char *mensaje);

char *opcJson(char *opc, char *mensaje);

char *mkdirCarpeta(char *ruta, char *carpeta);

char *rmdirAlgo(char *ruta, char *archivo);

char *rutaAbsoluta(char *ruta, char *nombre);

int existe(char *ruta);

int calcularTamanito(char *ruta);

// List 1
char *recorrerDirectorio(char *ruta);
void verificar(char *msj);
char *generarList1(char *ruta);

//





#endif //SERVIDOR_LIST_H
