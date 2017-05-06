#include <stdio.h>
#include "../includes/G-2313-06-P3_ssl.h"

void ssl_start_client(){
	int desc, whileStatus = 1, k = 0;
	SSL_CTX* ctx=NULL;
	SSL * ssl=NULL;
	struct addrinfo hints, *res;
	char ssl_cert_1[100];
	char ssl_cert_2[100];
	char ssl_cert_3[100];
	char message[2048];
	char buffer[8096];

	strcpy(ssl_cert_1, "./certs/ca.pem");
	strcpy(ssl_cert_2, "./certs/clientkey.pem");
	strcpy(ssl_cert_3, "./certs/clientcert.pem");

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
	fijar_contexto_SSL(ctx, ssl_cert_1, ssl_cert_2, ssl_cert_3);
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
		recibir_datos_SSL(ssl, &buffer);
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

	strcpy(ssl_cert_1, "./certs/ca.pem");
	strcpy(ssl_cert_2, "./certs/serverkey.pem");
	strcpy(ssl_cert_3, "./certs/servercert.pem");

	ssl_ctx = inicializar_nivel_SSL(&desc);
	if(desc<=0){
		return;
	}
	ERR_print_errors_fp(stdout);
	fijar_contexto_SSL(ssl_ctx, ssl_cert_1, ssl_cert_2, ssl_cert_3);
	ERR_print_errors_fp(stdout);


	ssl=aceptar_canal_seguro_SSL(ssl_ctx, desc, 8080, 80, ip4addr);
	ERR_print_errors_fp(stdout);

	if(!evaluar_post_connectar_SSL(ssl)){
		ERR_print_errors_fp(stdout);
		return;
	}
	while(whileStatus){
		k++;
		bzero(buffer, sizeof(char)*8096);
		state = recibir_datos_SSL(ssl, &buffer);
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

SSL_CTX* inicializar_nivel_SSL(int *desc){
	SSL_CTX* response;

	SSL_load_error_strings();
	SSL_library_init();

	response = SSL_CTX_new(SSLv23_method());
	ERR_print_errors_fp(stdout);

	*desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return response;
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

int enviar_datos_SSL(SSL *ssl, const char *buf){
	if(!ssl|!buf){
		printf("No existe ssl o buff\n");
		return -1;
	}
	return SSL_write(ssl, buf, strlen(buf));
}

int recibir_datos_SSL(SSL * ssl, char *buf){
	int size = 8096;
	if(ssl){
		/*if(*buf==NULL){
			*buf = (char *) malloc(size * sizeof(char));
		} else {
			free(*buf);
			*buf = (char *) malloc(size * sizeof(char));
		}*/
	}
	if(buf==NULL){
		return -1;
	}
	return SSL_read(ssl, buf, size);
}

int fijar_contexto_SSL(SSL_CTX* ssl_ctx, const char* CAfile, const char* prvKeyFile,const char* certFile){
	char CApath [1024];
	char buf[2048];

	SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);

	sprintf(CApath, "%s%s", getcwd(buf, sizeof(buf)), "/certs/");

	/*printf("CApath:%s, CAfile:%s, prvKeyFile:%s, certFile:%s\n",CApath, CAfile, prvKeyFile, certFile );*/
	if (!SSL_CTX_load_verify_locations(ssl_ctx, CAfile, CApath)){
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
