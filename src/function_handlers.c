#include "function_handlers.h"

/*
Errores:
  - 431 No nickname given
  - 432 %s: Erroneus nickname
  - 433 %s: Nickname already in used
Correcto:
  - %s!127.0.0.1 NICK %s\r\n
*/
int server_command_function_nick(char* command, int desc){
  char *nick, *prefix, *msg;
  if(IRCParse_Nick(command, &prefix, &nick, &msg)!=IRC_OK){
    if(IRCMsg_ErrNoNickNameGiven(&msg, prefix, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      return -1;
    }
  } else if(strlen(nick) > 9){
    if(IRCMsg_ErrErroneusNickName(&msg, prefix, nick, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      return -1;
    }
  } else if (server_users_find(0, nick, TRUE)!=0){
    if(IRCMsg_ErrNickNameInUse(&msg, prefix, nick, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      return -1;
    }
  } else {
    if (IRCTADUser_New("user", nick, "realname", NULL, "host", "ip", desc)==IRC_OK){
      if(IRCMsg_Nick(&command, prefix, nick, msg)==IRC_OK){
        send(desc, command, strlen(command), 0);
        return 0;
      }
    }
  }
  /* Comprobar si usuario esta siendo usado por otro*/
  return 0;
}

int server_command_function_user(char* command, int desc){
  char *unknown_user = NULL, *unknown_nick = NULL, *unknown_real = NULL;
  long unknown_id = 0;
  char *host, *IP, *away;
  long creationTS, actionTS;
  if(IRCTADUser_GetData(&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, &desc, &creationTS, &actionTS, &away)==IRC_OK){
    syslog(LOG_INFO, "USER: USUARIO OBTENIDO CORRECTAMENTE: %s", unknown_nick);
  }
  free(unknown_user);
  free(unknown_nick);
  return 0;
}
