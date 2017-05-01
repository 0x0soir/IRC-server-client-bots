# Makefile Jorge y Javi
GROUP_CLIENT=G-2313-06-P2

# Flags para el compilador
FFLAGS= -Wall -pedantic -pthread -rdynamic
REDESLIB= -lircredes -lirctad -lircinterface -lsoundredes

# Cliente
GTK_CONFIG = `pkg-config --cflags gtk+-3.0`
GTK_LIB = `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`
SRCDIR_CLIENT = src/$(GROUP_CLIENT)_client.c
SRCDIR_CLIENT += includes/$(GROUP_CLIENT)_client.h
SRCDIR_CLIENT += srclib/$(GROUP_CLIENT)_tcp.c
SRCDIR_CLIENT += includes/$(GROUP_CLIENT)_tcp.h
SRCDIR_CLIENT += srclib/$(GROUP_CLIENT)_files.c
SRCDIR_CLIENT += includes/$(GROUP_CLIENT)_files.h
SRCDIR_CLIENT += src/$(GROUP_CLIENT)_client_common_functions.c
SRCDIR_CLIENT += includes/$(GROUP_CLIENT)_client_common_functions.h
SRCDIR_CLIENT += src/$(GROUP_CLIENT)_client_function_handlers.c
SRCDIR_CLIENT += includes/$(GROUP_CLIENT)_client_function_handlers.h
SRCDIR_CLIENT += src/$(GROUP_CLIENT)_client_err_handlers.c
SRCDIR_CLIENT += includes/$(GROUP_CLIENT)_client_err_handlers.h

#Comienzo del makefile
#Regla All: compila todo el programa

all: clean tar client

client:
	@echo "\033[31mCompilando cliente...\033[00m"
	gcc $(GTK_CONFIG) -o ./obj/$(GROUP_CLIENT)_client $(SRCDIR_CLIENT) $(FFLAGS) $(REDESLIB) $(GTK_LIB)
	@echo "\033[32m--> Cliente compilado: $(GROUP_CLIENT)_client\033[00m"

#Regla clean: limpia el directorio del ficheros temporales
clean:
	@echo "Borrando ficheros temporales"
	rm -rf obj/*.o $(GROUP_CLIENT)
	rm -rf objlib/*.o $(GROUP_CLIENT)
	rm -rf $(GROUP_CLIENT)_client
tar:
	@echo "\033[31mComprimiendo...\033[00m"
	tar --exclude .git -czf $(GROUP_CLIENT).tar.gz ../$(GROUP_CLIENT) --exclude $(GROUP_CLIENT).tar.gz
	@echo "\033[32mFichero comprimido: $(GROUP).tar.gz\033[00m"
