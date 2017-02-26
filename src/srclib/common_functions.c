#include "../includes/common_functions.h"

int server_users_find_by_nick(char* data){
  char *unknown_user = NULL, *unknown_real = NULL;
  long unknown_id = 0;
  char *host, *IP, *away;
  long creationTS, actionTS;
  bool return_flag = false;
  int socket = NULL;
  syslog(LOG_INFO, "NICK: BUSCANDO USUARIO POR: %s", data);
  if(IRCTADUser_GetData(&unknown_id, &unknown_user, &data, &unknown_real, &host, &IP, &socket, &creationTS, &actionTS, &away)==IRC_OK){
    syslog(LOG_INFO, "NICK: USUARIO ENCONTRADO");
    return_flag = true;
  } else {
    syslog(LOG_INFO, "NICK: ERROR BUSCANDO POR NICK %ld %s %s", unknown_id, unknown_user, unknown_real);
  }
  return return_flag;
}

int server_users_find_by_socket(int sockdesc){
  char *unknown_user = NULL, *unknown_nick = NULL, *unknown_real = NULL;
  long unknown_id = 0;
  char *host, *IP, *away;
  long creationTS, actionTS;
  bool return_flag = false;
  if(IRCTADUser_GetData(&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, &sockdesc, &creationTS, &actionTS, &away)==IRC_OK){
    if(unknown_nick!=NULL){
      syslog(LOG_INFO, "USUARIO ENCONTRADO");
      return_flag = true;
    }
  }
  free(unknown_user);
  free(unknown_nick);
  free(host);
  free(IP);
  free(away);
  return return_flag;
}
