#include "../includes/G-2313-06-P2_client_function_handlers.h"

extern int socket_desc;

/* IN FUNCTIONS */
void server_in_command_nick(char* command, int desc, char * nick_static, int* register_status){
  char *prefix, *nick, *msg, *msgEnvio[150];
  char *nickComplex, *userComplex, *hostComplex, *serverComplex;
  IRCInterface_PlaneRegisterInMessageThread(command);
  if(IRCParse_Nick(command, &prefix, &nick, &msg)==IRC_OK){
    if(IRCParse_ComplexUser(prefix, &nickComplex, &userComplex, &hostComplex, &serverComplex)==IRC_OK){
      sprintf(msgEnvio, "Ahora tu nick es: %s", msg);
      IRCInterface_WriteSystemThread(NULL, msgEnvio);
      IRCInterface_ChangeNick(nickComplex, msg);
    }
  }
}

void server_in_command_pong(char* command, int desc, char * nick_static, int* register_status){
  IRCInterface_PlaneRegisterInMessageThread(command);
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
