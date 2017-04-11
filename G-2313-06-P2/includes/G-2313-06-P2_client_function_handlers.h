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

void server_in_command_nick(char* command, int desc, char* nick_static, int* register_status);
void server_in_command_pong(char* command, int desc, char* nick_static, int* register_status);
void server_out_command_nick(char* command, int desc, char* nick_static, int* register_status);
