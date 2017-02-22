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

#define MAX_CONNECTIONS 100
#define SERVER_PORT 6667
#define CLIENT_MESSAGE_MAXSIZE 8096

int server_start(void);
void server_accept_connection(int socket_id);
void server_start_communication(int connval);
void server_exit(int handler);
void server_execute_function(long functionName, char* command, int desc);
int isClosedSocket(int val_read, char str[]);
void server_daemon();

/* Tipos */
typedef int (*FunctionCallBack)(char*, int);
