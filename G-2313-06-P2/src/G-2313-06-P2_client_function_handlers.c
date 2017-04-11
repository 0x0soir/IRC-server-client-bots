#include "../includes/G-2313-06-P2_client_function_handlers.h"

extern int socket_desc;
extern char* nick_cliente;

/* IN FUNCTIONS */
void server_in_command_nick(char* command, int desc, char * nick_static, int* register_status){
  char *prefix, *nick, *msg, *msgEnvio[150];
  char *nickComplex, *hostComplex, *serverComplex;
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
