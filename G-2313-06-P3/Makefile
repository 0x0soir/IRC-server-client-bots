# Makefile Jorge y Javi
GROUP=G-2313-06-P3
SRC_CERTIFICADOS=certs

# Ficheros para la realizacion de la compilacion
SRC_SERVER_ECHO = src/$(GROUP)_server_echo.c
SRC_SERVER_ECHO += srclib/$(GROUP)_tcp.c
SRC_SERVER_ECHO += includes/$(GROUP)_tcp.h
SRC_SERVER_ECHO += srclib/$(GROUP)_ssl.c
SRC_SERVER_ECHO += includes/$(GROUP)_ssl.h

SRC_CLIENT_ECHO = src/$(GROUP)_client_echo.c
SRC_CLIENT_ECHO += srclib/$(GROUP)_tcp.c
SRC_CLIENT_ECHO += includes/$(GROUP)_tcp.h
SRC_CLIENT_ECHO += srclib/$(GROUP)_ssl.c
SRC_CLIENT_ECHO += includes/$(GROUP)_ssl.h

SRC_SERVER_IRC = src/server_irc/$(GROUP)_server.c
SRC_SERVER_IRC += includes/server_irc/$(GROUP)_server.h
SRC_SERVER_IRC += src/server_irc/$(GROUP)_common_functions.c
SRC_SERVER_IRC += includes/server_irc/$(GROUP)_common_functions.h
SRC_SERVER_IRC += src/server_irc/$(GROUP)_function_handlers.c
SRC_SERVER_IRC += includes/server_irc/$(GROUP)_function_handlers.h
SRC_SERVER_IRC += src/server_irc/$(GROUP)_thread_pool.c
SRC_SERVER_IRC += includes/server_irc/$(GROUP)_list.h
SRC_SERVER_IRC += includes/server_irc/$(GROUP)_thread_pool.h
SRC_SERVER_IRC += srclib/$(GROUP)_ssl.c
SRC_SERVER_IRC += includes/$(GROUP)_ssl.h
SRC_SERVER_IRC += srclib/$(GROUP)_tcp.c
SRC_SERVER_IRC += includes/$(GROUP)_tcp.h

GTK_CONFIG = `pkg-config --cflags gtk+-3.0`
GTK_LIB = `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`
SRC_CLIENT_IRC = src/client_irc/$(GROUP)_client.c
SRC_CLIENT_IRC += includes/client_irc/$(GROUP)_client.h
SRC_CLIENT_IRC += srclib/$(GROUP)_tcp.c
SRC_CLIENT_IRC += includes/$(GROUP)_tcp.h
SRC_CLIENT_IRC += srclib/client_irc/$(GROUP)_files.c
SRC_CLIENT_IRC += includes/client_irc/$(GROUP)_files.h
SRC_CLIENT_IRC += src/client_irc/$(GROUP)_client_common_functions.c
SRC_CLIENT_IRC += includes/client_irc/$(GROUP)_client_common_functions.h
SRC_CLIENT_IRC += src/client_irc/$(GROUP)_client_function_handlers.c
SRC_CLIENT_IRC += includes/client_irc/$(GROUP)_client_function_handlers.h
SRC_CLIENT_IRC += src/client_irc/$(GROUP)_client_err_handlers.c
SRC_CLIENT_IRC += includes/client_irc/$(GROUP)_client_err_handlers.h
SRC_CLIENT_IRC += srclib/$(GROUP)_ssl.c
SRC_CLIENT_IRC += includes/$(GROUP)_ssl.h

# Flags para el compilador
FFLAGS= -Wall -w -pedantic -pthread -rdynamic
REDESLIB= -lircredes -lirctad -lircinterface -lsoundredes
LIB_SSL = -lssl -lcrypto
LIB = ./lib/
OBJ = ./obj/
SRCLIB = ./srclib/

all: clean certificados claves server_echo client_echo servidor_irc prelibreria libreria

certificados:
		@echo "\033[31mGenerando certificados...\033[00m"
		@rm -Rf $(SRC_CERTIFICADOS)
		@mkdir -p $(SRC_CERTIFICADOS)
		@openssl genrsa -out $(SRC_CERTIFICADOS)/rootkey.pem 2048 > /dev/null 2>&1
		@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/rootkey.pem\033[00m"
		@openssl req -new -x509 -key $(SRC_CERTIFICADOS)/rootkey.pem -out $(SRC_CERTIFICADOS)/rootcert.pem -subj "/C=ES/ST=Madrid/L=Madrid/O=UAM/CN=Redes2 CA"
		@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/rootcert.pem\033[00m"
		@cat $(SRC_CERTIFICADOS)/rootcert.pem $(SRC_CERTIFICADOS)/rootkey.pem > $(SRC_CERTIFICADOS)/ca.pem
		@echo "\033[32m--> Certificados generados en: $(SRC_CERTIFICADOS)\033[00m"

claves:
	@echo "\033[31mGenerando claves del cliente...\033[00m"
	@openssl req -nodes -newkey rsa:2048 -keyout $(SRC_CERTIFICADOS)/clientkey.pem -out $(SRC_CERTIFICADOS)/clientreq.pem -subj "/C=ES/ST=Madrid/L=Madrid/O=UAM/CN=G-2313-06-P3-client" > /dev/null 2>&1
	@openssl x509 -req -in $(SRC_CERTIFICADOS)/clientreq.pem -CA $(SRC_CERTIFICADOS)/ca.pem -CAkey $(SRC_CERTIFICADOS)/ca.pem -CAcreateserial -out $(SRC_CERTIFICADOS)/clientcert.pem  > /dev/null 2>&1
	@cat $(SRC_CERTIFICADOS)/clientcert.pem $(SRC_CERTIFICADOS)/clientkey.pem $(SRC_CERTIFICADOS)/rootcert.pem > $(SRC_CERTIFICADOS)/cliente.pem
	@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/clientcert.pem\033[00m"
	@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/clientkey.pem\033[00m"
	@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/cliente.pem\033[00m"
	@echo "\033[31mGenerando claves del servidor...\033[00m"
	@openssl req -nodes -newkey rsa:2048 -keyout $(SRC_CERTIFICADOS)/serverkey.pem -out $(SRC_CERTIFICADOS)/serverreq.pem -subj "/C=ES/ST=Madrid/L=Madrid/O=UAM/CN=G-2313-06-P3-server" > /dev/null 2>&1
	@openssl x509 -req -in $(SRC_CERTIFICADOS)/serverreq.pem -CA $(SRC_CERTIFICADOS)/ca.pem -CAkey $(SRC_CERTIFICADOS)/ca.pem -CAcreateserial -out $(SRC_CERTIFICADOS)/servercert.pem  > /dev/null 2>&1
	@cat $(SRC_CERTIFICADOS)/servercert.pem $(SRC_CERTIFICADOS)/serverkey.pem $(SRC_CERTIFICADOS)/rootcert.pem > $(SRC_CERTIFICADOS)/servidor.pem
	@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/servercert.pem\033[00m"
	@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/serverkey.pem\033[00m"
	@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/servidor.pem\033[00m"
	@echo "\033[32m--> Claves generadas en: $(SRC_CERTIFICADOS)\033[00m"

server_echo:
	@mkdir -p "echo"
	@echo "\033[31mCompilando servidor echo...\033[00m"
	@gcc -o ./echo/servidor_echo $(SRC_SERVER_ECHO) $(FFLAGS) $(REDESLIB) $(LIB_SSL)
	@echo "\033[32m--> Servidor compilado: ./echo/servidor_echo\033[00m"

client_echo:
	@mkdir -p "echo"
	@echo "\033[31mCompilando cliente echo...\033[00m"
	@gcc -o ./echo/cliente_echo $(SRC_CLIENT_ECHO) $(FFLAGS) $(REDESLIB) $(LIB_SSL)
	@echo "\033[32m--> Cliente compilado: ./echo/cliente_echo\033[00m"

servidor_irc:
	@mkdir -p "cliente_servidor"
	@echo "\033[31mCompilando servidor IRC...\033[00m"
	@gcc -o ./cliente_servidor/servidor_IRC $(SRC_SERVER_IRC) $(FFLAGS) $(REDESLIB) $(LIB_SSL)
	@echo "\033[32m--> Servidor compilado: ./cliente_servidor/servidor_IRC\033[00m"

cliente_irc:
	@echo "\033[31mCompilando cliente IRC...\033[00m"
	@gcc $(GTK_CONFIG) -o ./cliente_servidor/cliente_IRC $(SRC_CLIENT_IRC) $(FFLAGS) $(REDESLIB) $(GTK_LIB) $(LIB_SSL)
	@echo "\033[32m--> Cliente compilado: ./cliente_servidor/cliente_IRC\033[00m"

doxy:
	@echo "\033[31mGenerando documentación...\033[00m"
	@doxygen Doxyfile  > /dev/null 2>&1
	@echo "\033[32m--> Documentación generada en: ./doc/html/index.html\033[00m"

prelibreria:
	@echo "\033[31mGenerando librería...\033[00m"
	@mkdir -p "obj"
	@mkdir -p "lib"

libreria: $(LIB)$(GROUP)_ssl.a

$(LIB)$(GROUP)_ssl.a: $(OBJ)$(GROUP)_ssl.o
	@ar -rv $@ $^ -lssl > /dev/null 2>&1
	@echo "\033[32m--> Libreria generada en: ./lib/$(GROUP)_ssl.a\033[00m"

$(OBJ)$(GROUP)_ssl.o: $(SRCLIB)$(GROUP)_ssl.c
	@$(CC) $(CCFLAGS) -o $@ -c $^

entrega: clean tar

clean:
	@echo "\033[31mBorrando ficheros temporales...\033[00m"
	rm -rf obj/*.o $(GROUP)
	rm -rf objlib/*.o $(GROUP)
	rm -rf "echo"
	rm -rf "cliente_servidor"
	rm -rf lib
	rm -rf obj
	rm -rf certs
tar:
	@echo "\033[31mComprimiendo...\033[00m"
	tar --exclude .git -czf $(GROUP).tar.gz ../$(GROUP) --exclude $(GROUP).tar.gz
	@echo "\033[32mFichero comprimido: $(GROUP).tar.gz\033[00m"
