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

void server_command_function_nick(char* command, int desc, char* nick_static, int* register_status);
void server_command_function_user(char* command, int desc, char* nick_static, int* register_status);
void server_command_function_join(char* command, int desc, char* nick_static, int* register_status);
void server_command_function_quit(char* command, int desc, char* nick_static, int* register_status);
void server_command_function_ping(char* command, int desc, char* nick_static, int* register_status);
void server_command_function_list(char* command, int desc, char* nick_static, int* register_status);
void server_command_function_privmsg(char* command, int desc, char* nick_static, int* register_status);
void server_command_function_part(char* command, int desc, char* nick_static, int* register_status);
