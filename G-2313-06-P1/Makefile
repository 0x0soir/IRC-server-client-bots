#Makefile Jorge y Javi
GROUP=G-2313-06-P1

#Ficheros para la realizacion de la compilacion
SRCDIR = src/$(GROUP)_server.c
SRCDIR += includes/$(GROUP)_server.h
SRCDIR += src/$(GROUP)_common_functions.c
SRCDIR += includes/$(GROUP)_common_functions.h
SRCDIR += src/$(GROUP)_function_handlers.c
SRCDIR += includes/$(GROUP)_function_handlers.h
SRCDIR += src/$(GROUP)_thread_pool.c
SRCDIR += includes/$(GROUP)_list.h
SRCDIR += includes/$(GROUP)_thread_pool.h

#Flags para el compilador
FFLAGS= -Wall -pedantic -pthread -w
REDESLIB= -lircredes -lirctad -lircinterface -lsoundredes

#Comienzo del makefile
#Regla All: compila todo el programa

all: clean tar server

server:
	@echo "\033[31mCompilando ejecutable...\033[00m"
	gcc -o ./obj/$(GROUP) $(SRCDIR) $(FFLAGS) $(REDESLIB)
	@echo "\033[32mEjecutable compilado: $(GROUP)\033[00m"

#Regla clean: limpia el directorio del ficheros temporales
clean:
	@echo "Borrando ficheros temporales"
	rm -rf obj/*.o $(GROUP)
	rm -rf objlib/*.o $(GROUP)
	rm -rf $(GROUP)
tar:
	@echo "\033[31mComprimiendo...\033[00m"
	tar --exclude .git -czf $(GROUP).tar.gz ../$(GROUP) --exclude $(GROUP).tar.gz
	@echo "\033[32mFichero comprimido: $(GROUP).tar.gz\033[00m"
