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
#include <redes2/ircxchat.h>
#include <netdb.h>
#include <pthread.h>

#include "G-2313-06-P2_files.h"

void server_in_command_nick(char* command);
void server_in_command_pong(char* command);
void server_in_command_join(char* command);
void server_in_command_part(char* command);
void server_in_command_mode(char* command);
void server_in_command_topic(char* command);
void server_in_command_kick(char* command);
void server_in_command_who(char* command, int desc, char * nick_static, int* register_status);
void server_in_command_privmsg(char* command);
void server_in_command_rpl_welcome(char* command);
void server_in_command_rpl_created(char* command);
void server_in_command_rpl_yourhost(char* command);
void server_in_command_rpl_luserclient(char* command);
void server_in_command_rpl_luserme(char* command);
void server_in_command_rpl_motdstart(char* command);
void server_in_command_rpl_motd(char* command);
void server_in_command_rpl_endofmotd(char* command);
void server_in_command_rpl_whoreply(char* command);

void server_out_command_nick(char* command);
void server_out_command_join(char* command);
void server_out_command_names(char* command);
void server_out_command_list(char* command);
void server_out_command_part(char* command);
void server_out_command_mode(char* command);
void server_out_command_kick(char* command);
void server_out_command_privmsg(char* command);
