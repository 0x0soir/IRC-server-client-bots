#include "../includes/G-2313-06-P2_client_function_handlers.h"

extern int socket_desc;
extern char* nick_cliente;

/* IN FUNCTIONS */
void server_in_command_nick(char* command){
  char *prefix, *nick, *msg, msgEnvio[512] = "";
  char *my_nick, *my_user, *my_realname, *my_password, *my_server,
   *nick_nick, *nick_user, *nick_host, *nick_server;
  int my_port, my_ssl;
  char *channelActual;
  channelActual = IRCInterface_ActiveChannelName();
  IRCInterface_PlaneRegisterInMessageThread(command);
  if(IRCParse_Nick(command, &prefix, &nick, &msg)==IRC_OK){
    if(IRCParse_ComplexUser(prefix, &nick_nick, &nick_user, &nick_host, &nick_server)==IRC_OK){
      IRCInterface_GetMyUserInfoThread(&my_nick, &my_user, &my_realname, &my_password, &my_server, &my_port, &my_ssl);
      if(strcmp(nick_user+1, my_user)==0){
        if(!nick_cliente){
          syslog(LOG_INFO, "[CLIENTE] No hay puntero reservado");
          nick_cliente = malloc(sizeof(msg));
          strcpy(nick_cliente, msg);
        }
        sprintf(msgEnvio, "Ahora eres conocido como: %s", msg);
        IRCInterface_ErrorDialogThread(msgEnvio);
        syslog(LOG_INFO, "[CLIENTE] NICK OLD: %s NEW: %s", nick_cliente, msg);
        if((channelActual!=NULL)&&(strcmp(channelActual, "System")!=0)){
          IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), "*", msgEnvio);
        } else {
          IRCInterface_WriteSystemThread(NULL, msgEnvio);
        }
        IRCInterface_ChangeNick(nick_cliente, msg);
        if(nick_cliente){
          free(nick_cliente);
          nick_cliente = NULL;
          nick_cliente = malloc(sizeof(msg));
          strcpy(nick_cliente, msg);
        }
        syslog(LOG_INFO, "[CLIENTE] NICK NEW: %s", nick_cliente);
      } else {
        syslog(LOG_INFO, "[CLIENTE] NICK NO ERES TU %s %s", nick_nick, msg);
        sprintf(msgEnvio, "%s ahora es conocido como: %s", nick_nick, msg);
        IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), "*", msgEnvio);
      }
    }
  }
  IRC_MFree(12, &prefix, &nick, &msg, &my_nick, &my_user, &my_realname,
    &my_password, &my_server, &nick_nick, &nick_user, &nick_host, &nick_server);
}

void server_in_command_pong(char* command){
  IRCInterface_PlaneRegisterInMessageThread(command);
}

void server_in_command_join(char* command){
  char *prefix, *channel, *key, *msg, msgEnvio[512] = "", *join_nick,
   *join_user, *join_host, *join_server, *msgWho;
   memset(msgEnvio,0,sizeof(msgEnvio));
  IRCInterface_PlaneRegisterInMessageThread(command);
  if(IRCParse_Join(command, &prefix, &channel, &key, &msg)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] [IN]: JOIN PREFIX %s", prefix);
    if(IRCParse_ComplexUser(prefix, &join_nick, &join_user, &join_host, &join_server)==IRC_OK){
      if(strcmp(join_nick, nick_cliente)==0){
        sprintf(msgEnvio, "Te has unido al canal %s", msg);
      } else {
        sprintf(msgEnvio, "%s (~%s@%s) ha entrado en %s", join_nick, join_nick, join_server, msg);
      }
      syslog(LOG_INFO, "[CLIENTE] [IN]: JOIN mensaje: %s", msgEnvio);
      if(IRCInterface_QueryChannelExistThread(msg)==0){
        IRCInterface_AddNewChannelThread(msg, 0);
      }
      if(IRCMsg_Who(&msgWho, NULL, msg, NULL) == IRC_OK) {
        if(send(socket_desc, msgWho, strlen(msgWho), 0)>0){
          IRCInterface_PlaneRegisterOutMessageThread(msgWho);
        }
      }
      if(msgWho){
        IRC_MFree(1, &msgWho);
      }
      IRCInterface_WriteChannelThread(msg, "*", msgEnvio);
    }
  }
  IRC_MFree(8, &prefix, &channel, &key, &msg, &join_nick, &join_user,
    &join_host, &join_server);
}

void server_in_command_part(char* command){
  char *prefix, *channel, *msg, msgEnvio[512] = "", *part_nick, *part_user, *part_host, *part_server;
  IRCInterface_PlaneRegisterInMessageThread(command);
  if(IRCParse_Part(command, &prefix, &channel, &msg)==IRC_OK){
    if(IRCParse_ComplexUser(prefix, &part_nick, &part_user, &part_host, &part_server)==IRC_OK){
      if(strcmp(part_nick, nick_cliente)==0){
        IRCInterface_DeleteNickChannelThread(channel, part_nick);
        IRCInterface_RemoveChannelThread(channel);
        sprintf(msgEnvio, "Has salido del canal %s: %s", channel, msg);
      } else {
        IRCInterface_DeleteNickChannelThread(channel, part_nick);
        sprintf(msgEnvio, "%s (~%s@%s) ha abandonado el canal %s: %s", part_nick, part_nick, part_server, channel, msg);
        IRCInterface_WriteChannelThread(channel, NULL, msgEnvio);
      }
      syslog(LOG_INFO, "[CLIENTE] [IN]: Part OK %s %s %s", part_nick, msg, msgEnvio);
    }
  }
  IRC_MFree(8, &prefix, &channel, &msg, &part_nick, &part_user, &part_host, &part_server);
}

void server_in_command_mode(char* command){
  char *prefix, *channel, *mode, *mode_nick, *mode_user, *mode_host, *mode_server, *user_target;
  char buff[200];
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: MODE");
  if(IRCParse_Mode(command, &prefix, &channel, &mode, &user_target)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] [IN]: MODE parseado");
    if(IRCParse_ComplexUser(prefix, &mode_nick, &mode_user, &mode_host, &mode_server)==IRC_OK){
      syslog(LOG_INFO, "[CLIENTE] [IN]: MODE complex user");
      if (strstr(mode, "+l")) {
        sprintf(buff, "%s establece limite del canal a %s", mode_nick, user_target);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "-l")) {
        sprintf(buff, "%s quita limite del canal", mode_nick);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+k")) {
        sprintf(buff, "%s establece contraseña del canal como %s", mode_nick, user_target);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "-k")) {
        sprintf(buff, "%s quita contraseña del canal", mode_nick);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+m")) {
        syslog(LOG_INFO, "[CLIENTE] [IN]: MODE M %s %s", mode_nick, channel);
        sprintf(buff, "%s establece el modo +m %s", mode_nick, channel);
        syslog(LOG_INFO, "[CLIENTE] [IN]: MODE M tras sprintf");
        IRCInterface_WriteChannelThread(channel, "*", buff);
        syslog(LOG_INFO, "[CLIENTE] [IN]: MODE M tras writechannel");
      } else if (strstr(mode, "-m")) {
        sprintf(buff, "%s establece el modo -m %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+p")) {
        sprintf(buff, "%s establece el modo +p %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "-p")) {
        sprintf(buff, "%s establece el modo -p %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+i")) {
        sprintf(buff, "%s establece el modo +i %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "-i")) {
        sprintf(buff, "%s establece el modo -i %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+s")) {
        sprintf(buff, "%s establece el modo +s %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "-s")) {
        sprintf(buff, "%s establece el modo -s %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+n")) {
        sprintf(buff, "%s establece el modo +n %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "-n")) {
        sprintf(buff, "%s establece el modo -n %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+t")) {
        sprintf(buff, "%s establece el modo +t %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "-t")) {
        sprintf(buff, "%s establece el modo -t %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+b")) {
        sprintf(buff, "%s banea a %s", mode_nick, user_target);
        IRCInterface_WriteChannelThread(channel, "*", buff);
      } else if (strstr(mode, "+v")) {
        sprintf(buff, "%s da voz a %s", mode_nick, user_target);
        IRCInterface_WriteChannelThread(channel, "*", buff);
        IRCInterface_ChangeNickStateChannelThread(channel, user_target, VOICE);
      } else if (strstr(mode, "-v")) {
        sprintf(buff, "%s quita voz a %s", mode_nick, user_target);
        IRCInterface_WriteChannelThread(channel, "*", buff);
        IRCInterface_ChangeNickStateChannelThread(channel, user_target, NONE);
      } else if (strstr(mode, "+o")) {
        sprintf(buff, "%s da OP a %s", mode_nick, user_target);
        IRCInterface_WriteChannelThread(channel, "*", buff);
        IRCInterface_ChangeNickStateChannelThread(channel, user_target, OPERATOR);
      } else if (strstr(mode, "-o")) {
        sprintf(buff, "%s quita OP a %s", mode_nick, user_target);
        IRCInterface_WriteChannelThread(channel, "*", buff);
        IRCInterface_ChangeNickStateChannelThread(channel, user_target, NONE);
      }
    }
  }
  IRC_MFree(6, &prefix, &channel, &mode_nick, &mode_user, &mode_host, &mode_server);;
}

void server_in_command_topic(char* command){
  char *prefix, *channel, *msg, *parse_nick, *parse_user, *parse_host, *parse_server;
  char buffer[200];
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: TOPIC");
  if(IRCParse_Topic(command, &prefix, &channel, &msg)==IRC_OK){
    if(IRCParse_ComplexUser(prefix, &parse_nick, &parse_user, &parse_host, &parse_server)==IRC_OK){
      syslog(LOG_INFO, "[CLIENTE] [IN]: TOPIC parseado");
      sprintf(buffer, "%s ha cambiado el tema a: %s", parse_nick, msg);
      IRCInterface_SetTopicThread(msg);
      IRCInterface_WriteChannelThread(channel, "*", buffer);
    }
  }
  IRC_MFree(7, &prefix, &channel, &msg, &parse_nick, &parse_user, &parse_host, &parse_server);
}

void server_in_command_kick(char* command){
  char *prefix, *channel, *msg, *user_target, *parse_nick, *parse_user, *parse_host, *parse_server;
  char buffer[512] = "";
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: KICK");
  if(IRCParse_Kick(command, &prefix, &channel, &user_target, &msg)==IRC_OK){
    if(IRCParse_ComplexUser(prefix, &parse_nick, &parse_user, &parse_host, &parse_server)==IRC_OK){
      syslog(LOG_INFO, "[CLIENTE] [IN]: KICK parseado");
      syslog(LOG_INFO, "[CLIENTE] [IN]: user_target: %s", user_target);
      syslog(LOG_INFO, "[CLIENTE] [IN]: nick_cliente: %s", nick_cliente);
      if(strcmp(user_target, nick_cliente)==0){
        sprintf(buffer, "Has sido expulsado de %s por %s (Motivo: %s)", channel, parse_nick, msg);
        IRCInterface_ErrorDialogThread(buffer);
        IRCInterface_DeleteNickChannel(channel, user_target);
        IRCInterface_RemoveChannelThread(channel);
        IRCInterface_WriteSystemThread("*", buffer);
      } else {
        IRCInterface_DeleteNickChannel(channel, user_target);
        sprintf(buffer, "%s ha expulsado a %s de %s (%s)", parse_nick, user_target, channel, msg);
        IRCInterface_WriteChannelThread(channel, NULL, buffer);
      }
    }
  }
  IRC_MFree(8, &prefix, &channel, &msg, &user_target, &parse_nick, &parse_user, &parse_host, &parse_server);
}

void server_in_command_who(char* command){
  char *prefix, *channel, *msg, *user_target, *parse_nick, *parse_user, *parse_host, *parse_server, *parse_type, *parse_realname;
  int parse_hopcount;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: WHO");
  if (IRCParse_RplWhoReply(command, &prefix, &parse_nick, &channel, &parse_user, &parse_host, &parse_server, &user_target, &parse_type, &msg, &parse_hopcount, &parse_realname) == IRC_OK) {
    IRCInterface_AddNickChannelThread(channel, user_target, parse_user, parse_realname, parse_host, 0);
  }
  IRC_MFree(7, &prefix, &channel, &msg, &user_target, &parse_nick, &parse_user, &parse_host, &parse_server, &parse_type, &parse_realname);
}

void server_in_command_privmsg(char* command){
  char *prefix, *channel, *msg, *parse_nick, *parse_user, *parse_host, *parse_server, *msgFichero;
  pthread_t tid;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: PRIVMSG");
  if(IRCParse_Privmsg(command, &prefix, &channel, &msg)==IRC_OK){
    if(IRCParse_ComplexUser(prefix, &parse_nick, &parse_user, &parse_host, &parse_server)==IRC_OK){
      syslog(LOG_INFO, "[CLIENTE] [IN]: Channel: %s", channel);
      if(msg[0]==1){
        if (msg[1] == 'F' && msg[2] == 'S'){	// Envio de ficheros
          msgFichero = malloc(strlen(msg) + 1);
          strcpy(msgFichero, msg);
    			pthread_create(&tid, NULL, &server_especial_recibir_ficheros, (void *)msgFichero);
    		}
      } else {
        if(channel[0]=='#'){
          syslog(LOG_INFO, "[CLIENTE] [IN]: Es canal");
          IRCInterface_WriteChannelThread(channel, parse_nick, msg);
        } else {
          syslog(LOG_INFO, "[CLIENTE] [IN]: No es canal");
          if(IRCInterface_QueryChannelExist(channel)==FALSE){
            IRCInterface_AddNewChannelThread(parse_nick, 0);
            IRCInterface_WriteChannelThread(parse_nick, parse_nick, msg);
          } else {
            IRCInterface_WriteChannelThread(parse_nick, parse_nick, msg);
          }
        }
      }
    }
  }
  IRC_MFree(7, &prefix, &channel, &msg, &parse_nick, &parse_user, &parse_host, &parse_server);
}

void server_in_command_ping(char* command){
  char *prefix, *msg, *msg2, *parse_server, *parse_host;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: PING");
  if(IRCParse_Ping(command, &prefix, &parse_server, &parse_host, &msg2)==IRC_OK){
    if(IRCMsg_Pong(&msg, prefix, parse_server, parse_host, msg2)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] [IN] PING envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    }
  }
  IRC_MFree(5, &prefix, &msg, &msg2, &parse_host, &parse_server);
}

void server_in_command_rpl_welcome(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL WELCOME");
  IRCParse_RplWelcome(command, &prefix, &parse_nick, &msg);
  IRCInterface_WriteSystemThread(NULL, msg);
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_rpl_created(char* command){
  char *prefix, *msg, *parse_nick, *parse_timedate;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL CREATED");
  IRCParse_RplCreated(command, &prefix, &parse_nick, &parse_timedate, &msg);
  IRCInterface_WriteSystemThread(NULL, msg);
  IRC_MFree(4, &prefix, &parse_nick, &parse_timedate, &msg);
}

void server_in_command_rpl_yourhost(char* command){
  char *prefix, *msg, *parse_nick, *parse_servername, *parse_versionname;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL YOURHOST");
  IRCParse_RplYourHost(command, &prefix, &parse_nick, &msg, &parse_servername, &parse_versionname);
  IRCInterface_WriteSystemThread(NULL, msg);
  IRC_MFree(5, &prefix, &parse_nick, &parse_servername, &parse_versionname, &msg);
}

void server_in_command_rpl_luserclient(char* command){
  char *prefix, *msg, *parse_nick;
  int parse_nusers, parse_ninvisibles, parse_nservers;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL LUSERCLIENT");
  IRCParse_RplLuserClient(command, &prefix, &parse_nick, &msg, &parse_nusers, &parse_ninvisibles, &parse_nservers);
  IRCInterface_WriteSystemThread(NULL, msg);
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_rpl_luserme(char* command){
  char *prefix, *msg, *parse_nick;
  int parse_nclientes, parse_nservers;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL LUSERME");
  IRCParse_RplLuserMe(command, &prefix, &parse_nick, &msg, &parse_nclientes, &parse_nservers);
  IRCInterface_WriteSystemThread(NULL, msg);
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_rpl_motdstart(char* command){
  char *prefix, *msg, *parse_nick, *parse_servername;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL START OF MOTD");
  IRCParse_RplMotdStart(command, &prefix, &parse_nick, &msg, &parse_servername);
  IRCInterface_WriteSystemThread(NULL, msg);
  IRC_MFree(4, &prefix, &parse_nick, &parse_servername, &msg);
}

void server_in_command_rpl_motd(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL MOTD");
  IRCParse_RplMotd(command, &prefix, &parse_nick, &msg);
  IRCInterface_WriteSystemThread(NULL, msg);
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_rpl_endofmotd(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL END OF MOTD");
  IRCParse_RplEndOfMotd(command, &prefix, &parse_nick, &msg);
  IRCInterface_WriteSystemThread(NULL, msg);
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_rpl_whoreply(char* command){
  char *prefix, *nick, *channel, *user, *host, *server, *nick2, *type, *msg, *realname;
  int hopcount;
  size_t length;
  nickstate ns;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: RPL WHO");
  if (IRCParse_RplWhoReply(command, &prefix, &nick, &channel, &user, &host, &server, &nick2, &type, &msg, &hopcount, &realname) == IRC_OK) {
    length = strlen(type);
    if(length==2){
      if(type[1]=='@'){
        ns = OPERATOR;
      } else {
        ns = NONE;
      }
    } else {
      ns = NONE;
    }
    IRCInterface_AddNickChannelThread(channel, nick2, user, realname, host, ns);
    IRC_MFree(10, &prefix, &nick, &channel, &user, &host, &server, &nick2, &type, &msg, &realname);
  }
}

/* OUT FUNCTIONS */
void server_out_command_nick(char* command){
  char *newNick, *msg;
  syslog(LOG_INFO, "[CLIENTE] Send nick %s", command);
  if(IRCUserParse_Nick(command, &newNick)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] NICK PARSE OK");
    IRCMsg_Nick(&msg, NULL, newNick, NULL);
    if(send(socket_desc, msg, strlen(msg), 0)>0){
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
  if(newNick){
    free(newNick);
    newNick = NULL;
  }
  if(msg){
    free(msg);
    msg = NULL;
  }
}

void server_out_command_join(char* command){
  char *msg, *channel, *password;
  syslog(LOG_INFO, "[CLIENTE] Send join %s", command);
  if(IRCUserParse_Join(command, &channel, &password)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] JOIN PARSE OK");
    IRCMsg_Join(&msg, NULL, channel, password, NULL);
    if(send(socket_desc, msg, strlen(msg), 0)>0){
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
  IRC_MFree(3, &msg, &channel, &password);
}

void server_out_command_names(char* command){
  char *msg, *channel, *targetserver;
  syslog(LOG_INFO, "[CLIENTE] Send join %s", command);
  if(IRCUserParse_Names(command, &channel, &targetserver)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] NAMES PARSE OK");
    IRCMsg_Names(&msg, NULL, channel, targetserver);
    if(send(socket_desc, msg, strlen(msg), 0)>0){
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
}

void server_out_command_list(char* command){
  char *msg, *channel, *search;
  syslog(LOG_INFO, "[CLIENTE] Send join %s", command);
  if(IRCUserParse_List(command, &channel, &search)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] LIST PARSE OK");
    IRCMsg_List(&msg, NULL, channel, search);
    if(send(socket_desc, msg, strlen(msg), 0)>0){
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
  IRC_MFree(3, &msg, &channel, &search);
}

void server_out_command_part(char* command){
  char *msg = NULL, *channelActual = NULL, *channel = NULL;
  channelActual = IRCInterface_ActiveChannelName();
  syslog(LOG_INFO, "[CLIENTE] Send part %s %s", command, channelActual);
  if(IRCUserParse_Part(command, &channel)==IRC_OK){
    if(channel){
      if(IRCInterface_QueryChannelExist(channel)==FALSE){
        IRCInterface_WriteChannel(channelActual, NULL, " Uso: PART [<canal>], abandona el canal, por omisión el canal actual");
      } else {
        IRC_ToLower(channel);
        IRCMsg_Part(&msg, NULL, channel, "Adios");
      }
    } else {
      IRCMsg_Part(&msg, NULL, channelActual, "Adios");
    }
    if(send(socket_desc, msg, strlen(msg), 0)>0){
      syslog(LOG_INFO, "[CLIENTE] Part envia: %s", msg);
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
  IRC_MFree(1, &msg);
  IRC_MFree(1, &channel);
}

void server_out_command_mode(char* command){
  char *msg = NULL, *channelActual = NULL, *mode = NULL, *filter = NULL;
  channelActual = IRCInterface_ActiveChannelName();
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send mode %s", command);
  if(IRCUserParse_Mode(command, &mode, &filter)==IRC_OK){
    if(IRCMsg_Mode(&msg, NULL, channelActual, mode, NULL)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] Mode envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    } else {
      syslog(LOG_INFO, "[CLIENTE] [OUT] Mode error en IRCMsg_Mode");
    }
  } else {
    syslog(LOG_INFO, "[CLIENTE] [OUT] Mode error en UserParse_Mode");
  }
  IRC_MFree(4, &msg, &channelActual, &mode, &filter);
}

void server_out_command_kick(char* command){
  char *msg = NULL, *channelActual = NULL, *user_target = NULL;
  channelActual = IRCInterface_ActiveChannelName();
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send kick %s", command);
  if(IRCUserParse_Kick(command, &user_target, &msg)==IRC_OK){
    if(IRCMsg_Kick(&msg, NULL, channelActual, user_target, msg)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] Kick envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    } else {
      syslog(LOG_INFO, "[CLIENTE] [OUT] Mode error en IRCMsg_Kick");
    }
  } else {
    syslog(LOG_INFO, "[CLIENTE] [OUT] Mode error en UserParse_Kick");
  }
  IRC_MFree(3, &msg, &channelActual, &user_target);
}

void server_out_command_privmsg(char* command){
  char *msg = NULL, *channelActual = NULL;
  channelActual = IRCInterface_ActiveChannelName();
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send privmsg %s", command);
  IRCMsg_Privmsg(&msg, NULL, channelActual, command);
  if(strcmp(channelActual, "System")==0){
    IRCInterface_ErrorDialog("Para enviar un mensaje de texto debes unirte a un canal antes.");
  } else {
    IRCInterface_WriteChannel(channelActual, nick_cliente, command);
    if(send(socket_desc, msg, strlen(msg), 0)>0){
      syslog(LOG_INFO, "[CLIENTE] [OUT] Privmsg envia: %s", msg);
      IRCInterface_PlaneRegisterOutMessage(msg);
    } else {
      syslog(LOG_INFO, "[CLIENTE] [OUT] Error al enviar: %s", msg);
    }
  }
  IRC_MFree(1, &msg);
}

void server_out_command_whois(char* command){
  char *msg = NULL, *msg2 = NULL;
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send whois %s", command);
  if(IRCUserParse_Whois(command, &msg)==IRC_OK){
    if(IRCMsg_Whois(&msg2, NULL, NULL, msg)==IRC_OK){
      if(send(socket_desc, msg2, strlen(msg2), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] Whois envia: %s", msg2);
        IRCInterface_PlaneRegisterOutMessage(msg2);
      }
    }
  }
  IRC_MFree(2, &msg, &msg2);
}

void server_out_command_invite(char* command){
  char *msg = NULL, *nick = NULL, *channel = NULL;
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send invite %s", command);
  if(IRCUserParse_Invite(command, &nick, &channel)==IRC_OK){
    if(IRCMsg_Invite(&msg, NULL, nick, channel)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] Invite envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    }
  }
  IRC_MFree(3, &msg, &nick, &channel);
}

void server_out_command_topic(char* command){
  char *msg = NULL, *topic = NULL,*channelActual = NULL;
  channelActual = IRCInterface_ActiveChannelName();
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send topic %s", command);
  if(IRCUserParse_Topic(command, &topic)==IRC_OK){
    if(strcmp(channelActual, "System")==0){
      IRCInterface_ErrorDialog("Debes estar en un canal para ejecutar este comando. (/join #<canal>)");
    } else {
      if(IRCMsg_Topic(&msg, NULL, channelActual, topic)==IRC_OK){
        if(send(socket_desc, msg, strlen(msg), 0)>0){
          syslog(LOG_INFO, "[CLIENTE] Topic envia: %s", msg);
          IRCInterface_PlaneRegisterOutMessage(msg);
        }
      }
    }
  }
  IRC_MFree(3, &msg, &topic, &channelActual);
}

void server_out_command_me(char* command){
  char *msg = NULL, *msg2 = NULL,*channelActual = NULL;
  channelActual = IRCInterface_ActiveChannelName();
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send me %s", command);
  if(IRCUserParse_Me(command, &msg2)==IRC_OK){
    if(strcmp(channelActual, "System")==0){
      IRCInterface_ErrorDialog("Debes estar en un canal para ejecutar este comando. (/join #<canal>)");
    } else {
      if(IRCMsg_Privmsg(&msg, NULL, channelActual, msg2)==IRC_OK){
        if(send(socket_desc, msg, strlen(msg), 0)>0){
          syslog(LOG_INFO, "[CLIENTE] Me envia: %s", msg);
          IRCInterface_PlaneRegisterOutMessage(msg);
        }
      }
    }
  }
  IRC_MFree(3, &msg, &msg2, &channelActual);
}

void server_out_command_msg(char* command){
  char *msg = NULL, *msg2 = NULL, *target = NULL;
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send msg %s", command);
  if(IRCUserParse_Msg(command, &target, &msg2)==IRC_OK){
    if(IRCMsg_Privmsg(&msg, NULL, target, msg2)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] Msg envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    }
  }
  IRC_MFree(3, &msg, &msg2, &target);
}

void server_out_command_notice(char* command){
  char *msg = NULL, *msg2 = NULL, *target = NULL;
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send notice %s", command);
  if(IRCUserParse_Notice(command, &target, &msg2)==IRC_OK){
    if(IRCMsg_Notice(&msg, NULL, target, msg2)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] Notice envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    }
  }
  IRC_MFree(3, &msg, &msg2, &target);
}

void server_out_command_ignore(char* command){
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send ignore %s", command);
  IRCInterface_ErrorDialog("Este comando no funciona completamente ya que la libreria de Metis es limitada.");
}

void server_out_command_who(char* command){
  char *msg = NULL, *target = NULL;
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send who %s", command);
  if(IRCUserParse_Who(command, &target)==IRC_OK){
    if(IRCMsg_Who(&msg, NULL, target, NULL)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] who envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    }
  } else {
    IRCInterface_ErrorDialog("Comando mal introducido. Formato: (/who #<canal>)");
  }
  IRC_MFree(2, &msg, &target);
}

void server_out_command_whowas(char* command){
  char *msg = NULL, *target = NULL;
  int max;
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send whowas %s", command);
  if(IRCUserParse_WhoWas(command, &target, &max)==IRC_OK){
    if(IRCMsg_Whowas(&msg, NULL, target, 0, NULL)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] Whowas envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    }
  } else {
    IRCInterface_ErrorDialog("Comando mal introducido. Formato: (/whowas #<canal>)");
  }
  IRC_MFree(2, &msg, &target);
}

void server_out_command_motd(char* command){
  char *msg = NULL, *target = NULL;
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send motd %s", command);
  if(IRCUserParse_Motd(command, &target)==IRC_OK){
    if(IRCMsg_Motd(&msg, NULL, target)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)>0){
        syslog(LOG_INFO, "[CLIENTE] Motd envia: %s", msg);
        IRCInterface_PlaneRegisterOutMessage(msg);
      }
    }
  } else {
    IRCInterface_ErrorDialog("Comando mal introducido. Formato: (/motd #<servidor>)");
  }
  IRC_MFree(2, &msg, &target);
}
