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
      syslog(LOG_INFO, "USER: HOST: %s", host);
      if (IRCTADUser_New(user, nick_static, realname, NULL, "localhost", IP, desc)==IRC_OK){
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

void server_command_function_join(char* command, int desc, char * nick_static, int* register_status){
  char *key = NULL, *prefix = NULL, *msg = NULL, *channel = NULL, *unknown_user = NULL, *unknown_nick = NULL, *unknown_real = NULL, *server = NULL;
  long unknown_id = 0;
  char *host, *IP, *away;
  long creationTS, actionTS;
  if (IRCParse_Join(command, &prefix, &channel, &key, &msg) != IRC_OK){
  	syslog(LOG_INFO, "JOIN: ERROR EN PARSE");
  	IRCMsg_ErrNeedMoreParams(&msg, "ip.servidor", nick_static, command);
  	send(desc, msg, strlen(msg), 0);
  } else {
    if(IRCTAD_Join(channel, nick_static, "o", key)==IRC_OK){
      if(IRCTADUser_GetData(&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, &desc, &creationTS, &actionTS, &away)==IRC_OK){
        syslog(LOG_INFO, "JOIN: %s %s", server, host);
        if(IRC_Prefix(&prefix, nick_static, unknown_user, "localhost", "LOCALHOST")==IRC_OK){
      		if(IRCMsg_Join(&command, "ip.servidor", NULL, key, channel)==IRC_OK){
        		send(desc, command, strlen(command), 0);
            syslog(LOG_INFO, "MSG: %s", command);
          }
        }
      }
    }
    if(key){
      free(key);
      key = NULL;
    }
    if(prefix){
      free(prefix);
      prefix = NULL;
    }
    if(msg){
      free(msg);
      msg = NULL;
    }
    if(channel){
      free(channel);
      channel = NULL;
    }
    if(unknown_user){
      free(unknown_user);
      unknown_user = NULL;
    }
    if(unknown_nick){
      free(unknown_nick);
      unknown_nick = NULL;
    }
    if(unknown_real){
      free(unknown_real);
      unknown_real = NULL;
    }
    if(server){
      free(server);
      server = NULL;
    }
    if(host){
      free(host);
      host = NULL;
    }
    if(IP){
      free(IP);
      IP = NULL;
    }
    if(away){
      free(away);
      away = NULL;
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
