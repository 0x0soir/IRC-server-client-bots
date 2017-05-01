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

#include "G-2313-06-P2_tcp.h"

void *server_especial_ficheros(void *vmsg);
