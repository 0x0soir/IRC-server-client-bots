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
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <ifaddrs.h>

#include "G-2313-06-P2_tcp.h"
#include "G-2313-06-P2_client_common_functions.h"
#include "G-2313-06-P2_client_function_handlers.h"

void* client_function_ping(void *arg);
void* client_function_response(void *arg);
void client_pre_in_function(char* command);
void client_execute_in_function(long functionName, char* command);
void client_pre_out_function(char* command);
void client_execute_out_function(long functionName, char* command);

typedef struct {
	char *nick;
	char *filename;
	char *data;
	long unsigned length;
}srecv;
