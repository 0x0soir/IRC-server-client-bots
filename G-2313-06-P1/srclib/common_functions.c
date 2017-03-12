#include "../includes/common_functions.h"

int server_users_find_by_nick(char* data){
  char *unknown_user = NULL, *unknown_real = NULL, *host = NULL, *IP = NULL, *away = NULL;
  long unknown_id = 0, creationTS, actionTS;
  bool return_flag = false;
  int socket = NULL;
  syslog(LOG_INFO, "NICK: BUSCANDO USUARIO POR: %s", data);
  if(IRCTADUser_GetData(&unknown_id, &unknown_user, &data, &unknown_real, &host, &IP, &socket, &creationTS, &actionTS, &away)==IRC_OK){
    syslog(LOG_INFO, "NICK: USUARIO ENCONTRADO");
    return_flag = true;
  } else {
    syslog(LOG_INFO, "NICK: USUARIO NO ENCONTRADO %s", data);
  }
  if(unknown_user){
    free(unknown_user);
    unknown_user = NULL;
  }
  if(unknown_real){
    free(unknown_real);
    unknown_real = NULL;
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
  return return_flag;
}

int server_users_find_by_socket(int sockdesc){
  char *unknown_user = NULL, *unknown_nick = NULL, *unknown_real = NULL;
  long unknown_id = 0;
  char *host = NULL, *IP = NULL, *away = NULL;
  long creationTS, actionTS;
  bool return_flag = false;
  if(IRCTADUser_GetData(&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, &sockdesc, &creationTS, &actionTS, &away)==IRC_OK){
    if(unknown_nick!=NULL){
      syslog(LOG_INFO, "USUARIO ENCONTRADO");
      return_flag = true;
    }
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
  return return_flag;
}

int server_channels_find_by_name(char *channel){
  char **channels;
  long size;
  int i, return_flag = 0;
  if(IRCTADChan_GetList(&channels, &size, NULL)==IRC_OK){
    for (i = 0; i < size; i++){
      if (strcmp(channels[i], channel) == 0) {
        return_flag = 1;
      }
    }
    IRCTADChan_FreeList(channels, size);
  }
  return return_flag;
}

int server_channels_update_nick(char *nick_old, char* nick_new){
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int find_socket = 0;
  long find_id = 0, find_creationTS, find_actionTS;
  int return_value = -1;
  if(IRCTADUser_GetData(&find_id, &find_user, &nick_old, &find_real, &find_host, &find_ip, &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
    if(IRCTADUser_Set(find_id, find_user, nick_old, find_real, find_user, nick_new, find_real)==IRC_OK){
      return_value = 0;
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
  find_user = find_real = find_host = find_away = NULL;
  find_socket = find_id = 0;
  return return_value;
}

int server_channels_update_away(char* nick, char* away){
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int find_socket = 0;
  long find_id = 0, find_creationTS, find_actionTS;
  int return_value = -1;
  if(IRCTADUser_GetData(&find_id, &find_user, &nick, &find_real, &find_host, &find_ip, &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
    if(IRCTADUser_SetAway(find_id, find_user, nick, find_real, away)==IRC_OK){
      return_value = 0;
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
  find_user = find_real = find_host = find_away = NULL;
  find_socket = find_id = 0;
  return return_value;
}
