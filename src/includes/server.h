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
#include "function_handlers.h"
#include "common_functions.h"

/* External libs */
#include "list.h"
#include "thread_pool.h"

#define MIN_POOL_THREADS        2
#define MAX_CONNECTIONS         100
#define SERVER_PORT             6667
#define CLIENT_MESSAGE_MAXSIZE  8096

int server_start(void);
void server_accept_connection(int socket_id);
void *server_start_communication(int connval);
void server_exit();
void server_execute_function(long functionName, char* command, int desc, char * nick, int * register_status);
int isClosedSocket(int val_read, char str[]);
void server_daemon();
void server_start_pool();

/* Tipos */
typedef int (*FunctionCallBack)(char *, int, char *, int *);
