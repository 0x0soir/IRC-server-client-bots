#include "../includes/G-2313-06-BOT_bot_handlers.h"
#include "../includes/G-2313-06-BOT_bot.h"

extern int socket_desc;
extern char* nick;
extern char* channel;

void server_in_command_privmsg(char* command){
  char *prefix, *channel, *msg, *parse_nick, *parse_user, *parse_host, *parse_server;
  char msgEnvio[2048] = "";
  syslog(LOG_INFO, "[BOT] [IN]: PRIVMSG");
  if(IRCParse_Privmsg(command, &prefix, &channel, &msg)==IRC_OK){
    if(IRCParse_ComplexUser(prefix, &parse_nick, &parse_user, &parse_host, &parse_server)==IRC_OK){
      syslog(LOG_INFO, "[BOT] [IN]: Channel: %s", channel);
      if(channel[0]=='#'){
        syslog(LOG_INFO, "[BOT] [IN]: Es canal");
        if(strstr(msg, nick)){
          fprintf(stderr, ANSI_COLOR_YELLOW "[S] Canal: %s | Mensaje: %s\n" ANSI_COLOR_RESET, channel, msg);
          removeSubstring(msg, nick);
          trim(msg);
          IRC_ToLower(msg);
          sprintf(msgEnvio, "%s: %s", parse_nick, server_bot_text_test(msg));
          server_out_command_privmsg(msgEnvio);
        }
      } else {
        syslog(LOG_INFO, "[BOT] [IN]: No es canal");
        fprintf(stderr, ANSI_COLOR_YELLOW "[S] MP: %s | Mensaje: %s\n" ANSI_COLOR_RESET, channel, msg);
      }
    }
  }
  IRC_MFree(7, &prefix, &channel, &msg, &parse_nick, &parse_user, &parse_host, &parse_server);
}

void server_in_command_ping(char* command){
  char *prefix = NULL, *msg = NULL, *msg2 = NULL, *parse_server = NULL, *parse_host = NULL;
  syslog(LOG_INFO, "[BOT] [IN]: PING %s", command);
  if(IRCParse_Ping(command, &prefix, &parse_server, &parse_host, &msg2)==IRC_OK){
    syslog(LOG_INFO, "[BOT] [IN]: PING 2 %s %s %s", parse_server, parse_host, msg2);
    if(!msg2){
      if(IRCMsg_Pong(&msg, prefix, parse_server, parse_host, parse_server+1)==IRC_OK){
        if(send(socket_desc, msg, strlen(msg), 0)>0){
          syslog(LOG_INFO, "[BOT] [IN] PING envia: %s", msg);
        }
      }
    } else {
      if(IRCMsg_Pong(&msg, prefix, parse_server, parse_host, msg2)==IRC_OK){
        if(send(socket_desc, msg, strlen(msg), 0)>0){
          syslog(LOG_INFO, "[BOT] [IN] PING envia: %s", msg);
        }
      }
    }
  }
  IRC_MFree(5, &prefix, &msg, &msg2, &parse_host, &parse_server);
}

void server_in_command_rpl_welcome(char* command){
  char *prefix, *msg, *parse_nick;
  IRCParse_RplWelcome(command, &prefix, &parse_nick, &msg);
  fprintf(stderr, ANSI_COLOR_YELLOW "[S] Conexión satisfactoria: %s\n" ANSI_COLOR_RESET, msg);
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_out_command_names(char* command){
  char *msg, *channel, *targetserver;
  syslog(LOG_INFO, "[BOT] Send names %s", command);
  if(IRCUserParse_Names(command, &channel, &targetserver)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] NAMES PARSE OK");
    IRCMsg_Names(&msg, NULL, channel, targetserver);
    send(socket_desc, msg, strlen(msg), 0);
  }
}

void server_out_command_list(char* command){
  char *msg, *channel, *search;
  syslog(LOG_INFO, "[CLIENTE] Send join %s", command);
  if(IRCUserParse_List(command, &channel, &search)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] LIST PARSE OK");
    IRCMsg_List(&msg, NULL, channel, search);
    send(socket_desc, msg, strlen(msg), 0);
  }
  IRC_MFree(3, &msg, &channel, &search);
}

void server_out_command_privmsg(char* command){
  char *msg = NULL, *channelActual = NULL;
  channelActual = channel;
  syslog(LOG_INFO, "[BOT] [OUT] Send privmsg %s", command);
  IRCMsg_Privmsg(&msg, NULL, channelActual, command);
  if(send(socket_desc, msg, strlen(msg), 0)>0){
    syslog(LOG_INFO, "[BOT] [OUT] Privmsg envia: %s", msg);
  } else {
    syslog(LOG_INFO, "[BOT] [OUT] Error al enviar: %s", msg);
  }
  IRC_MFree(1, &msg);
}

void removeSubstring(char *s,const char *toremove){
  while(s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}

void trim(char* str){
  if(!str)
      return;
  char* ptr = str;
  int len = strlen(ptr);

  while(len-1 > 0 && isspace(ptr[len-1]))
    ptr[--len] = 0;

  while(*ptr && isspace(*ptr))
    ++ptr, --len;

  memmove(str, ptr, len + 1);
}

char* server_bot_text_test(char * msg){
  char *textos_preguntas[] = {
      "hola",
      "años tienes",
      "que tal",
      "sentido de la vida",
      "aprobar",
      "chiste",
  };
  char *textos_respuestas[][2] = {
      {
        "Hola :)",
        "Ey!"
      },
      {
        "256 módulo 255.",
        "Nací junto a IRC."
      },
      {
        "Bien, como siempre gracias.",
        "Aquí estoy esperando a las segundas matrículas..."
      },
      {
        "Es complicado, pero podría ser aprobar Cirel en primera convocatoria :)",
        "El mundo se divide en 10 clases de personas, las que conocen el sistema binario y las que no."
      },
      {
        "Si",
        "No, te vas a segunda matrícula (o tercera?)"
      },
      {
        "¿Qué diferencia hay entre hacer la colada y utilizar Windows? - Que la colada no se cuelga sola.",
        "¿Qué diferencia hay entre hacer la colada y utilizar Windows? - Que la colada no se cuelga sola.",
      }
  };
  int i;
  syslog(LOG_INFO, "[BOT] [OUT] Buscando frase:%s", msg);
  for(i=0;i<6;i++){
      if(strstr(msg, textos_preguntas[i])){
        return textos_respuestas[i][rand()%2];
      }
  }
  return "No he entendido nada, prueba otra cosa :D";
}
