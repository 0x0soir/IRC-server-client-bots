#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <syslog.h>

#include "../includes/G-2313-06-P3_tcp.h"

void ssl_start_client();
void ssl_start_server();
SSL_CTX* inicializar_nivel_SSL(int *desc);
int fijar_contexto_SSL(SSL_CTX* ssl_ctx, const char* CAfile, const char* prvKeyFile,const char* certFile);
SSL* aceptar_canal_seguro_SSL(SSL_CTX* ctx_ssl, int desc, int puerto, int tam, struct sockaddr_in ip4addr);
int evaluar_post_connectar_SSL(SSL * ssl);
int enviar_datos_SSL(SSL * ssl, const char *buf);
int recibir_datos_SSL(SSL * ssl, char *buf);
void cerrar_canal_SSL(SSL *ssl, SSL_CTX *ctl_ssl, int desc);
SSL* conectar_canal_seguro_SSL(SSL_CTX* ctx_ssl, int desc, struct sockaddr res);
