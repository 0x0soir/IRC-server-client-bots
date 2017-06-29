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
  functions[JOIN]             = &server_in_command_join;
  functions[PING]             = &server_in_command_ping;
  functions[RPL_WELCOME]      = &server_in_command_rpl_welcome;
  functions[RPL_LIST]         = &server_in_command_rpl_list;

  /* Llamar a la funcion del argumento */
  if((functionName<0)||(functionName>IRC_MAX_COMMANDS)||(functions[functionName]==NULL)){
    /* Default aqui */
  } else {
   (*functions[functionName])(command);
  }
}
