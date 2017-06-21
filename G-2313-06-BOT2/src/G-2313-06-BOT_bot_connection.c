#include "../includes/G-2313-06-BOT_bot.h"

extern char *server;
extern int port;
extern char *channel;
extern char *nick;
extern int socket_desc;
extern int alive;
extern arraylist* channelsJoin;
extern arraylist* usersSpyTerms;
int status = 0;
pthread_t thread_response;

void run_siri(){
	if(connect_server()==1){
		syslog(LOG_INFO, "[BOT] Abrimos hilo de recepciones...");
		pthread_create(&thread_response, NULL, client_function_response, NULL);
		while (alive) {
			if(status==0){
				send_nick();
				send_user();
			}
		}
	}
}

void* client_function_response(void *arg){
  char buff[CLIENT_MESSAGE_MAXSIZE] = "";
  char* next;
  char* resultado;
  int valread;
	syslog(LOG_INFO, "[BOT] [!!] Dentro de hilo");
  while(TRUE) {
    syslog(LOG_INFO, "[BOT] Esperando a recibir mensajes...");
    valread = recv(socket_desc, buff, CLIENT_MESSAGE_MAXSIZE, 0);
    if(valread < 0) {
      pthread_exit(NULL);
    }

    syslog(LOG_INFO, "[BOT] Mensaje recibido: %s", buff);
    next = IRC_UnPipelineCommands(buff, &resultado);
    syslog(LOG_INFO, "[BOT] Procesa: %s", resultado);
    client_pre_in_function(resultado);
    free(resultado);
    while(next != NULL) {
      next = IRC_UnPipelineCommands(next, &resultado);
      if(resultado !=((void *) 0)) {
        syslog(LOG_INFO, "[BOT] Procesa: %s", resultado);
        client_pre_in_function(resultado);
      }
      free(resultado);
    }
    memset(buff,0,sizeof(buff));
  }
}

int connect_server(){
	struct sockaddr_in socket_address;
  struct hostent *server_info;
  syslog(LOG_INFO, "[BOT] Conectando al servidor... ");
  if(!nick||!server){
    return -1;
  } else {
    if((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
			syslog(LOG_INFO, "[BOT] [!!] Error al generar socket");
      return -1;
    } else {
			syslog(LOG_INFO, "[BOT] [!] Socket generado OK");
    }

    server_info = gethostbyname(server);
    if(server_info == NULL) {
      fprintf(stderr, ANSI_COLOR_RED "[!!] Error al obtener host\n" ANSI_COLOR_RESET);
			syslog(LOG_INFO, "[BOT] [!!] Error al obtener host");
      return -1;
    } else {
      fprintf(stderr, ANSI_COLOR_GREEN "[B] Host obtenido OK\n" ANSI_COLOR_RESET);
			syslog(LOG_INFO, "[BOT] [!] Host obtenido OK");
    }

    bzero((char *) &socket_address, sizeof (socket_address));
    socket_address.sin_family = AF_INET;
    bcopy((char *) server_info->h_addr, (char *) &socket_address.sin_addr.s_addr, server_info->h_length);
    socket_address.sin_port = htons(port);

    if(connect(socket_desc, (struct sockaddr*) &socket_address, sizeof (socket_address)) < 0) {
			fprintf(stderr, ANSI_COLOR_RED "[!!] Error conectando al servidor\n" ANSI_COLOR_RESET);
			syslog(LOG_INFO, "[BOT] [!!] Error conectando al servidor");
      return -1;
    } else {
			fprintf(stderr, ANSI_COLOR_GREEN "[B] Conexión con servidor OK\n" ANSI_COLOR_RESET);
    }
  }
	return 1;
}

void send_nick(){
	char *msgNick;
	IRCMsg_Nick(&msgNick, NULL, nick, NULL);
	if(send(socket_desc, msgNick, strlen(msgNick), 0) < 0) {
		fprintf(stderr, ANSI_COLOR_RED "[!!] Error enviando el mensaje: %s (%s)\n" ANSI_COLOR_RESET, msgNick, strerror(errno));
	} else {
		fprintf(stderr, ANSI_COLOR_GREEN "[B] Mensaje nick enviado OK\n" ANSI_COLOR_RESET);
		status = 1;
	}
}

void send_user(){
	char *msgUser;
	IRCMsg_User(&msgUser, NULL, "bot_espia", server, "bot_espia");
	if(send(socket_desc, msgUser, strlen(msgUser), 0) < 0) {
		fprintf(stderr, ANSI_COLOR_RED "[!!] Error enviando el mensaje: %s (%s)\n" ANSI_COLOR_RESET, msgUser, strerror(errno));
	} else {
		fprintf(stderr, ANSI_COLOR_GREEN "[B] Mensaje user enviado OK\n" ANSI_COLOR_RESET);
		status = 1;
	}
}

void send_join(char *channel){
	char *msgJoin;
	IRCMsg_Join(&msgJoin, NULL, channel, NULL, NULL);
	if(send(socket_desc, msgJoin, strlen(msgJoin), 0) < 0) {
		fprintf(stderr, ANSI_COLOR_RED "[!!] Error enviando el mensaje: %s (%s)\n" ANSI_COLOR_RESET, msgJoin, strerror(errno));
	} else {
		fprintf(stderr, ANSI_COLOR_GREEN "[B] Enviando JOIN a %s...\n" ANSI_COLOR_RESET, channel);
		status = 1;
	}
}
