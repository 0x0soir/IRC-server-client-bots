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
#include <redes2/ircxchat.h>

#include "../G-2313-06-P3_tcp.h"

#ifndef A_FWD_H
#define A_FWD_H
typedef struct {
	char *nick;
	char *filename;
	char *data;
	long unsigned length;
}srecv;
#endif

void *server_especial_enviar_ficheros(void *vrecv);
void *server_especial_recibir_ficheros(void *vmsg);
