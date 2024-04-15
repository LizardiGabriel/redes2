#ifndef SERVIDOR_SOCKET3_H
#define SERVIDOR_SOCKET3_H

#include "Socket3.c"


int iniciarSocket3(int, char*, int tam, char *path, char *typeFile, int tamBuffer, int tamVentanaInt);

int iniciarSocket3UDP(int puerto, char *nombreArchivo, int tam, char *path, char *typeFile, int tamBuffer, int tamVentanaInt);


#endif 