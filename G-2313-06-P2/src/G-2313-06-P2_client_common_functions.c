#include "../includes/G-2313-06-P2_client_common_functions.h"

extern int socket_desc;

void* client_function_ping(void *arg){
  char* command, *server, *nick, *user, *realname, *password;
  int port, ssl;
  IRCInterface_GetMyUserInfoThread(&nick, &user, &realname, &password, &server, &port, &ssl);
  IRC_MFree(3, &nick, &user, &realname);
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

void client_show_error(char* msg){
  char *channelActual;
  channelActual = IRCInterface_ActiveChannelName();
  if((channelActual!=NULL)&&(strcmp(channelActual, "System")!=0)){
    IRCInterface_WriteChannelThread(IRCInterface_ActiveChannelName(), "*", msg);
  } else {
    IRCInterface_WriteSystemThread(NULL, msg);
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
  functions[NICK]             = &server_in_command_nick;
  functions[PONG]             = &server_in_command_pong;
  functions[JOIN]             = &server_in_command_join;
  functions[PART]             = &server_in_command_part;
  functions[MODE]             = &server_in_command_mode;
  functions[TOPIC]            = &server_in_command_topic;
  functions[KICK]             = &server_in_command_kick;
  functions[PRIVMSG]          = &server_in_command_privmsg;
  functions[PING]             = &server_in_command_ping;

  /* Mensajes de respuesta*/
  functions[RPL_WELCOME]      = &server_in_command_rpl_welcome;
  functions[RPL_CREATED]      = &server_in_command_rpl_created;
  functions[RPL_YOURHOST]     = &server_in_command_rpl_yourhost;
  functions[RPL_LUSERCLIENT]  = &server_in_command_rpl_luserclient;
  functions[RPL_LUSERME]      = &server_in_command_rpl_luserme;
  functions[RPL_MOTDSTART]    = &server_in_command_rpl_motdstart;
  functions[RPL_MOTD]         = &server_in_command_rpl_motd;
  functions[RPL_ENDOFMOTD]    = &server_in_command_rpl_endofmotd;
  functions[RPL_WHOREPLY]     = &server_in_command_rpl_whoreply;

  /* Mensajes de error */
  functions[ERR_CANNOTSENDTOCHAN] = &server_in_command_err_cannotsendtochan;
  functions[ERR_ALREADYREGISTRED] = &server_in_command_err_alreadyregistred;
  functions[ERR_NONICKNAMEGIVEN] = &server_in_command_err_nonicknamegiven;
  functions[ERR_ERRONEUSNICKNAME] = &server_in_command_err_erroneusnickname;
  functions[ERR_NICKNAMEINUSE] = &server_in_command_err_nicknameinuse;
  functions[ERR_NICKCOLLISION] = &server_in_command_err_nickcollision;
  functions[ERR_UNAVAILRESOURCE] = &server_in_command_err_unavailresource;
  functions[ERR_RESTRICTED] = &server_in_command_err_restricted;
  functions[ERR_PASSWDMISMATCH] = &server_in_command_err_passwdmismatch;
  functions[ERR_BANNEDFROMCHAN] = &server_in_command_err_bannedfromchan;
  functions[ERR_CHANNELISFULL] = &server_in_command_err_channelisfull;
  functions[ERR_CHANOPRIVSNEEDED] = &server_in_command_err_chanoprivsneeded;
  functions[ERR_INVITEONLYCHAN] = &server_in_command_err_inviteonlychan;
  functions[ERR_NOCHANMODES] = &server_in_command_err_nochanmodes;
  functions[ERR_NOSUCHCHANNEL] = &server_in_command_err_nosuchchannel;
  functions[ERR_UNKNOWNMODE] = &server_in_command_err_unknownmode;
  functions[ERR_NOMOTD] = &server_in_command_err_nomotd;
  functions[ERR_NOSUCHNICK] = &server_in_command_err_nosuchnick;

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
  functions[UNICK]      = &server_out_command_nick;
  functions[UJOIN]      = &server_out_command_join;
  functions[UNAMES]     = &server_out_command_names;
  functions[ULIST]      = &server_out_command_list;
  functions[UPART]      = &server_out_command_part;
  functions[UMODE]      = &server_out_command_mode;
  functions[UKICK]      = &server_out_command_kick;
  functions[PRIVMSG]    = &server_out_command_privmsg;
  functions[UWHOIS]     = &server_out_command_whois;
  functions[UINVITE]    = &server_out_command_invite;
  functions[UTOPIC]     = &server_out_command_topic;
  functions[UME]        = &server_out_command_me;
  functions[UMSG]       = &server_out_command_msg;
  functions[UNOTICE]    = &server_out_command_notice;
  functions[UIGNORE]    = &server_out_command_ignore;
  functions[UWHO]       = &server_out_command_who;
  functions[UWHOWAS]    = &server_out_command_whowas;
  functions[UMOTD]      = &server_out_command_motd;

  /* Llamar a la funcion del argumento */
  if((functionName<0)||(functionName>IRC_MAX_COMMANDS)||(functions[functionName]==NULL)){
    if(command[0]=='/'){
      IRCInterface_ErrorDialog("Ese comando no existe, introduce un nuevo comando.");
    } else {
      (*functions[PRIVMSG])(command);
    }
  } else {
    (*functions[functionName])(command);
  }
}
