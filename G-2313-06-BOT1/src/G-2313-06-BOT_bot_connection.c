#include "../includes/G-2313-06-BOT_bot.h"

extern char *server;
extern int port;
extern char *channel;
extern char *nick;
extern int socket_desc;
extern int alive;
int status = 0;
pthread_t thread_response;

void run_siri(){
	char buff[CLIENT_MESSAGE_MAXSIZE] = "";
	int valread;
	connect_server();
	while (alive) {
		if(status==0){
			send_nick();
			send_user();
			send_join();
		}
		valread = recv(socket_desc, buff, CLIENT_MESSAGE_MAXSIZE, 0);
		if(valread == 0) {
			continue;
		}
		syslog(LOG_INFO, "[BOT] Mensaje recibido: %s", buff);
		memset(buff,0,sizeof(buff));
	}
}

void connect_server(){
	struct sockaddr_in socket_address;
  struct hostent *server_info;
  char *command, *msgNick, *msgUser;
  fprintf(stderr, ANSI_COLOR_YELLOW "Conectando al servidor...\n" ANSI_COLOR_RESET);
  if(!nick||!server){
    return IRCERR_NOCONNECT;
  } else {
    /* Se crea el socket que vamos a usar */
    if((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
      fprintf(stderr, ANSI_COLOR_RED "[!!] Error al generar socket\n" ANSI_COLOR_RESET);
      return IRCERR_NOCONNECT;
    } else {
      fprintf(stderr, ANSI_COLOR_GREEN "[!] Socket generado OK\n" ANSI_COLOR_RESET);
    }

    server_info = gethostbyname(server);
    if(server_info == NULL) {
      fprintf(stderr, ANSI_COLOR_RED "[!!] Error al obtener host\n" ANSI_COLOR_RESET);
      return IRCERR_NOCONNECT;
    } else {
      fprintf(stderr, ANSI_COLOR_GREEN "[!] Host obtenido OK\n" ANSI_COLOR_RESET);
    }

    bzero((char *) &socket_address, sizeof (socket_address));
    socket_address.sin_family = AF_INET;
    bcopy((char *) server_info->h_addr, (char *) &socket_address.sin_addr.s_addr, server_info->h_length);
    socket_address.sin_port = htons(port);

    if(connect(socket_desc, (struct sockaddr*) &socket_address, sizeof (socket_address)) < 0) {
			fprintf(stderr, ANSI_COLOR_RED "[!!] Error conectando al servidor\n" ANSI_COLOR_RESET);
      return IRCERR_NOCONNECT;
    } else {
			fprintf(stderr, ANSI_COLOR_GREEN "[!] Conexión con servidor OK\n" ANSI_COLOR_RESET);
    }
    /*pthread_create(&thread_response, NULL, client_function_response, NULL);*/

    /* Ya conectados, parseamos nick/user y lo enviamos */
    /*IRCMsg_Nick(&msgNick, NULL, nick, NULL);
    IRCMsg_User(&msgUser, NULL, "bot_siri", server, "bot_siri");

    IRC_PipelineCommands(&command, msgNick, msgUser, NULL);
    syslog(LOG_ERR, "[CLIENTE] Registro pipelined: %s", command);

    if(send(socket_desc, command, strlen(command), 0) < 0) {
			fprintf(stderr, ANSI_COLOR_RED "[!!] Error enviando el mensaje: %s (%s)\n" ANSI_COLOR_RESET, command, strerror(errno));
			return IRCERR_NOCONNECT;
    } else {
			fprintf(stderr, ANSI_COLOR_GREEN "[!] Mensaje enviado OK\n" ANSI_COLOR_RESET);
		}

    syslog(LOG_ERR, "[CLIENTE] Socket desc: %d", socket_desc);
    free(command);*/
  }
}

void send_nick(){
	char *command, *msgNick;
	IRCMsg_Nick(&msgNick, NULL, nick, NULL);
	if(send(socket_desc, msgNick, strlen(msgNick), 0) < 0) {
		fprintf(stderr, ANSI_COLOR_RED "[!!] Error enviando el mensaje: %s (%s)\n" ANSI_COLOR_RESET, msgNick, strerror(errno));
		return IRCERR_NOCONNECT;
	} else {
		fprintf(stderr, ANSI_COLOR_GREEN "[!] Mensaje nick enviado OK\n" ANSI_COLOR_RESET);
		status = 1;
	}
}

void send_user(){
	char *command, *msgUser;
	IRCMsg_User(&msgUser, NULL, "bot_siri", server, "bot_siri");
	if(send(socket_desc, msgUser, strlen(msgUser), 0) < 0) {
		fprintf(stderr, ANSI_COLOR_RED "[!!] Error enviando el mensaje: %s (%s)\n" ANSI_COLOR_RESET, msgUser, strerror(errno));
		return IRCERR_NOCONNECT;
	} else {
		fprintf(stderr, ANSI_COLOR_GREEN "[!] Mensaje user enviado OK\n" ANSI_COLOR_RESET);
		status = 1;
	}
}

void send_join(){
	char *command, *msgJoin;
	IRCMsg_Join(&msgJoin, NULL, channel, NULL, NULL);
	if(send(socket_desc, msgJoin, strlen(msgJoin), 0) < 0) {
		fprintf(stderr, ANSI_COLOR_RED "[!!] Error enviando el mensaje: %s (%s)\n" ANSI_COLOR_RESET, msgJoin, strerror(errno));
		return IRCERR_NOCONNECT;
	} else {
		fprintf(stderr, ANSI_COLOR_GREEN "[!] Mensaje join enviado OK\n" ANSI_COLOR_RESET);
		status = 1;
	}
}
