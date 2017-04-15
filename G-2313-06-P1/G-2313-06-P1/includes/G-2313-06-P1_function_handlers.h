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

#include "G-2313-06-P1_common_functions.h"

void server_command_nick(char* command, int desc, char* nick_static, int* register_status);
void server_command_user(char* command, int desc, char* nick_static, int* register_status);
void server_command_join(char* command, int desc, char* nick_static, int* register_status);
void server_command_quit(char* command, int desc, char* nick_static, int* register_status);
void server_command_ping(char* command, int desc, char* nick_static, int* register_status);
void server_command_list(char* command, int desc, char* nick_static, int* register_status);
void server_command_privmsg(char* command, int desc, char* nick_static, int* register_status);
void server_command_part(char* command, int desc, char* nick_static, int* register_status);
void server_command_names(char* command, int desc, char* nick_static, int* register_status);
void server_command_kick(char* command, int desc, char * nick_static, int* register_status);
void server_command_mode(char* command, int desc, char * nick_static, int* register_status);
void server_command_away(char* command, int desc, char * nick_static, int* register_status);
void server_command_whois(char* command, int desc, char * nick_static, int* register_status);
void server_command_topic(char* command, int desc, char * nick_static, int* register_status);
void server_command_motd(char* command, int desc, char * nick_static, int* register_status);
