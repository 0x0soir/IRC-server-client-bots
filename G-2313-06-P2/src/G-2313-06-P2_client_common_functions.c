#include "../includes/G-2313-06-P2_client_common_functions.h"

extern int socket_desc;

void* client_function_ping(void *arg){
  char* command, *server, *nick, *user, *realname, *password;
  int port, ssl;
  IRCInterface_GetMyUserInfoThread(&nick, &user, &realname, &password, &server, &port, &ssl);
  free(nick);
  free(user);
  free(realname);
  syslog(LOG_INFO, "[CLIENTE] Inicia hilo ping");
  while(TRUE) {
    syslog(LOG_INFO, "[CLIENTE] PING");
    IRCMsg_Ping(&command, NULL, server, NULL);
    if(send(socket_desc, command, strlen(command), 0) < 0) {
      syslog(LOG_INFO, "[CLIENTE] PING error");
      return NULL;
    }
    IRCInterface_PlaneRegisterOutMessageThread(command);
    sleep(30);
  }
}

void* client_function_response(void *arg){
  char buff[CLIENT_MESSAGE_MAXSIZE] = "";
  char* next;
  char* resultado;
  int valread;
  /*int tid;*/
  /* tid = pthread_self(); */

  while(TRUE) {
    syslog(LOG_INFO, "[CLIENTE] Esperando a recibir mensajes...");
    valread = recv(socket_desc, buff, CLIENT_MESSAGE_MAXSIZE, 0);
    if(valread < 0) {
      pthread_exit(NULL);
    }

    syslog(LOG_INFO, "[CLIENTE] Mensaje recibido: %s", buff);
    next = IRC_UnPipelineCommands(buff, &resultado);
    syslog(LOG_INFO, "[CLIENTE] Procesa: %s", resultado);
    client_pre_in_function(resultado);
    free(resultado);
    while(next != NULL) {
      next = IRC_UnPipelineCommands(next, &resultado);
      if(resultado !=((void *) 0)) {
        syslog(LOG_INFO, "[CLIENTE] Procesa: %s", resultado);
        client_pre_in_function(resultado);
      }
      free(resultado);
    }
    memset(buff,0,sizeof(buff));
  }
}

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
  functions[NICK] = &server_in_command_nick;
  functions[PONG] = &server_in_command_pong;
  functions[JOIN] = &server_in_command_join;
  functions[PART] = &server_in_command_part;
  functions[MODE] = &server_in_command_mode;
  functions[TOPIC] = &server_in_command_topic;
  functions[KICK] = &server_in_command_kick;
  functions[RPL_WELCOME] = &server_in_command_rpl_welcome;
  functions[RPL_CREATED] = &server_in_command_rpl_created;
  functions[RPL_YOURHOST] = &server_in_command_rpl_yourhost;
  functions[RPL_LUSERCLIENT] = &server_in_command_rpl_luserclient;
  functions[RPL_LUSERME] = &server_in_command_rpl_luserme;
  functions[RPL_MOTDSTART] = &server_in_command_rpl_motdstart;
  functions[RPL_MOTD] = &server_in_command_rpl_motd;
  functions[RPL_ENDOFMOTD] = &server_in_command_rpl_endofmotd;



  /* Llamar a la funcion del argumento */
  if((functionName<0)||(functionName>IRC_MAX_COMMANDS)||(functions[functionName]==NULL)){
    /* Default aqui */
  } else {
   (*functions[functionName])(command);
  }
}

void client_pre_out_function(char* command){
  syslog(LOG_INFO, "[CLIENTE] COMANDO %ld", IRC_CommandQuery(command+1));
  client_execute_out_function(IRC_CommandQuery(command+1), command);
}

void client_execute_out_function(long functionName, char* command){
  FunctionCallBack functions[IRC_MAX_USER_COMMANDS];
  int i;
  for(i=0; i<IRC_MAX_USER_COMMANDS; i++){
    functions[i]=NULL;
  }
  /* Definir lista de funciones para cada comando*/
  functions[NICK] = &server_out_command_nick;
  functions[JOIN] = &server_out_command_join;
  functions[NAMES] = &server_out_command_names;
  functions[LIST] = &server_out_command_list;
  functions[PART] = &server_out_command_part;
  functions[MODE] = &server_out_command_mode;

  /* Llamar a la funcion del argumento */
  if((functionName<0)||(functionName>IRC_MAX_USER_COMMANDS)||(functions[functionName]==NULL)){
    /* Default aqui */
  } else {
    (*functions[functionName])(command);
  }
}
