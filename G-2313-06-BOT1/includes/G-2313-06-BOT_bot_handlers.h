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

void server_in_command_nick(char* command);
void server_in_command_join(char* command);
void server_in_command_privmsg(char* command);
void server_in_command_ping(char* command);
void server_in_command_pong(char* command);

void server_in_command_rpl_welcome(char* command);

void server_out_command_nick(char* command);
void server_out_command_join(char* command);
void server_out_command_names(char* command);
void server_out_command_list(char* command);
void server_out_command_part(char* command);
void server_out_command_privmsg(char* command);

void removeSubstring(char *s,const char *toremove);
void trim(char* str);
char* server_bot_text_test(char * msg);
