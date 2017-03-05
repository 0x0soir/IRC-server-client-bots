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
  if(IRCParse_Nick(command, &prefix, &nick, &msg)!=IRC_OK){
    if(IRCMsg_ErrNoNickNameGiven(&msg, prefix, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
  } else if(strlen(nick) > 9){
    if(IRCMsg_ErrErroneusNickName(&msg, prefix, nick, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
  } else if(server_users_find_by_nick(nick)==true){
    if(IRCMsg_ErrNickNameInUse(&msg, prefix, nick, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
  } else {
    if(IRCMsg_Nick(&command, prefix, nick, msg)==IRC_OK){
      syslog(LOG_INFO, "NICK: OK");
      send(desc, command, strlen(command), 0);
      strcpy(nick_static, nick);
      *register_status = 1;
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
  char *user = NULL, *realname = NULL, *prefix = NULL, *host = NULL, *IP = NULL;
  syslog(LOG_INFO, "-----> EXECUTE USER: %s", nick_static);
  if((*register_status)==1){
    syslog(LOG_INFO, "-----> EXECUTE USER: %s", nick_static);
    if(IRCParse_User(command, &prefix, &user, &host, &IP, &realname)==IRC_OK){
      if (IRCTADUser_New(user, nick_static, realname, NULL, "localhost", IP, desc)==IRC_OK){
        syslog(LOG_INFO, "USER CREADO: SOCKET %d", desc);
        if(IRCMsg_RplWelcome(&command, "ip.servidor", nick_static, nick_static, user, "localhost")==IRC_OK){
          send(desc, command, strlen(command), 0);
          syslog(LOG_INFO, "USER: OK");
          *register_status = 2;
        }
      }
    }
  }
  if(user){
    free(user);
    user = NULL;
  }
  if(realname){
    free(realname);
    realname = NULL;
  }
  if(host){
    free(host);
    host = NULL;
  }
  if(IP){
    free(IP);
    IP = NULL;
  }
  if(prefix){
    free(prefix);
    prefix = NULL;
  }
}

/*
  JOIN
  Comprobar si el comando esta parseado bien.
    -> Comprobar si el canal existe, si existe se hace join, si no, se crea como "o"
*/
void server_command_function_join(char* command, int desc, char * nick_static, int* register_status){
  char *key = NULL, *prefix = NULL, *msg = NULL, *channel = NULL, *unknown_user = NULL, *unknown_nick = NULL, *unknown_real = NULL, *server = NULL;
  long unknown_id = 0, find_id = 0, find_creationTS, find_actionTS, creationTS, actionTS, numberOfUsers;
  char *host, *IP, *away, *modeFlag, **arrayNicks;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int i, find_socket = 0;
  if (IRCParse_Join(command, &prefix, &channel, &key, &msg) != IRC_OK){
  	IRCMsg_ErrNeedMoreParams(&msg, "ip.servidor", nick_static, command);
  	send(desc, msg, strlen(msg), 0);
    if(msg)
      free(msg);
    if(key)
      free(key);
  } else {
    if(server_channels_find_by_name(channel)==0){
      modeFlag = "o";
    }
    if(IRCTAD_Join(channel, nick_static, modeFlag, key)==IRC_OK){
      if(IRCTADUser_GetData(&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, &desc, &creationTS, &actionTS, &away)==IRC_OK){
        syslog(LOG_INFO, "JOIN: %s %s", server, host);
        if(IRC_Prefix(&prefix, nick_static, unknown_user, NULL, "LOCALHOST")==IRC_OK){
          syslog(LOG_INFO, "JOIN PREFIX: %s", prefix);
      		if(IRCMsg_Join(&command, prefix + 1, NULL, key, channel)==IRC_OK){
            /* Enviar aviso a todos los usuarios */
            IRCTAD_ListNicksOnChannelArray(channel, &arrayNicks, &numberOfUsers);
            for(i=0; i< numberOfUsers; i++){
              if(IRCTADUser_GetData(&find_id, &find_user, &arrayNicks[i], &find_real, &find_host, &find_ip,
                &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
                send(find_socket, command, strlen(command), 0);
                find_user = NULL;
                find_real = NULL;
                find_ip = NULL;
                find_host = NULL;
                find_away = NULL;
                find_socket = 0;
                find_id = 0;
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
          }
        }
      }
      if(unknown_user){
        free(unknown_user);
      }
      if(unknown_nick){
        free(unknown_nick);
      }
      if(unknown_real){
        free(unknown_real);
      }
      if(server){
        free(server);
      }
      if(host){
        free(host);
      }
      if(IP){
        free(IP);
      }
      if(away){
        free(away);
      }
    }
    /*if(modeFlag){
      free(modeFlag);
      modeFlag = NULL;
    }*/
    if(prefix){
      free(prefix);
      prefix = NULL;
    }
    if(channel){
      free(channel);
      channel = NULL;
    }
  }
}

/* Msg: Almacena el texto de quit del usuario */
void server_command_function_quit(char* command, int desc, char * nick_static, int* register_status){
  char *user = NULL, *realname = NULL, *prefix = NULL, *host = NULL, *IP = NULL, *msg = NULL;
  syslog(LOG_INFO, "-----> EXECUTE QUIT: %s", nick_static);
  if((*register_status)==2){
    if(IRCParse_Quit(command, &prefix, &msg)==IRC_OK){
      IRCTAD_Quit(nick_static);
      if(IRCMsg_Quit(&command, prefix, msg)==IRC_OK){
        send(desc, command, strlen(command), 0);
        syslog(LOG_INFO, "MSG: %s", command);
        *register_status = 0;
      }
    }
  }
  if(user){
    free(user);
    user = NULL;
  }
  if(realname){
    free(realname);
    realname = NULL;
  }
  if(host){
    free(host);
    host = NULL;
  }
  if(IP){
    free(IP);
    IP = NULL;
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

void server_command_function_ping(char* command, int desc, char * nick_static, int* register_status){
  char *ping = NULL, *pong = NULL, *prefix = NULL, *msg = NULL;
  if(IRCParse_Ping(command, &prefix, &ping, &pong, &msg)==IRC_OK){
  	if(IRCMsg_Pong(&command, "ip.servidor", ping, pong, ping)==IRC_OK){
  		send(desc, command, strlen(command), 0);
  	}
    free(ping);
    free(pong);
    free(prefix);
    free(msg);
  }
}

void server_command_function_list(char* command, int desc, char * nick_static, int* register_status){
  char *ping = NULL, *pong = NULL, *prefix = NULL, *msg = NULL, **channels, *topic, visible[50];
  long size;
  int i;
  IRCTADChan_GetList(&channels, &size, NULL);
  if(IRCMsg_RplListStart(&msg, "ip.servidor", nick_static)==IRC_OK){
    send(desc, msg, strlen(msg), 0);
    free(msg);
    for (i = 0; i < size; i++) {
      if (IRCTADChan_GetModeInt(channels[i])!=IRCMODE_SECRET) {
        if(IRCTAD_GetTopic(channels[i], &topic)==IRC_OK){
          sprintf(visible, "%ld", IRCTADChan_GetNumberOfUsers(channels[i]));
          IRCMsg_RplList(&msg, "ip.servidor", nick_static, channels[i], visible, topic);
          send(desc, msg, strlen(msg), 0);
          free(msg);
        }
      }
    }
    if(IRCMsg_RplListEnd(&msg, "ip.servidor", nick_static)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      free(msg);
    }
  }
}
