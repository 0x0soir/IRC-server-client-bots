#include "../includes/function_handlers.h"

/*
Errores:
  - 431 No nickname given
  - 432 %s: Erroneus nickname
  - 433 %s: Nickname already in used
Correcto:
  - %s!127.0.0.1 NICK %s\r\n
*/
void server_command_function_nick(char* command, int desc, char * nick_static, int* register_status){
  char *nick = NULL, *prefix = NULL, *msg = NULL;
  int response;
  if(IRCParse_Nick(command, &prefix, &nick, &msg)!=IRC_OK){
    if(IRCMsg_ErrNoNickNameGiven(&msg, prefix, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
  } else if(strlen(nick) > 9){
    if(IRCMsg_ErrErroneusNickName(&msg, prefix, nick, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
  } else if((response=server_users_find_by_nick(nick))==true){
    if(IRCMsg_ErrNickNameInUse(&msg, prefix, nick, nick)==IRC_OK){
      syslog(LOG_INFO, "[DESC: %d] NICK: %s ya se está usando, idiota (Valor de control: %d)", desc, nick, *register_status);
      send(desc, msg, strlen(msg), 0);
    }
  } else {
    if(*register_status==2){
      response = server_channels_update_nick(nick_static, nick);
      if(response==0){
        if(IRCMsg_Nick(&msg, "ip.servidor", NULL, nick)==IRC_OK){
          syslog(LOG_INFO, "[DESC: %d] NICK: UPDATED OK", desc);
          send(desc, msg, strlen(msg), 0);
          strcpy(nick_static, nick);
        }
      }
    } else {
      if(IRCMsg_Nick(&msg, "ip.servidor", NULL, nick)==IRC_OK){
        syslog(LOG_INFO, "[DESC: %d] NICK: OK", desc);
        send(desc, msg, strlen(msg), 0);
        strcpy(nick_static, nick);
        *register_status = 1;
      }
    }
  }
  /* Comprobar si usuario esta siendo usado por otro*/
  if(nick){
    free(nick);
    nick = NULL;
  }
  if(prefix){
    free(prefix);
    prefix = NULL;
  }
  if(msg){
    free(msg);
    msg = NULL;
  }
}

void server_command_function_user(char* command, int desc, char * nick_static, int* register_status){
  char *user = NULL, *realname = NULL, *prefix = NULL, *host = NULL, *IP = NULL, *msg = NULL;
  struct sockaddr_in addr;
  struct hostent *he;
  int sclient;
  sclient = sizeof(addr);
  getpeername(desc, (struct sockaddr *)&addr, &sclient);
  IP = inet_ntoa(addr.sin_addr);
  he = gethostbyaddr((char *)&addr.sin_addr, sizeof(addr.sin_addr), AF_INET);
  if((*register_status)==1){
    syslog(LOG_INFO, "[DESC: %d] -----> EXECUTE USER: %s", desc, nick_static);
    syslog(LOG_INFO, "[DESC: %d] USER: Valor de control previo %d", desc, *register_status);
    if(IRCParse_User(command, &prefix, &user, &host, &IP, &realname)==IRC_OK){
      if (IRCTADUser_New(user, nick_static, realname, NULL, he->h_name, IP, desc)==IRC_OK){
        if(IRCMsg_RplWelcome(&msg, "ip.servidor", nick_static, nick_static, user, "localhost")==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          IRC_MFree(1, &msg);
          *register_status = 2;
        }
      }
    }
  }
  syslog(LOG_INFO, "[DESC: %d] EXECUTE USER: Valor de control posterior %d", desc, *register_status);
  IRC_MFree(6, &user, &realname, &prefix, &host, &IP, &msg);
}

/*
  JOIN
  Comprobar si el comando esta parseado bien.
    -> Comprobar si el canal existe, si existe se hace join, si no, se crea como "o"
*/
void server_command_function_join(char* command, int desc, char * nick_static, int* register_status){
  char *key = NULL, *prefix = NULL, *msg = NULL, *channel = NULL, *unknown_user = NULL, *unknown_nick = NULL, *unknown_real = NULL, *server = NULL;
  long unknown_id = 0, find_id = 0, find_creationTS, find_actionTS, creationTS, actionTS, numberOfUsers;
  char *host, *IP, *away, *modeFlag = malloc(sizeof(char)*2), **arrayNicks;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int i, find_socket = 0, response, logic_comparator, flag_login_possible = 0;
  if (IRCParse_Join(command, &prefix, &channel, &key, &msg) != IRC_OK){
  	IRCMsg_ErrNeedMoreParams(&msg, "ip.servidor", nick_static, command);
  	send(desc, msg, strlen(msg), 0);
  } else {
    if((response=server_channels_find_by_name(channel))==0){
      strcpy(modeFlag, "o");
    }
    logic_comparator = IRCTADChan_GetModeInt(channel);
    if((logic_comparator&IRCMODE_CHANNELPASSWORD)==IRCMODE_CHANNELPASSWORD){
      syslog(LOG_INFO, "JOIN: NECESITA CLAVE");
      if(IRCTADChan_TestPassword(channel, key)!=IRC_OK){
        if(IRCMsg_ErrBadChannelKey(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
        }
        IRC_MFree(1, &msg);
      } else {
        syslog(LOG_INFO, "JOIN: CLAVE CORRECTA");
        flag_login_possible = 1;
      }
    } else {
      flag_login_possible = 1;
    }
    if(flag_login_possible){
      syslog(LOG_INFO, "JOIN: HACE JOIN EN CANAL: %s CON CLAVE: %s", channel, key);
      if(IRCTAD_Join(channel, nick_static, modeFlag, key)==IRC_OK){
        if(IRCTADUser_GetData(&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, &desc, &creationTS, &actionTS, &away)==IRC_OK){
          syslog(LOG_INFO, "JOIN: %s %s", server, host);
          if(IRC_Prefix(&prefix, nick_static, unknown_user, NULL, "LOCALHOST")==IRC_OK){
            syslog(LOG_INFO, "JOIN PREFIX: %s", prefix);
        		if(IRCMsg_Join(&msg, prefix + 1, NULL, NULL, channel)==IRC_OK){
              /* Enviar aviso a todos los usuarios */
              IRCTAD_ListNicksOnChannelArray(channel, &arrayNicks, &numberOfUsers);
              for(i=0; i< numberOfUsers; i++){
                if(IRCTADUser_GetData(&find_id, &find_user, &arrayNicks[i], &find_real, &find_host, &find_ip,
                  &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
                  send(find_socket, msg, strlen(msg), 0);
                  IRC_MFree(5, &find_user, &find_real, &find_host, &find_ip, &find_away);
                  find_socket = 0;
                  find_id = 0;
                }
              }
              IRCTADChan_FreeList(arrayNicks, numberOfUsers);
            }
            IRC_MFree(1, &msg);
          }
        }
        IRC_MFree(5, &unknown_user, &unknown_nick, &unknown_real, &server, &host, &IP, &away);
      }
    }
  }
  IRC_MFree(5, &msg, &key, &modeFlag, &prefix, &channel);
}

/* Msg: Almacena el texto de quit del usuario */
void server_command_function_quit(char* command, int desc, char * nick_static, int* register_status){
  char *user = NULL, *realname = NULL, *prefix = NULL, *host = NULL, *IP = NULL, *msg = NULL, *msg2 = NULL;
  syslog(LOG_INFO, "-----> EXECUTE QUIT: %s", nick_static);
  if(IRCParse_Quit(command, &prefix, &msg2)==IRC_OK){
    IRCTAD_Quit(nick_static);
    if(IRCMsg_Quit(&msg, prefix, msg2)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      syslog(LOG_INFO, "MSG: %s", msg);
      close(desc);
    }
  }
  *register_status = 0;
  IRC_MFree(7, &user, &realname, &host, &IP, &prefix, &msg, &msg2);
}

void server_command_function_ping(char* command, int desc, char * nick_static, int* register_status){
  char *ping = NULL, *pong = NULL, *prefix = NULL, *msg = NULL;
  if(IRCParse_Ping(command, &prefix, &ping, &pong, &msg)==IRC_OK){
  	if(IRCMsg_Pong(&command, "ip.servidor", ping, pong, ping)==IRC_OK){
  		send(desc, command, strlen(command), 0);
  	}
  }
  IRC_MFree(4, &ping, &pong, &prefix, &msg);
}

void server_command_function_list(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, **channels = NULL, *topic = NULL, visible[50];
  long size;
  int i;
  strcpy(visible, "");
  IRCTADChan_GetList(&channels, &size, NULL);
  if(IRCMsg_RplListStart(&msg, "ip.servidor", nick_static)==IRC_OK){
    send(desc, msg, strlen(msg), 0);
    for (i = 0; i < size; i++) {
      if (IRCTADChan_GetModeInt(channels[i])!=IRCMODE_SECRET) {
        if(IRCTAD_GetTopic(channels[i], &topic)==IRC_OK){
          sprintf(visible, "%ld", IRCTADChan_GetNumberOfUsers(channels[i]));
          IRCMsg_RplList(&msg, "ip.servidor", nick_static, channels[i], visible, topic);
          send(desc, msg, strlen(msg), 0);
          IRC_MFree(1, &msg);
        }
      }
    }
    if(IRCMsg_RplListEnd(&msg, "ip.servidor", nick_static)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
  }
  IRCTADChan_FreeList(channels, size);
  IRC_MFree(3, &prefix, &msg, &topic);
}

void server_command_function_privmsg(char* command, int desc, char *nick_static, int* register_status){
  char *prefix = NULL, *msgtarget = NULL, *msg = NULL, *awayMsg = NULL, *msgSend = NULL, *msgSend2 = NULL;
  long find_id = 0, find_creationTS, find_actionTS, numberOfUsers;
  char **arrayNicks;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int i, find_socket = 0;
  if(IRCParse_Privmsg(command, &prefix, &msgtarget, &msg)==IRC_OK){
    if(msgtarget[0]=='#'){
      /* Es canal */
      IRCTAD_ListNicksOnChannelArray(msgtarget, &arrayNicks, &numberOfUsers);
      for(i=0; i< numberOfUsers; i++){
        if(strcmp(nick_static, arrayNicks[i])!=0){
          if(IRCTADUser_GetData(&find_id, &find_user, &arrayNicks[i], &find_real, &find_host, &find_ip, &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
            if(IRC_Prefix(&prefix, arrayNicks[i], find_user, NULL, "LOCALHOST")==IRC_OK){
              if(IRCMsg_Privmsg(&msgSend, prefix+1, msgtarget, msg)==IRC_OK){
                syslog(LOG_INFO, "-----> EXECUTE PRIVMSG: Socket: %d", find_socket);
                send(find_socket, msgSend, strlen(msgSend), 0);
              }
              if(find_user){
                free(find_user);
                find_user = NULL;
              }
              if(find_real){
                free(find_real);
                find_real = NULL;
              }
              if(find_host){
                free(find_host);
                find_host = NULL;
              }
              if(find_ip){
                free(find_ip);
                find_ip = NULL;
              }
              if(find_away){
                free(find_away);
                find_away = NULL;
              }
              if(msgSend){
                free(msgSend);
                msgSend = NULL;
              }
              find_socket = find_id = 0;
            }
          }
        }
      }
      IRCTADChan_FreeList(arrayNicks, numberOfUsers);
    } else {
      /* Es usuario */
      if(IRCTADUser_GetData(&find_id, &find_user, &msgtarget, &find_real, &find_host, &find_ip, &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
        if(IRC_Prefix(&prefix, nick_static, find_user, NULL, "LOCALHOST")==IRC_OK){
          if(IRCMsg_Privmsg(&msgSend2, prefix+1, msgtarget, msg)==IRC_OK){
            if(IRCTADUser_GetAway(0, NULL, msgtarget, NULL, &awayMsg)==IRC_OK){
              if(awayMsg!=NULL){
                if(IRCMsg_RplAway(&msgSend, "ip.servidor", nick_static, msgtarget, awayMsg)==IRC_OK){
                  send(desc, msgSend, strlen(msgSend), 0);
                }
                if(msgSend){
                  free(msgSend);
                  msgSend = NULL;
                }
              }
            }
            syslog(LOG_INFO, "-----> EXECUTE PRIVMSG: Socket: %d", find_socket);
            send(find_socket, msgSend2, strlen(msgSend2), 0);
            if(find_user){
              free(find_user);
              find_user = NULL;
            }
            if(find_real){
              free(find_real);
              find_real = NULL;
            }
            if(find_host){
              free(find_host);
              find_host = NULL;
            }
            if(find_ip){
              free(find_ip);
              find_ip = NULL;
            }
            if(find_away){
              free(find_away);
              find_away = NULL;
            }
            if(prefix){
              free(prefix);
              prefix = NULL;
            }
            if(msgSend2){
              free(msgSend2);
              msgSend2 = NULL;
            }
            if(awayMsg){
              free(awayMsg);
              awayMsg = NULL;
            }
            find_user = find_real = find_host = find_away = prefix = NULL;
            find_socket = find_id = 0;
          }
        }
      } else {
        IRCMsg_ErrNoSuchNick(&msgSend, "ip.servidor", nick_static, msgtarget);
        send(desc, msgSend, strlen(msgSend), 0);
        if(msgSend){
          free(msgSend);
          msgSend = NULL;
        }
      }
    }
  }
  if(prefix){
    free(prefix);
    prefix = NULL;
  }
  if(msgtarget){
    free(msgtarget);
    msgtarget = NULL;
  }
  if(msg){
    free(msg);
    msg = NULL;
  }
}

void server_command_function_part(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, **channels, *target_channel;
  long size;
  syslog(LOG_INFO, "-----> EXECUTE PART: %s", command);
  if(IRCParse_Part(command, &prefix, &target_channel, &msg)==IRC_OK){
    IRCTADChan_GetList(&channels, &size, NULL);
  	if(size < 1) {
      IRCMsg_ErrNoSuchChannel(&msg, "ip.servidor", nick_static, target_channel);
      send(desc, msg, strlen(msg), 0);
      free(msg);
    } else {
      if(IRCTAD_Part(target_channel, nick_static)==IRC_OK){
        if(IRCMsg_Part(&msg, "ip.servidor", target_channel, msg)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
        }
      } else {
        IRCMsg_ErrNoSuchChannel(&msg, "ip.servidor", nick_static, target_channel);
        send(desc, msg, strlen(msg), 0);
        free(msg);
      }
    }
    IRCTADChan_FreeList(channels, size);
  }
}

void server_command_function_names(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *channel, *target, **arrayNicks, string_nicks[100], type[7] = "=";
  long numberOfUsers;
  int i;
  syslog(LOG_INFO, "-----> EXECUTE NAMES: %s", command);
  memset(string_nicks, 0, 100);
  if(IRCParse_Names(command, &prefix, &channel, &target)==IRC_OK){
    if(IRCTAD_ListNicksOnChannelArray(channel, &arrayNicks, &numberOfUsers)==IRC_OK){
      for(i = 0; i < numberOfUsers; i++) {
        if (i > 0){
          strcat(string_nicks, " ");
        }
        if((IRCTAD_GetUserModeOnChannel(channel, arrayNicks[i])&2)==2){
          strcat(string_nicks, "@");
        }
        strcat(string_nicks, arrayNicks[i]);
      }
      IRCTADChan_FreeList(arrayNicks, numberOfUsers);
    }
    if(strcmp(string_nicks, "")!=0){
      if(IRCMsg_RplNamReply(&msg, "ip.servidor", nick_static, type, channel, string_nicks)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
        free(msg);
      }
    }
    if(IRCMsg_RplEndOfNames(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      free(msg);
    }
  }
}

void server_command_function_kick(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *channel, *nick, *comment;
  long response, find_id = 0, find_creationTS, find_actionTS;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int find_socket = 0;
  syslog(LOG_INFO, "-----> EXECUTE KICK: %s", command);
  if(IRCParse_Kick(command, &prefix, &channel, &nick, &comment)==IRC_OK){
    if((IRCTAD_GetUserModeOnChannel(channel, nick_static)&2)==2){
      response = IRCTAD_KickUserFromChannel(channel, nick);
      if(response==IRC_OK){
        if(IRCTADUser_GetData(&find_id, &find_user, &nick, &find_real, &find_host, &find_ip, &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
          if(IRCMsg_Kick(&msg, "ip.servidor", channel, nick, comment)==IRC_OK){
            send(find_socket, msg, strlen(msg), 0);
            free(msg);
          }
        }
        if(find_user)
          free(find_user);
        if(find_real)
          free(find_real);
        if(find_host)
          free(find_host);
        if(find_ip)
          free(find_ip);
        if(find_away)
          free(find_away);
        if(prefix)
          free(prefix);
        find_user = find_real = find_host = find_away = NULL;
        find_socket = find_id = 0;
      } else if(response==IRCERR_NOVALIDUSER){
        if(IRCMsg_ErrNoLogin(&msg, "ip.servidor", nick_static, nick)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
        }
      } else if(response==IRCERR_NOVALIDCHANNEL){
        if(IRCMsg_ErrNoSuchChannel(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
        }
      }
    } else {
      if(IRCMsg_ErrChanOPrivsNeeded(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
        free(msg);
      }
    }
  }
}

void server_command_function_mode(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *channel = NULL, *mode = NULL, *user = NULL, *mode_arg = NULL;
  long response = 0, logic_comparator;
  syslog(LOG_INFO, "-----> EXECUTE MODE: %s", command);
  if((response = IRCParse_Mode(command, &prefix, &channel, &mode, &user))==IRC_OK){
    if(mode){
      if((strcmp(mode, "+s")==0)||(strcmp(mode, "+t")==0)){
        if(IRCTAD_Mode(channel, nick_static, mode)==IRC_OK){
          if(IRCMsg_Mode(&msg, "ip.servidor", channel, mode, user)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
            free(msg);
            msg = NULL;
          }
        }
      } else if(strcmp(mode, "\\+k")==0){
        mode_arg = malloc(sizeof(user)+6);
        strcpy(mode_arg, "");
        strcat(mode_arg, "+k ");
        strcat(mode_arg, user);
        if(IRCTAD_Mode(channel, nick_static, mode_arg)==IRC_OK){
          if(IRCMsg_Mode(&msg, "ip.servidor", channel, mode, user)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
            free(msg);
            msg = NULL;
          }
        }
      } else {
        if(IRCMsg_ErrUnknownMode(&msg, "ip.servidor", nick_static, mode, channel)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
          msg = NULL;
        }
      }
    } else {
      logic_comparator = IRCTADChan_GetModeInt(channel);
      if((logic_comparator&IRCMODE_TOPICOP)==IRCMODE_TOPICOP){
        if(IRCMsg_RplChannelModeIs(&msg, "ip.servidor", nick_static, channel, "+t")==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
          msg = NULL;
        }
      } else if((logic_comparator&IRCMODE_SECRET)==IRCMODE_SECRET){
        if(IRCMsg_RplChannelModeIs(&msg, "ip.servidor", nick_static, channel, "+s")==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
          msg = NULL;
        }
      } else if((logic_comparator&IRCMODE_CHANNELPASSWORD)==IRCMODE_CHANNELPASSWORD){
        if(IRCMsg_RplChannelModeIs(&msg, "ip.servidor", nick_static, channel, "\\+k")==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
          msg = NULL;
        }
      }
    }
  } else if (response==IRCERR_NOSTRING){
    syslog(LOG_INFO, "-----> EXECUTE MODE: IRCERR_NOSTRING");
  } else if (response==IRCERR_ERRONEUSCOMMAND){
    syslog(LOG_INFO, "-----> EXECUTE MODE: IRCERR_ERRONEUSCOMMAND");
  }
  if(channel){
    free(channel);
    channel = NULL;
  }
  if(mode){
    free(mode);
    mode = NULL;
  }
  if(user){
    free(user);
    user = NULL;
  }
  if(mode_arg){
    free(mode_arg);
    mode_arg = NULL;
  }
  if(prefix){
    free(prefix);
    prefix = NULL;
  }
}

void server_command_function_away(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *msg_away = NULL;
  syslog(LOG_INFO, "-----> EXECUTE AWAY: %s", command);
  if(IRCParse_Away(command, &prefix, &msg_away)==IRC_OK){
    server_channels_update_away(nick_static, msg_away);
    if(msg_away!=NULL){
      if(IRCMsg_RplNowAway(&msg, "ip.servidor", nick_static)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
      }
      IRC_MFree(1, &msg);
    } else {
      if(IRCMsg_RplUnaway(&msg, "ip.servidor", nick_static)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
      }
      IRC_MFree(1, &msg);
    }
  }
  IRC_MFree(2, &prefix, &msg_away);
}

void server_command_function_whois(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *target = NULL, *maskarray = NULL, *awayMsg = NULL, **arrayChannels = NULL, *stringsWhois = NULL;
  long find_id = 0, find_creationTS, find_actionTS, numberOfChannels = 0;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int find_socket = 0, i;
  syslog(LOG_INFO, "-----> EXECUTE WHOIS: %s", command);
  if(IRCParse_Whois(command, &prefix, &target, &maskarray)==IRC_OK){
    if(IRCTADUser_GetAway(0, NULL, maskarray, NULL, &awayMsg)==IRC_OK){
      if(awayMsg!=NULL){
        if(IRCMsg_RplAway(&msg, "ip.servidor", nick_static, maskarray, awayMsg)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
        }
        if(msg){
          free(msg);
          msg = NULL;
        }
      } else {
        if(IRCTADUser_GetData(&find_id, &find_user, &maskarray, &find_real, &find_host, &find_ip,
          &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
          if(IRCTAD_ListChannelsOfUserArray(find_user, maskarray, &arrayChannels, &numberOfChannels)==IRC_OK){
            syslog(LOG_INFO, "WHOIS: LISTA DE CANALES OK");
            if(IRCMsg_RplWhoIsUser(&msg, "ip.servidor", nick_static, maskarray, find_user, find_host, find_real)==IRC_OK){
              syslog(LOG_INFO, "WHOIS: mensaje ok");
              send(desc, msg, strlen(msg), 0);
            }
            if(msg){
              free(msg);
              msg = NULL;
            }
            stringsWhois = (char *) malloc(numberOfChannels*51*sizeof(char));
            strcpy(stringsWhois, "");

            for(i=0; i<numberOfChannels; i++){
              if(i>0){
                strcat(stringsWhois, " ");
              }
              if((IRCTAD_GetUserModeOnChannel(arrayChannels[i], maskarray)&2)==2){
                strcat(stringsWhois, "@");
              }
              strcat(stringsWhois, arrayChannels[i]);
            }

            if(IRCMsg_RplWhoIsChannels(&msg, "ip.servidor", nick_static, maskarray, stringsWhois)==IRC_OK){
              send(desc, msg, strlen(msg), 0);
            }
            IRC_MFree(1, &msg);

            if(IRCMsg_RplEndOfWhoIs(&msg, "ip.servidor", nick_static, maskarray)==IRC_OK){
              send(desc, msg, strlen(msg), 0);
            }
            IRC_MFree(2, &msg, &stringsWhois);
          } else {
            syslog(LOG_INFO, "WHOIS: ERROR AL OBTENER CANALES");
          }
          IRCTADChan_FreeList(arrayChannels, numberOfChannels);
          IRC_MFree(5, &find_user, &find_real, &find_ip, &find_host, &find_away);
          find_socket = find_id = 0;
          syslog(LOG_INFO, "WHOIS: USUARIO ENCONTRADO");
        }
      }
    }
  } else {
    if(IRCMsg_ErrNoNickNameGiven(&msg, "ip.servidor", nick_static)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
    if(msg){
      free(msg);
      msg = NULL;
    }
  }
  if(target){
    free(target);
    target = NULL;
  }
  if(maskarray){
    free(maskarray);
    maskarray = NULL;
  }
  if(awayMsg){
    free(awayMsg);
    awayMsg = NULL;
  }
}

void server_command_function_topic(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *channel = NULL, *topic = NULL;
  syslog(LOG_INFO, "-----> EXECUTE TOPIC: %s", command);
  if(IRCParse_Topic(command, &prefix, &channel, &topic)==IRC_OK){
    syslog(LOG_INFO, "TOPIC a");
    if(topic&&channel){
      syslog(LOG_INFO, "TOPIC b");
      if((IRCTADChan_GetModeInt(channel)&IRCMODE_TOPICOP)==IRCMODE_TOPICOP){
        if(((IRCTAD_GetUserModeOnChannel(channel, nick_static)&1)==1)||((IRCTAD_GetUserModeOnChannel(channel, nick_static)&2)==2)){
          if(IRCTAD_SetTopic(channel, nick_static, topic)==IRC_OK){
            if(IRCMsg_Topic(&msg, "ip.servidor", channel, topic)==IRC_OK){
              send(desc, msg, strlen(msg), 0);
            }
          }
        } else {
          if(IRCMsg_ErrChanOPrivsNeeded(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
          }
        }
      } else {
        if(IRCTAD_SetTopic(channel, nick_static, topic)==IRC_OK){
          if(IRCMsg_Topic(&msg, "ip.servidor", channel, topic)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
          }
        }
      }
    } else {
      syslog(LOG_INFO, "TOPIC c");
      if(IRCTAD_GetTopic(channel, &topic)==IRC_OK){
        if(!topic){
          if(IRCMsg_RplNoTopic(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
          }
        } else {
          if(IRCMsg_RplTopic(&msg, "ip.servidor", nick_static, channel, topic)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
          }
        }
      }
    }
  }
  IRC_MFree(4, &prefix, &channel, &topic, &msg);
}

void server_command_function_motd(char* command, int desc, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *target = NULL;
  syslog(LOG_INFO, "-----> EXECUTE MOTD: %s", command);
  if(IRCParse_Motd(command, &prefix, &target)==IRC_OK){
    if(IRCMsg_RplMotdStart(&msg, "ip.servidor", nick_static, "¡BIENVENIDO!")==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
    IRC_MFree(1, &msg);

    if(IRCMsg_RplMotd(&msg, "ip.servidor", nick_static, "¡BIENVENIDO!")==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
    IRC_MFree(1, &msg);

    if(IRCMsg_RplEndOfMotd(&msg, "ip.servidor", nick_static)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
    IRC_MFree(1, &msg);
  }
  IRC_MFree(2, &prefix, &target);
}
