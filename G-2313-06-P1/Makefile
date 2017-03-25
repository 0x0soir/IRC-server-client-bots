# Makefile Jorge y Javi
GROUP=G-2313-06-P1

# Ficheros para la realizacion de la compilacion
SRCDIR = src/$(GROUP)_server.c
SRCDIR += includes/$(GROUP)_server.h
SRCDIR += src/$(GROUP)_common_functions.c
SRCDIR += includes/$(GROUP)_common_functions.h
SRCDIR += src/$(GROUP)_function_handlers.c
SRCDIR += includes/$(GROUP)_function_handlers.h
SRCDIR += src/$(GROUP)_thread_pool.c
SRCDIR += includes/$(GROUP)_list.h
SRCDIR += includes/$(GROUP)_thread_pool.h

# Flags para el compilador
FFLAGS= -Wall -pedantic -pthread -w -rdynamic
REDESLIB= -lircredes -lirctad -lircinterface -lsoundredes

# Cliente
GTK_CONFIG = `pkg-config --cflags gtk+-3.0`
GTK_LIB = `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`
SRCDIR_CLIENT = src/$(GROUP)_client.c

#Comienzo del makefile
#Regla All: compila todo el programa

all: clean tar server client

server:
	@echo "\033[31mCompilando servidor...\033[00m"
	@gcc -o ./obj/$(GROUP)_server $(SRCDIR) $(FFLAGS) $(REDESLIB)
	@echo "\033[32m--> Servidor compilado: $(GROUP)_server\033[00m"

client:
	@echo "\033[31mCompilando cliente...\033[00m"
	gcc $(GTK_CONFIG) -o ./obj/$(GROUP)_client $(SRCDIR_CLIENT) $(FFLAGS) $(REDESLIB) $(GTK_LIB)
	@echo "\033[32m--> Cliente compilado: $(GROUP)_client\033[00m"

#Regla clean: limpia el directorio del ficheros temporales
clean:
	@echo "Borrando ficheros temporales"
	rm -rf obj/*.o $(GROUP)
	rm -rf objlib/*.o $(GROUP)
	rm -rf $(GROUP)_server
	rm -rf $(GROUP)_client
tar:
	@echo "\033[31mComprimiendo...\033[00m"
	tar --exclude .git -czf $(GROUP).tar.gz ../$(GROUP) --exclude $(GROUP).tar.gz
	@echo "\033[32mFichero comprimido: $(GROUP).tar.gz\033[00m"
