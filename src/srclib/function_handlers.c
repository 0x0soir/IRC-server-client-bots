#include "../includes/function_handlers.h"

/*
Errores:
  - 431 No nickname given
  - 432 %s: Erroneus nickname
  - 433 %s: Nickname already in used
Correcto:
  - %s!127.0.0.1 NICK %s\r\n
*/
int server_command_function_nick(char* command, int desc, char * nick_static){
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
  } else if(server_users_find_by_nick(nick)==true){
    if(IRCMsg_ErrNickNameInUse(&msg, prefix, nick, nick)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      return -1;
    }
  } else {
    if(IRCMsg_Nick(&command, prefix, nick, msg)==IRC_OK){
      syslog(LOG_INFO, "NICK: OK");
      send(desc, command, strlen(command), 0);
      strcpy(nick_static, nick);
      return 0;
    }
  }
  /* Comprobar si usuario esta siendo usado por otro*/
  return 0;
}

int server_command_function_user(char* command, int desc, char * nick_static){
  char *user = NULL, *realname = NULL, *prefix, *host, *IP;
  syslog(LOG_INFO, "-----> EXECUTE USER: %s", nick_static);
  if(IRCParse_User(command, &prefix, &user, &host, &IP, &realname)==IRC_OK){
    if (IRCTADUser_New(user, nick_static, realname, NULL, host, IP, desc)==IRC_OK){
      if(IRCMsg_RplWelcome(&command, prefix, nick_static, nick_static, user, host)==IRC_OK){
        send(desc, command, strlen(command), 0);
        syslog(LOG_INFO, "USER: OK");
        return 0;
      }
    }
  }
  free(user);
  free(realname);
  free(host);
  free(IP);
  return 0;
}
