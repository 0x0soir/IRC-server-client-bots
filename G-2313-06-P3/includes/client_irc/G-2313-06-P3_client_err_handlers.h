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

#include "G-2313-06-P3_client_common_functions.h"

void server_in_command_err_cannotsendtochan(char* command);
void server_in_command_err_alreadyregistred(char* command);
void server_in_command_err_nonicknamegiven(char* command);
void server_in_command_err_erroneusnickname(char* command);
void server_in_command_err_nicknameinuse(char* command);
void server_in_command_err_nickcollision(char* command);
void server_in_command_err_unavailresource(char* command);
void server_in_command_err_restricted(char* command);
void server_in_command_err_passwdmismatch(char* command);
void server_in_command_err_bannedfromchan(char* command);
void server_in_command_err_channelisfull(char* command);
void server_in_command_err_chanoprivsneeded(char* command);
void server_in_command_err_inviteonlychan(char* command);
void server_in_command_err_nochanmodes(char* command);
void server_in_command_err_nosuchchannel(char* command);
void server_in_command_err_unknownmode(char* command);
void server_in_command_err_nomotd(char* command);
void server_in_command_err_nosuchnick(char* command);
