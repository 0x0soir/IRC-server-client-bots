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

#ifndef SPECIAL_HEADERS
#define SPECIAL_HEADERS
#include "G-2313-06-P2_client_function_handlers.h"
#include "G-2313-06-P2_client_err_handlers.h"
#endif

#ifndef CLIENT_MESSAGE_MAXSIZE
#define CLIENT_MESSAGE_MAXSIZE  16192
#endif

void* client_function_ping(void *arg);
void* client_function_response(void *arg);
void client_show_error(char* msg);
void client_pre_in_function(char* command);
void client_execute_in_function(long functionName, char* command);
void client_pre_out_function(char* command);
void client_execute_out_function(long functionName, char* command);

/* Tipos */
#ifndef COMMON_TYPES
#define COMMON_TYPES
typedef void (*FunctionCallBack)(char *);
#endif
