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

#include "G-2313-06-P2_client_function_handlers.h"

#define CLIENT_MESSAGE_MAXSIZE  8096

void* client_function_ping(void *arg);
void* client_function_response(void *arg);
void client_pre_function(char* command);
void client_execute_function(long functionName, char* command);

/* Tipos */
typedef void (*FunctionCallBack)(char *, int, char *, int *);
