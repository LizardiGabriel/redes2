Almacenamiento remoto de archivos

Implementar una aplicación cliente - servidor que permita al usuario 

1. Listar el contenido de las carpetas almacenadas local o remotamente (list)
2. Crear carpetas Local o remotamente (mkdir)
3. Eliminar carpetas/archivos local o remotamente (rmdir)
4. Cambiar la ruta de directorio localo remoto (cd)
5. Enviar archivos o carpetas desde una carpeta local (cliente) hacia una carpeta remota (servidor) (put)
6. Enviar archivos o carpetas desde una carpeta remota -servidor- hacia una carpeta local -cliente- (get)
7. Salir de la aplicación (quit)




Ejecutar el servidor en C (Jansson.h y zlib.h)
Ejecutar el cliente en java (org.json.jar)



// instalar Jansson.h -> descargar la libreria para el servidor

tar -xvzf jansson-2.13.tar.gz
cd jansson-2.13

mkdir build
cd build
cmake 

make ..
make check
make install

// instalar zlib (en ubuntu) -> descargar libreria para el servidor


// importar libreria org json para el cliente



Cambiar la ip en cliente/src/Conexion.java

En cliente y servidor se usan los puertos 1234, 1235, 1236


//Para compilar el servidor 
gcc -o prueba servidor.c -ljansson

