#include "../includes/G-2313-06-BOT_bot_in_out.h"

void client_pre_in_function(char* command){
  client_execute_in_function(IRC_CommandQuery(command), command);
}

void client_execute_in_function(long functionName, char* command){
  FunctionCallBack functions[IRC_MAX_COMMANDS];
  int i;
  for(i=0; i<IRC_MAX_COMMANDS; i++){
    functions[i]=NULL;
  }
  /* Definir lista de funciones para cada comando*/
  functions[PRIVMSG]          = &server_in_command_privmsg;
  functions[PING]             = &server_in_command_ping;

  /* Llamar a la funcion del argumento */
  if((functionName<0)||(functionName>IRC_MAX_COMMANDS)||(functions[functionName]==NULL)){
    /* Default aqui */
  } else {
   (*functions[functionName])(command);
  }
}

void client_pre_out_function(char* command){
  syslog(LOG_INFO, "[CLIENTE] COMANDO %ld", IRCUser_CommandQuery(command));
  client_execute_out_function(IRCUser_CommandQuery(command), command);
}

void client_execute_out_function(long functionName, char* command){
  FunctionCallBack functions[IRC_MAX_COMMANDS];
  int i;
  for(i=0; i<IRC_MAX_COMMANDS; i++){
    functions[i]=NULL;
  }
  /* Definir lista de funciones para cada comando*/
  functions[UNAMES]     = &server_out_command_names;
  functions[ULIST]      = &server_out_command_list;

  /* Llamar a la funcion del argumento */
  if((functionName<0)||(functionName>IRC_MAX_COMMANDS)||(functions[functionName]==NULL)){
  } else {
    (*functions[functionName])(command);
  }
}
