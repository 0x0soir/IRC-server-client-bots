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

# Flags para el compilador
FFLAGS= -Wall -pedantic -pthread -w -rdynamic
REDESLIB= -lircredes -lirctad -lircinterface -lsoundredes
LIB_SSL = -lssl -lcrypto

#Comienzo del makefile
#Regla All: compila todo el programa

all: certificados claves

certificados:
		@echo "\033[31mGenerando certificados...\033[00m"
		@rm -R $(SRC_CERTIFICADOS)
		@mkdir -p $(SRC_CERTIFICADOS)
		@openssl genrsa -out $(SRC_CERTIFICADOS)/rootkey.pem 2048 > /dev/null 2>&1
		@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/rootkey.pem\033[00m"
		@openssl req -new -x509 -key $(SRC_CERTIFICADOS)/rootkey.pem -out $(SRC_CERTIFICADOS)/rootcert.pem -subj "/C=ES/ST=Madrid/L=Madrid/O=UAM/CN=Redes2 CA"
		@echo "\033[33m\tCertificado generado: $(SRC_CERTIFICADOS)/rootcert.pem\033[00m"
		@cat $(SRC_CERTIFICADOS)/rootkey.pem $(SRC_CERTIFICADOS)/rootcert.pem > $(SRC_CERTIFICADOS)/ca.pem
		@echo "\033[32m--> Certificados generados en: $(SRC_CERTIFICADOS)\033[00m"

claves:
	@echo "\033[31mGenerando claves del cliente...\033[00m"
	@openssl req -nodes -newkey rsa:2048 -keyout $(SRC_CERTIFICADOS)/clientkey.pem -out $(SRC_CERTIFICADOS)/clientreq.pem -subj "/C=ES/ST=Madrid/L=Madrid/O=UAM/CN=G-2313-06-P3-client" > /dev/null 2>&1
	@openssl x509 -req -in $(SRC_CERTIFICADOS)/clientreq.pem  -CA $(SRC_CERTIFICADOS)/ca.pem -CAkey $(SRC_CERTIFICADOS)/ca.pem -CAcreateserial -out $(SRC_CERTIFICADOS)/clientcert.pem  > /dev/null 2>&1
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
	@echo "\033[31mCompilando servidor echo...\033[00m"
	gcc -o ./echo/servidor_echo $(SRC_SERVER_ECHO) $(FFLAGS) $(REDESLIB) $(LIB_SSL)
	@echo "\033[32m--> Servidor compilado: ./echo/servidor_echo\033[00m"

client_echo:
	@echo "\033[31mCompilando cliente echo...\033[00m"
	gcc -o ./echo/cliente_echo $(SRC_CLIENT_ECHO) $(FFLAGS) $(REDESLIB) $(LIB_SSL)
	@echo "\033[32m--> Cliente compilado: ./echo/cliente_echo\033[00m"

#Regla clean: limpia el directorio del ficheros temporales
clean:
	@echo "Borrando ficheros temporales"
	rm -rf obj/*.o $(GROUP)
	rm -rf objlib/*.o $(GROUP)
	rm -rf $(GROUP)_server
tar:
	@echo "\033[31mComprimiendo...\033[00m"
	tar --exclude .git -czf $(GROUP).tar.gz ../$(GROUP) --exclude $(GROUP).tar.gz
	@echo "\033[32mFichero comprimido: $(GROUP).tar.gz\033[00m"
