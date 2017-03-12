#Makefile Jorge y Javi
#Nombre de ejecutable

GROUP=G-2313-06-P1

#Ficheros para la realizacion de la compilacion
SRCDIR = src/server.c
SRCDIR += includes/server.h
SRCDIR += srclib/common_functions.c
SRCDIR += includes/common_functions.h
SRCDIR += srclib/function_handlers.c
SRCDIR += includes/function_handlers.h
SRCDIR += srclib/thread_pool.c
SRCDIR += includes/list.h
SRCDIR += includes/thread_pool.h

#Flags para el compilador
FFLAGS= -Wall -pedantic -pthread
REDESLIB= -lircredes -lirctad -lircinterface -lsoundredes

#Comienzo del makefile
#Regla All: compila todo el programa

all: server

server:
	@echo "\033[31mCompilando ejecutable...\033[00m"
	gcc -o ./obj/$(GROUP) $(SRCDIR) $(FFLAGS) $(REDESLIB)
	@echo "\033[32mEjecutable compilado: $(GROUP)\033[00m"

#Regla clean: limpia el directorio del ficheros temporales
clean:
	@echo "Borrando ficheros temporales"
	rm -rf *.o $(GROUP)

tar:
	@echo "\033[31mComprimiendo...\033[00m"
	tar --exclude .git -czvf $(GROUP).tar.gz ../$(GROUP) --exclude $(GROUP).tar.gz
	@echo "\033[32mFichero comprimido: $(GROUP)\033[00m"
