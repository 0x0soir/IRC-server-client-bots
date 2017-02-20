
#include "function_handlers.h"

#define MAX_CONNECTIONS 100
#define SERVER_PORT 6667
#define CLIENT_MESSAGE_MAXSIZE 8096

int server_start(void);
void server_accept_connection(int socket_id);
void server_start_communication(int connval);
void server_exit(int handler);
void server_execute_function(long i);
int isClosedSocket(int val_read, char str[]);
void server_daemon();

/* Tipos */
typedef int (*FunctionCallBack)(int);
