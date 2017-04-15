#include "../includes/G-2313-06-P2_client_function_handlers.h"

extern int socket_desc;
extern char* nick_cliente;

/* IN FUNCTIONS */
void server_in_command_nick(char* command, int desc, char * nick_static, int* register_status){
  char *prefix, *nick, *msg, *msgEnvio[150];
  IRCInterface_PlaneRegisterInMessageThread(command);
  if(IRCParse_Nick(command, &prefix, &nick, &msg)==IRC_OK){
    if(!nick_cliente){
      syslog(LOG_INFO, "[CLIENTE] No hay puntero reservado ");
      nick_cliente = malloc(sizeof(msg));
      strcpy(nick_cliente, msg);
    }
    sprintf(msgEnvio, "Ahora tu nick es: %s", msg);
    syslog(LOG_INFO, "[CLIENTE] NICK OLD: %s NEW: %s", nick_cliente, msg);
    IRCInterface_WriteSystemThread(NULL, msgEnvio);
    IRCInterface_ChangeNick(nick_cliente, msg);
    if(nick_cliente){
      free(nick_cliente);
      nick_cliente = NULL;
      nick_cliente = malloc(sizeof(msg));
      strcpy(nick_cliente, msg);
    }
    syslog(LOG_INFO, "[CLIENTE] NICK NEW: %s", nick_cliente);
  }
  IRC_MFree(3, &prefix, &nick, &msg);
}

void server_in_command_pong(char* command, int desc, char * nick_static, int* register_status){
  IRCInterface_PlaneRegisterInMessageThread(command);
}

void server_in_command_join(char* command, int desc, char * nick_static, int* register_status){
  char *prefix, *channel, *key, *msg, *msgEnvio[150], *join_nick, *join_user, *join_host, *join_server;
  long mode = 1L;
  IRCInterface_PlaneRegisterInMessageThread(command);
  if(IRCParse_Join(command, &prefix, &channel, &key, &msg)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] [IN]: JOIN PREFIX %s", prefix);
    if(IRCParse_ComplexUser(prefix, &join_nick, &join_user, &join_host, &join_server)==IRC_OK){
      if(strcmp(join_nick, nick_cliente)==0){
        sprintf(msgEnvio, "Ahora estás hablando en %s", msg);
      } else {
        sprintf(msgEnvio, "%s (~%s@%s) ha entrado %s", join_nick, join_nick, join_server, msg);
      }
      if(IRCInterface_QueryChannelExistThread(msg)!=0){
        IRCInterface_AddNickChannel(msg, join_nick, join_user, join_user, join_server, 0);
        syslog(LOG_INFO, "[CLIENTE] [IN]: Canal existente, añado nick");
      } else {
        IRCInterface_AddNewChannelThread(msg, 0);
        IRCInterface_AddNickChannel(msg, join_nick, join_user, join_user, join_server, 0);
        syslog(LOG_INFO, "[CLIENTE] [IN]: Canal no existente, lo creo");
        /*strcpy(msgEnvio, msg);*/
        /*pthread_create(&thread1, NULL, func_who, (void *) buff);*/
      }
      IRCInterface_WriteChannelThread(msg, NULL, msgEnvio);
    } else {
      syslog(LOG_INFO, "[CLIENTE] [IN]: Error en Parse Complex");
    }
  }
  IRC_MFree(8, &prefix, &channel, &key, &msg, &join_nick, &join_user, &join_host, &join_server);
}

void server_in_command_part(char* command, int desc, char * nick_static, int* register_status){
  char *prefix, *channel, *msg, *msgEnvio[150], *part_nick, *part_user, *part_host, *part_server;
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
    } else {
      syslog(LOG_INFO, "[CLIENTE] [IN]: Error en Parse Complex");
    }
  }
  IRC_MFree(8, &prefix, &channel, &msg, &part_nick, &part_user, &part_host, &part_server);
}

void server_in_command_mode(char* command, int desc, char * nick_static, int* register_status){
  char *prefix, *channel, *msg, *msgEnvio[150], *mode, *mode_nick, *mode_user, *mode_host, *mode_server, *user_target;
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
        sprintf(buff, "%s establece el modo +m %s", mode_nick, channel);
        IRCInterface_WriteChannelThread(channel, "*", buff);
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
        IRCInterface_ChangeNickStateChannelThread(channel, user_target, VOICE);
      }
    } else {
      syslog(LOG_INFO, "[CLIENTE] [IN]: Error en Parse Complex");
    }
  }
  IRC_MFree(8, &prefix, &channel, &msg, &mode_nick, &mode_user, &mode_host, &mode_server);
}

/* OUT FUNCTIONS */
void server_out_command_nick(char* command, int desc, char * nick_static, int* register_status){
  char *newNick, *msg;
  syslog(LOG_INFO, "[CLIENTE] Send nick %s", command);
  if(IRCUserParse_Nick(command, &newNick)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] NICK PARSE OK");
    IRCMsg_Nick(&msg, NULL, newNick, NULL);
    if(send(socket_desc, msg, strlen(msg), 0) < 0){
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

void server_out_command_join(char* command, int desc, char * nick_static, int* register_status){
  char *msg, *channel, *password;
  syslog(LOG_INFO, "[CLIENTE] Send join %s", command);
  if(IRCUserParse_Join(command, &channel, &password)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] JOIN PARSE OK");
    IRCMsg_Join(&msg, NULL, channel, password, NULL);
    if(send(socket_desc, msg, strlen(msg), 0) < 0){
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
  IRC_MFree(3, &msg, &channel, &password);
}

void server_out_command_names(char* command, int desc, char * nick_static, int* register_status){
  char *msg, *channel, *targetserver;
  syslog(LOG_INFO, "[CLIENTE] Send join %s", command);
  if(IRCUserParse_Names(command, &channel, &targetserver)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] NAMES PARSE OK");
    IRCMsg_Names(&msg, NULL, channel, targetserver);
    if(send(socket_desc, msg, strlen(msg), 0) < 0){
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
}

void server_out_command_list(char* command, int desc, char * nick_static, int* register_status){
  char *msg, *channel, *search;
  syslog(LOG_INFO, "[CLIENTE] Send join %s", command);
  if(IRCUserParse_List(command, &channel, &search)==IRC_OK){
    syslog(LOG_INFO, "[CLIENTE] LIST PARSE OK");
    IRCMsg_List(&msg, NULL, channel, search);
    if(send(socket_desc, msg, strlen(msg), 0) < 0){
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
  IRC_MFree(3, &msg, &channel, &search);
}

void server_out_command_part(char* command, int desc, char * nick_static, int* register_status){
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
    if(send(socket_desc, msg, strlen(msg), 0) < 0){
      syslog(LOG_INFO, "[CLIENTE] Part envia: %s", msg);
      IRCInterface_PlaneRegisterOutMessage(msg);
    }
  }
  IRC_MFree(1, &msg);
  IRC_MFree(1, &channel);
}

void server_out_command_mode(char* command, int desc, char * nick_static, int* register_status){
  char *msg = NULL, *channelActual = NULL, *mode = NULL, *filter = NULL;
  channelActual = IRCInterface_ActiveChannelName();
  syslog(LOG_INFO, "[CLIENTE] [OUT] Send mode %s", command);
  if(IRCUserParse_Mode(command, &mode, &filter)==IRC_OK){
    if(IRCMsg_Mode(&msg, NULL, channelActual, mode, NULL)==IRC_OK){
      if(send(socket_desc, msg, strlen(msg), 0)==0){
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
