#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <arpa/inet.h>
#include <redes2/irc.h>
#include <netdb.h>
#include <pthread.h>

/* Custom includes */
#include "G-2313-06-P3_function_handlers.h"
#include "G-2313-06-P3_common_functions.h"
#include "G-2313-06-P3_list.h"
#include "G-2313-06-P3_thread_pool.h"
#include "../G-2313-06-P3_ssl.h"

#define MIN_POOL_THREADS        3
#define MAX_CONNECTIONS         100
#define CLIENT_MESSAGE_MAXSIZE  8096

int server_start(void);
void server_accept_connection(int socket_id);
void *server_start_communication(SSL *ssl);
void server_exit();
void server_execute_function(long functionName, char* command, int desc, SSL *ssl, char * nick, int * register_status);
int server_check_socket_status(int socket_desc);
void server_daemon();
void server_start_pool();

/* Tipos */
typedef void (*FunctionCallBack)(char *, int desc, SSL *ssl, char *, int *);
