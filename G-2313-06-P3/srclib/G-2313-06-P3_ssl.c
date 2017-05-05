#include <stdio.h>
#include "../includes/G-2313-06-P3_ssl.h"

void ssl_start_client(){
	int desc;
	SSL_CTX* ctx=NULL;
	SSL * ssl=NULL;
	struct addrinfo hints, *res;
	char buf [20];
	char ssl_cert_client[70];
	char ssl_cert_server[70];

	strcpy(ssl_cert_client, "./certs/ca.pem");
	strcpy(ssl_cert_server, "./certs/cliente.pem");

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	/*printf("inicializar_nivel_SSL\n");*/
	ctx = inicializar_nivel_SSL(&desc);
	/*printf("inicializar_nivel_SSL OK\n");*/
	ERR_print_errors_fp(stdout);
	if(ctx==NULL){
		return;
	}
	/*printf("fijar_contexto_SSL\n");*/
	fijar_contexto_SSL(ctx, ssl_cert_client, ssl_cert_server, ssl_cert_server);
	/*printf("fijar_contexto_SSL OK\n");*/

	ERR_print_errors_fp(stdout);
	/*printf("getaddrinfo\n");*/

	if(0!=getaddrinfo("localhost", "8080", &hints, &res)){
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
	/*printf("recibir\n");*/
	recibir_datos_SSL(ssl, buf);
	/*printf("recibido:[%s]\n",buf );*/
	enviar_datos_SSL(ssl, buf);
	ERR_print_errors_fp(stdout);
	cerrar_canal_SSL(ssl,ctx,desc);
	freeaddrinfo(res);
}

void ssl_start_server(){
	int desc;
	SSL_CTX *ssl_ctx;
	SSL *ssl=NULL;
	char buff[8096]="test";
	struct sockaddr_in ip4addr;

	char ssl_cert_client[70];
	char ssl_cert_server[70];

	strcpy(ssl_cert_client, "./certs/ca.pem");
	strcpy(ssl_cert_server, "./certs/cliente.pem");

	ssl_ctx = inicializar_nivel_SSL(&desc);
	ERR_print_errors_fp(stdout);
	fijar_contexto_SSL(ssl_ctx, ssl_cert_client, ssl_cert_server, ssl_cert_server);
	ERR_print_errors_fp(stdout);


	ssl=aceptar_canal_seguro_SSL(ssl_ctx, desc, 8080, 80, ip4addr);
	ERR_print_errors_fp(stdout);

	if(!evaluar_post_connectar_SSL(ssl)){
		ERR_print_errors_fp(stdout);
		return;
	}

	enviar_datos_SSL(ssl,buff);
	ERR_print_errors_fp(stdout);

	recibir_datos_SSL(ssl, buff);

	cerrar_canal_SSL(ssl,ssl_ctx,desc);
}

SSL_CTX* inicializar_nivel_SSL(int *desc){
	if(desc==NULL){
		return NULL;
	}
	*desc=tcp_new_socket();
	if(*desc==-1)
		return NULL;
	SSL_load_error_strings();
	SSL_library_init();
	/*ERR_print_errors_fp(stdout);*/

	return SSL_CTX_new(SSLv23_method());
}

SSL* aceptar_canal_seguro_SSL(SSL_CTX* ctx_ssl, int desc, int puerto, int tam, struct sockaddr_in ip4addr){
	SSL * ssl=NULL;
	int sockclient=-1;
	tcp_bind(desc,puerto);
	tcp_new_listen(desc,tam);
	sockclient=tcp_new_accept(desc, ip4addr);
	ssl=SSL_new(ctx_ssl);
	if(ssl==NULL)
		return NULL;
	ERR_print_errors_fp(stdout);

	if(!SSL_set_fd(ssl, sockclient))
		return NULL;
	ERR_print_errors_fp(stdout);

	if(!SSL_accept(ssl))
		return NULL;
	return ssl;
}

int evaluar_post_connectar_SSL(SSL * ssl){
	if(SSL_get_peer_certificate(ssl)==NULL){

		return 0;
	}
	/*printf("%ld\n", SSL_get_verify_result(ssl));*/
	/*printf("%d\n", X509_V_OK);*/
	if(SSL_get_verify_result(ssl)!=X509_V_OK){
		return 0;
	}
	return 1;
}

int enviar_datos_SSL(SSL * ssl,const void * buf){
	return SSL_write(ssl, buf, strlen(buf)+1);
}

int recibir_datos_SSL(SSL * ssl, void * buf){
	return SSL_read(ssl, buf, 8096);
}

int fijar_contexto_SSL(SSL_CTX* ssl_ctx, const char* CAfile, const char* prvKeyFile,const char* certFile){
	char CApath [1024];
	if (ssl_ctx==NULL)
		return 0;
	if(getcwd(CApath, sizeof(CApath))==NULL)
		return 0;
	strcat(CApath, "/cert");
	/*printf("CApath:%s, CAfile:%s, prvKeyFile:%s, certFile:%s\n",CApath, CAfile, prvKeyFile, certFile );*/
	if (!SSL_CTX_load_verify_locations(ssl_ctx,CAfile,CApath)){
		return 0;
	}
	SSL_CTX_set_default_verify_paths(ssl_ctx);
	ERR_print_errors_fp(stdout);
	if(!SSL_CTX_use_certificate_chain_file(ssl_ctx, certFile)){
		printf("ERROR SSL_CTX_use_certificate_chain_file\n");
		ERR_print_errors_fp(stdout);
		return 0;
	}

	SSL_CTX_use_PrivateKey_file(ssl_ctx, prvKeyFile, SSL_FILETYPE_PEM);
	ERR_print_errors_fp(stdout);

	SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

	ERR_print_errors_fp(stdout);
	return 1;
}

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

SSL* conectar_canal_seguro_SSL(SSL_CTX* ctx_ssl, int desc, struct sockaddr res){
	SSL * ssl=NULL;
	/*printf("connect:%d\n",tcp_new_open_connection(desc,res));*/
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
