#include "../includes/G-2313-06-BOT_bot_handlers.h"
#include "../includes/G-2313-06-BOT_bot.h"

extern int socket_desc;
extern char* nick;
extern arraylist* channelsJoin;
extern arraylist* usersSpyTerms;

void server_in_command_privmsg(char* command){
  char *prefix, *channel, *msg, *parse_nick, *parse_user,
  *parse_host, *parse_server, msgEnvio[512] = "", *msgEnvio2;
  syslog(LOG_INFO, "[BOT] [IN]: PRIVMSG");
  if(IRCParse_Privmsg(command, &prefix, &channel, &msg)==IRC_OK){
    if(IRCParse_ComplexUser(prefix, &parse_nick, &parse_user, &parse_host, &parse_server)==IRC_OK){
      syslog(LOG_INFO, "[BOT] [IN]: Channel: %s", channel);
      if(channel[0]=='#'){
        syslog(LOG_INFO, "[BOT] [IN]: Es canal");
        server_bot_check_spy(msg, channel, parse_nick);
      } else {
        syslog(LOG_INFO, "[BOT] [IN]: No es canal");
        if(msg[0]=='R'){
          if(msg[1]==':'){
            fprintf(stderr, ANSI_COLOR_YELLOW "[S] Nueva petición de rastreo: %s | Término: %s\n" ANSI_COLOR_RESET, parse_nick, msg);
            removeSubstring(msg, "R:");
            sprintf(msgEnvio, "%s:%s", parse_nick, msg);
            msgEnvio2 = malloc(sizeof(char)*strlen(msgEnvio)+1);
            strcpy(msgEnvio2, msgEnvio);
            arraylist_add(usersSpyTerms, msgEnvio2);
          }
        }
      }
    }
  }
  IRC_MFree(7, &prefix, &channel, &msg, &parse_nick, &parse_user, &parse_host, &parse_server);
}

void server_in_command_join(char* command){
  char *prefix, *channel, *key, *msg, *join_nick,
   *join_user, *join_host, *join_server;
  int i, checker = 0;
  if(IRCParse_Join(command, &prefix, &channel, &key, &msg)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] [IN]: JOIN PREFIX %s", prefix);
    if(IRCParse_ComplexUser(prefix, &join_nick, &join_user, &join_host, &join_server)==IRC_OK){
      if(strcmp(join_nick, nick)==0){
        /* Yo */
        fprintf(stderr, ANSI_COLOR_YELLOW "[S] Ahora estás en el canal %s %d\n" ANSI_COLOR_RESET, msg, channelsJoin->size);
        for(i = 0; i < channelsJoin->size; i = i + 1 ){
          if((char * ) arraylist_get(channelsJoin, i)){
            if(strcmp((char * ) arraylist_get(channelsJoin, i), msg)==0){
              checker = 1;
            }
          }
        }
        if((channelsJoin->size==0)||(checker==0)){
          arraylist_add(channelsJoin, msg);
        }
      }
    }
  }
  IRC_MFree(7, &prefix, &key, &msg, &join_nick, &join_user,
    &join_host, &join_server);
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
  channelsJoin = arraylist_create();
  usersSpyTerms = arraylist_create();
  fprintf(stderr, ANSI_COLOR_GREEN "[B] Enviando comando LIST...\n" ANSI_COLOR_RESET);
  server_out_command_list("/LIST");
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_rpl_list(char* command){
  char *prefix, *parse_nick, *parse_channel, *parse_visible, *parse_topic;
  syslog(LOG_INFO, "[BOT] [IN]: RPL_LIST");
  IRCParse_RplList(command, &prefix, &parse_nick, &parse_channel, &parse_visible, &parse_topic);
  if(parse_channel[0]=='#'){
    send_join(parse_channel);
  }
  IRC_MFree(5, &prefix, &parse_nick, &parse_channel, &parse_visible, &parse_topic);
}

void server_out_command_list(char* command){
  char *msg, *channel, *search;
  syslog(LOG_INFO, "[BOT] Send list");
  if(IRCUserParse_List(command, &channel, &search)==IRC_OK){
    syslog(LOG_INFO, "[BOT] LIST PARSE OK");
    IRCMsg_List(&msg, NULL, channel, search);
    send(socket_desc, msg, strlen(msg), 0);
  }
  IRC_MFree(3, &msg, &channel, &search);
}

void server_out_command_privmsg(char* command){
  char *msg = NULL, *channelActual = NULL;
  channelActual = "#redes2";
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

void server_bot_check_spy(char * msg, char * channel, char * user_msg){
  int i;
  char * msgTmp, * user, * term, * msgSocket, msgEnvio[512] = "";
  for(i=0;i<usersSpyTerms->size;i++){
    msgTmp = malloc(sizeof(char)*strlen((char *) arraylist_get(usersSpyTerms, i))+1);
    strcpy(msgTmp, (char *) arraylist_get(usersSpyTerms, i));
    user = strtok(msgTmp, ":");
    term = strtok(NULL, "");
    if(strstr(msg, term)){
      fprintf(stderr, ANSI_COLOR_CYAN "[B] Coincidencia encontrada: Usuario: %s Término: %s\n" ANSI_COLOR_RESET, user, term);
      sprintf(msgEnvio, "Detectado término buscado (%s) - Canal: %s - Usuario: %s - Mensaje: %s",
      term, channel, user_msg, msg);
      if(IRCMsg_Privmsg(&msgSocket, NULL, user, msgEnvio)==IRC_OK){
        if(send(socket_desc, msgSocket, strlen(msgSocket), 0)<=0){
          fprintf(stderr, ANSI_COLOR_RED "[B] Error al enviar mensaje al usuario %s\n" ANSI_COLOR_RESET, user);
        }
      }
    }
    IRC_MFree(1, &msgTmp);
  }
}
