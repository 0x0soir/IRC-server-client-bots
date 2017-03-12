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

/* Functions */
int server_users_find_by_nick(char* data);
int server_users_find_by_socket(int sockdesc);
int server_channels_find_by_name(char *channel);
int server_channels_update_nick(char *nick_old, char* nick_new);
int server_channels_update_away(char* nick, char* away);
int server_channels_update_whois(char* nick, char* away);
char* server_return_user(char* nick);
