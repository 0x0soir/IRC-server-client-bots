#Makefile Jorge y Javi
#Nombre de ejecutable
EXE_NAME=06_redes2

#Ficheros para la realizacion de la compilacion
SOURCE_FILES = server.c server.h function_handlers.c function_handlers.h

#Flags para el compilador
FFLAGS= -Wall -pedantic -pthread
REDESLIB= -lircredes

#Comienzo del makefile
#Regla All: compila todo el programa
all:
	echo "Compilando ficheros"
	gcc -o $(EXE_NAME) $(SOURCE_FILES) $(FFLAGS) $(REDESLIB)

#Regla clean: limpia el directorio del ficheros temporales
clean:
	echo "Borrando ficheros temporales"
	rm -rf *.o $(EXE_NAME)
