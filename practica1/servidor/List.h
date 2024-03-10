
#ifndef SERVIDOR_LIST_H
#define SERVIDOR_LIST_H

#include "List.c"


char *listarContenido(char *ruta);

char *directorioActual(char *ruta);

char *directorio();

char *cambiarDirectorio(char *ruta);

char *recibido();

char *opcJson(char *opc, char *mensaje);


#endif //SERVIDOR_LIST_H
