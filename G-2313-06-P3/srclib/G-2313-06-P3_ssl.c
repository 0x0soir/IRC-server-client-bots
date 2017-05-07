#include <stdio.h>
#include "../includes/G-2313-06-P3_ssl.h"

/*! @mainpage P3- Seguridad SSL
*
* En esta página se encuentra disponible la documentación de librería SSL
* de la práctica 3 de Redes de Comunicaciones II, grupo 2313, pareja 6.
*
* <h1>Librería SSL</h1>
* La librería SSL proporciona una capa superior a las funciones de los servidores
* echo proporcionados para la práctica. En ella se encuentran las funciones de
* fijación, conexión, envío y recepción mediante la capa SSL de socket.
* <ul>
*	<li>@subpage inicializar_nivel_SSL</li>
* <li>@subpage fijar_contexto_SSL</li>
* <li>@subpage aceptar_canal_seguro_SSL</li>
* <li>@subpage evaluar_post_connectar_SSL</li>
* <li>@subpage enviar_datos_SSL</li>
* <li>@subpage recibir_datos_SSL</li>
* <li>@subpage cerrar_canal_SSL</li>
* <li>@subpage conectar_canal_seguro_SSL</li>
* </ul>
* \dotfile G-2313-06-P3_ssl.h
*/

void ssl_start_client(){
	int desc, whileStatus = 1, k = 0;
	SSL_CTX* ctx=NULL;
	SSL * ssl=NULL;
	struct addrinfo hints, *res;
	char ssl_cert_1[100];
	char ssl_cert_2[100];
	char message[2048];
	char buffer[8096];

	strcpy(ssl_cert_1, "./certs/cliente.pem");
	strcpy(ssl_cert_2, "./certs/ca.pem");

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	/*printf("inicializar_nivel_SSL\n");*/

	fflush(stdout);
	ctx = inicializar_nivel_SSL(&desc);
	/*printf("inicializar_nivel_SSL OK\n");*/

	if(ctx==NULL){
		return;
	}
	/*printf("fijar_contexto_SSL\n");*/
	fijar_contexto_SSL(ctx, ssl_cert_1, ssl_cert_2);
	/*printf("fijar_contexto_SSL OK\n");*/

	ERR_print_errors_fp(stdout);
	/*printf("getaddrinfo\n");*/

	if(getaddrinfo("localhost", "8080", &hints, &res)!=0){
		return;
	}
	/*printf("getaddrinfo ok\n");*/

	/*printf("conectar_canal_seguro_SSL\n");*/

	ssl=conectar_canal_seguro_SSL(ctx, desc, *(res->ai_addr));
	ERR_print_errors_fp(stdout);
	/*printf("conectar_canal_seguro_SSL ok\n");*/
	if(!evaluar_post_connectar_SSL(ssl)){
		ERR_print_errors_fp(stdout);
		return;
	}
	/*printf("evaluar_post_connectar_SSL ok\n");*/
	while(whileStatus){
		k++;
		bzero(message, sizeof(char)*8096);
		bzero(buffer, sizeof(char)*8096);
		fgets(message, 8096, stdin);
		/*strcat(message, "\n\0");*/
		syslog(LOG_INFO, "[CLIENTE ECHO] [%d] Envia %s", k, message);
		enviar_datos_SSL(ssl, message);
		recibir_datos_SSL(ssl, &buffer[0]);
		fflush(stdout);
		fprintf(stdout, "%s", buffer);
		syslog(LOG_INFO, "[CLIENTE ECHO] [%d] Recibe %s", k, buffer);
		fflush(stdout);
		if(strcmp(message,"exit")==0){
			whileStatus=0;
		}
	}

	cerrar_canal_SSL(ssl,ctx,desc);
	freeaddrinfo(res);
}

void ssl_start_server(){
	int desc, k = 0;
	SSL_CTX *ssl_ctx;
	SSL *ssl=NULL;
	struct sockaddr_in ip4addr;
	int state, whileStatus = 1;
	char ssl_cert_1[100];
	char ssl_cert_2[100];
	char ssl_cert_3[100];
	char buffer[8096];

	strcpy(ssl_cert_1, "./certs/servidor.pem");
	strcpy(ssl_cert_2, "./certs/ca.pem");

	ssl_ctx = inicializar_nivel_SSL(&desc);
	if(desc<=0){
		return;
	}
	ERR_print_errors_fp(stdout);
	fijar_contexto_SSL(ssl_ctx, ssl_cert_1, ssl_cert_2);
	ERR_print_errors_fp(stdout);


	aceptar_canal_seguro_SSL(ssl_ctx, &ssl, desc, 8080, 80, ip4addr);
	ERR_print_errors_fp(stdout);

	if(!evaluar_post_connectar_SSL(ssl)){
		ERR_print_errors_fp(stdout);
		return;
	}
	while(whileStatus){
		k++;
		bzero(buffer, sizeof(char)*8096);
		state = recibir_datos_SSL(ssl, &buffer[0]);
		if(state>0){
			fflush(stdout);
			/*for(i = (strlen(buffer)-1); buffer[i] != '\n'; i--){
				buffer[i+1] = '\0';
			}*/
			printf("%s", buffer);
			syslog(LOG_INFO, "[SERVIDOR ECHO] [%d] Recibe %s", k, buffer);
			fflush(stdout);
			if(state > 0){
				fflush(stdout);
				enviar_datos_SSL(ssl, buffer);
				syslog(LOG_INFO, "[SERVIDOR ECHO] [%d] Envia %s", k, buffer);
			}
			if(strcmp(buffer,"exit")==0){
				whileStatus=0;
			}
		}
	}
	cerrar_canal_SSL(ssl, ssl_ctx, desc);
}


/**
* @page inicializar_nivel_SSL Inicialización de SSL
* @section synopsis_1 Synopsis
* <code>
* \b #include \b <G-2313-06-P3_ssl.h>
* <br>
* <b>SSL_CTX* inicializar_nivel_SSL(int *desc);</b>
* </code>
* @section descripcion_1 Descripción
*
* Inicializa el socket principal del servidor mediante una capa SSL,
* para ello se generan varias llamadas de comprobación y verificación
* de las características de SSL.
* <br>Recibe como parámetro un puntero a un entero donde se almacenará
* el identificador del descriptor (socket).
* <br><h2><b>Funciones de librería SSL:</b></h2>
* <ul>
* <li>SSL_load_error_strings();</li>
* <li>SSL_library_init();</li>
* </ul>

* @section return_1 Valores devueltos
* <ul>
* <li><b>SSL_CTX*</b> Contexto de la capa SSL.</li>
* </ul>
*
* @section authors_1 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
SSL_CTX* inicializar_nivel_SSL(int *desc){
	SSL_CTX* response;

	SSL_load_error_strings();
	SSL_library_init();

	response = SSL_CTX_new(SSLv23_method());
	ERR_print_errors_fp(stdout);

	*desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return response;
}

/**
* @page aceptar_canal_seguro_SSL Aceptación de un canal seguro
* @section synopsis_2 Synopsis
* <code>
* \b #include \b <G-2313-06-P3_ssl.h>
* <br>
* <b>int aceptar_canal_seguro_SSL(SSL_CTX* ctx_ssl, SSL **ssl, int desc, int puerto, int tam, struct sockaddr_in ip4addr);</b>
* </code>
* @section descripcion_2 Descripción
*
* Acepta una conexión recibida mediante la capa SSL, se ejecuta desde el
* lado del servidor. Mediante esta función el servidor se queda bloqueado
* esperando una nueva conexión a la que atender.
* <br>Recibe como parámetros los argumentos necesarios para la iteracción
* de la función con la capa SSL: contexto (SSL_CTX), SSL, descriptor del
* socket, puerto de conexión, tamaño de peticiones máxima a atender y
* la estructura de conexión IP4.
* <br><h2><b>Funciones de librería SSL:</b></h2>
* <ul>
* <li>SSL_new();</li>
* <li>SSL_set_fd();</li>
* <li>SSL_accept();</li>
* </ul>

* @section return_2 Valores devueltos
* <ul>
* <li><b>int</b> Valor entero que representa una aceptación correcta/incorrecta</li>
* </ul>
*
* @section authors_2 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
int aceptar_canal_seguro_SSL(SSL_CTX* ctx_ssl, SSL **ssl, int desc, int puerto, int tam, struct sockaddr_in ip4addr){
	int sockclient=-1;
	syslog(LOG_ERR, "SSL BIND en desc: %d %d", desc, puerto);
	tcp_bind(desc,puerto);
	syslog(LOG_ERR, "SSL LISTEN");
	tcp_new_listen(desc,tam);
	syslog(LOG_ERR, "SSL ACCEPT");
	sockclient=tcp_new_accept(desc, ip4addr);
	syslog(LOG_ERR, "SSL ACCEPTADO");
	*ssl=SSL_new(ctx_ssl);
	if(ssl==NULL){
		syslog(LOG_ERR, "Error en SSL null");
		return 0;
	}
	ERR_print_errors_fp(stdout);

	if(!SSL_set_fd(*ssl, sockclient))
		return 0;
	ERR_print_errors_fp(stdout);
	syslog(LOG_ERR, "SSL PRE RETURN con sockclient %d", sockclient);
	return SSL_accept(*ssl);
}

/**
* @page evaluar_post_connectar_SSL Comprobación de conexión SSL
* @section synopsis_3 Synopsis
* <code>
* \b #include \b <G-2313-06-P3_ssl.h>
* <br>
* <b>int evaluar_post_connectar_SSL(SSL * ssl);</b>
* </code>
* @section descripcion_3 Descripción
*
* Comprueba una conexión SSL recibida como parámetro, para ello verifica
* que los certificados utilizados en dicha conexión son válidos, así como
* el propio resultado de la estabilidad de la conexión SSL.
* <br><h2><b>Funciones de librería SSL:</b></h2>
* <ul>
* <li>SSL_get_peer_certificate();</li>
* <li>SSL_get_verify_result();</li>
* </ul>

* @section return_3 Valores devueltos
* <ul>
* <li><b>int</b> Valor entero que representa una aceptación correcta/incorrecta</li>
* </ul>
*
* @section authors_3 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
int evaluar_post_connectar_SSL(SSL * ssl){
	if(SSL_get_peer_certificate(ssl)==NULL){
		return -1;
	}
	if(SSL_get_verify_result(ssl)!=X509_V_OK){
		return -1;
	}
	return 1;
}

/**
* @page enviar_datos_SSL Envío de datos vía SSL
* @section synopsis_4 Synopsis
* <code>
* \b #include \b <G-2313-06-P3_ssl.h>
* <br>
* <b>int enviar_datos_SSL(SSL *ssl, const char *buf);</b>
* </code>
* @section descripcion_4 Descripción
*
* Envía un buffer de datos recibidos como parámetros mediante
* la conexión SSL recibida también como parámetro.
* <br><h2><b>Funciones de librería SSL:</b></h2>
* <ul>
* <li>SSL_write();</li>
* </ul>

* @section return_4 Valores devueltos
* <ul>
* <li><b>int</b> Valor entero que representa una aceptación correcta/incorrecta</li>
* </ul>
*
* @section authors_4 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
int enviar_datos_SSL(SSL *ssl, const char *buf){
	return SSL_write(ssl, buf, strlen(buf));
}

/**
* @page recibir_datos_SSL Recepción de datos vía SSL
* @section synopsis_5 Synopsis
* <code>
* \b #include \b <G-2313-06-P3_ssl.h>
* <br>
* <b>int recibir_datos_SSL(SSL * ssl, char *buf);</b>
* </code>
* @section descripcion_5 Descripción
*
* Recibe un buffer de datos recibidos como parámetros mediante
* la conexión SSL recibida también como parámetro.
* <br><h2><b>Funciones de librería SSL:</b></h2>
* <ul>
* <li>SSL_read();</li>
* </ul>

* @section return_5 Valores devueltos
* <ul>
* <li><b>int</b> Valor entero que representa una aceptación correcta/incorrecta</li>
* </ul>
*
* @section authors_5 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
int recibir_datos_SSL(SSL * ssl, char *buf){
	int size = 8096;
	return SSL_read(ssl, buf, size);
}

/**
* @page fijar_contexto_SSL Fijación del contexto SSL
* @section synopsis_6 Synopsis
* <code>
* \b #include \b <G-2313-06-P3_ssl.h>
* <br>
* <b>int fijar_contexto_SSL(SSL_CTX* ssl_ctx, char * certFile, char * certRoot);</b>
* </code>
* @section descripcion_6 Descripción
*
* Fija la conexión SSL comprobando que los certificados utilizados en
* dicha conexión están formados correctamente y tienen el formato adecuado
* de SSL.
* <br><h2><b>Funciones de librería SSL:</b></h2>
* <ul>
* <li>SSL_CTX_load_verify_locations(ssl_ctx, certRoot, certFile);</li>
* <li>SSL_CTX_set_default_verify_paths(ssl_ctx);</li>
* <li>SSL_CTX_use_certificate_file(ssl_ctx, certFile, SSL_FILETYPE_PEM);</li>
* <li>SSL_CTX_use_PrivateKey_file(ssl_ctx, certFile, SSL_FILETYPE_PEM;</li>
* <li>SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);</li>
* </ul>

* @section return_6 Valores devueltos
* <ul>
* <li><b>int</b> Valor entero que representa una aceptación correcta/incorrecta</li>
* </ul>
*
* @section authors_6 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
int fijar_contexto_SSL(SSL_CTX* ssl_ctx, char * certFile, char * certRoot){
	if (!SSL_CTX_load_verify_locations(ssl_ctx, certRoot, certFile)){
		ERR_print_errors_fp(stdout);
		syslog (LOG_INFO, "Error al verificar los paths");
		return 0;
	}
	SSL_CTX_set_default_verify_paths(ssl_ctx);

	if(!SSL_CTX_use_certificate_file(ssl_ctx, certFile, SSL_FILETYPE_PEM)){
		ERR_print_errors_fp(stdout);
		syslog (LOG_INFO, "Error al usar chain file");
		return 0;
	}
	SSL_CTX_use_PrivateKey_file(ssl_ctx, certFile, SSL_FILETYPE_PEM);
	SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);
	return 1;
}

/**
* @page cerrar_canal_SSL Cierre de un canal SSL
* @section synopsis_7 Synopsis
* <code>
* \b #include \b <G-2313-06-P3_ssl.h>
* <br>
* <b>void cerrar_canal_SSL(SSL *ssl, SSL_CTX *ctl_ssl, int desc);</b>
* </code>
* @section descripcion_7 Descripción
*
* Finaliza un canal de comunicación con un cliente vía SSL. Para ello
* libera los recursos de SSL recibidos y además realiza un apagado
* del descriptor (socket) utilizado por el cliente.
* <br><h2><b>Funciones de librería SSL:</b></h2>
* <ul>
* <li>SSL_shutdown(ssl);</li>
* <li>SSL_free(ssl);</li>
* <li>SSL_CTX_free(ctl_ssl);</li>
* </ul>

* @section return_7 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_7 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void cerrar_canal_SSL(SSL *ssl, SSL_CTX *ctl_ssl, int desc){
	int check;
	while(!(check=SSL_shutdown(ssl))){
		if(check==-1)
			return;
	}
	SSL_free(ssl);
	ssl= NULL;
	SSL_CTX_free(ctl_ssl);
	ctl_ssl=NULL;
	close(desc);
}

/**
* @page conectar_canal_seguro_SSL Conexión a un canal SSL
* @section synopsis_8 Synopsis
* <code>
* \b #include \b <G-2313-06-P3_ssl.h>
* <br>
* <b>SSL* conectar_canal_seguro_SSL(SSL_CTX* ctx_ssl, int desc, struct sockaddr res);</b>
* </code>
* @section descripcion_8 Descripción
*
* Inicia la comunicación con un canal seguro vía SSL. Para ello realiza
* una nueva conexión vía TCP y conecta la capa SSL a dicho socket generado.
* <br><h2><b>Funciones de librería SSL:</b></h2>
* <ul>
* <li>SSL_new(ctx_ssl);</li>
* <li>SSL_set_fd(ssl, desc);</li>
* <li>SSL_connect(ssl);</li>
* </ul>

* @section return_7 Valores devueltos
* <ul>
* <li><b>SSL*</b> Devuelve una conexión SSL al canal seguro </li>
* </ul>
*
* @section authors_7 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
SSL* conectar_canal_seguro_SSL(SSL_CTX* ctx_ssl, int desc, struct sockaddr res){
	SSL * ssl=NULL;
	tcp_new_open_connection(desc,res);
	/*printf("errno:%d\n",errno );*/
	ssl=SSL_new(ctx_ssl);

	if(ssl==NULL)
		return NULL;
	ERR_print_errors_fp(stdout);
	/*printf("SSL_set_fd\n");*/
	if(!SSL_set_fd(ssl, desc)){
		return NULL;
	}
	ERR_print_errors_fp(stdout);
	/*printf("SSL_connect\n");*/
	if(!SSL_connect(ssl)){
		return NULL;
	}
	return ssl;
}
