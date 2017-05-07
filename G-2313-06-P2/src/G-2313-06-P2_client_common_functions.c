#include "../includes/G-2313-06-P2_client_common_functions.h"

extern int socket_desc;

/*! @mainpage P2 - Cliente IRC
*
* En esta página se encuentra disponible la documentación de la práctica 3
* de Redes de Comunicaciones II, grupo 2313, pareja 6.
*
* <h1>Hilo de PING-PONG</h1>
* <ul>
*	<li>@subpage client_function_ping</li>
* </ul>
* <h1>Hilo de recepción de datos</h1>
* <ul>
*	<li>@subpage client_function_response</li>
* </ul>
* <h1>Envío de ficheros</h1>
* <ul>
*	<li>@subpage server_especial_enviar_ficheros</li>
*	<li>@subpage server_especial_recibir_ficheros</li>
* </ul>
* <hr>
* La práctica se ha diseñado de una forma parecia a como funcionaba el servidor,
* utilizando en este caso dos arrays de punteros a función mediante los cuales
* somos capaces de repartir los distintos comandos que recibimos o enviamos a
* la función destinada para ello.
* Distinguimos dos grandes estructuras:
* <ul><li>Entradas: Las denominamos así y se refieren a los mensajes que
* recibimos desde el servidor. Cada vez que recibimos un mensaje del servidor
* lo preprocesamos para saber exactamente a qué función corresponde y tras ello
* lo enviamos a la función encarga de repartir cada comando a su función
* correspondiente mediante un array de punteros a dichas funciones.
* </li><li>Salidas: Con ellas nos referimos a los comandos que escriben
* los usuarios en el cliente desarrollado para tal efecto. Gestionamos dichas
* salidas de la misma forma que las entradas, primero preprocesamos la salida para
* saber qué tipo de comando es y tras ello lo enviamos a su función de salida
* mediante un array de punteros a función.
* </li></ul>
* <hr>
* <h1>Parseo de recepciones</h1>
* <h2>Servidor a Cliente</h2>
* <ul>
*	<li>@subpage client_pre_in_function</li>
* <li>@subpage client_execute_in_function</li>
* </ul>
* <h2>[Servidor a cliente] Listado de comandos parseados:</h2>
* @warning Este listado de comandos se compone de los comandos que el
* servidor envía al cliente.
*
* @code
* functions[NICK]             = &server_in_command_nick;
* functions[JOIN]             = &server_in_command_join;
* functions[PART]             = &server_in_command_part;
* functions[MODE]             = &server_in_command_mode;
* functions[TOPIC]            = &server_in_command_topic;
* functions[KICK]             = &server_in_command_kick;
* functions[PRIVMSG]          = &server_in_command_privmsg;
* functions[PING]             = &server_in_command_ping;
* functions[PONG]             = &server_in_command_pong;
* @endcode
* <br><h2>Listado de respuestas parseadas:</h2>
* @code
* functions[RPL_WELCOME]      = &server_in_command_rpl_welcome;
* functions[RPL_CREATED]      = &server_in_command_rpl_created;
* functions[RPL_YOURHOST]     = &server_in_command_rpl_yourhost;
* functions[RPL_LUSERCLIENT]  = &server_in_command_rpl_luserclient;
* functions[RPL_LUSERME]      = &server_in_command_rpl_luserme;
* functions[RPL_MOTDSTART]    = &server_in_command_rpl_motdstart;
* functions[RPL_MOTD]         = &server_in_command_rpl_motd;
* functions[RPL_ENDOFMOTD]    = &server_in_command_rpl_endofmotd;
* functions[RPL_WHOREPLY]     = &server_in_command_rpl_whoreply;
* functions[RPL_AWAY]         = &server_in_command_rpl_away;
* functions[RPL_NOWAWAY]      = &server_in_command_rpl_nowaway;
* functions[RPL_TOPIC]        = &server_in_command_rpl_topic;
* functions[RPL_NOTOPIC]      = &server_in_command_rpl_notopic;
* functions[RPL_YOUREOPER]    = &server_in_command_rpl_youroper;
* functions[RPL_LUSEROP]      = &server_in_command_rpl_luserop;
* functions[RPL_LUSERCHANNELS]= &server_in_command_rpl_luserchannels;
* functions[RPL_YOURESERVICE] = &server_in_command_rpl_youreservice;
* functions[RPL_MYINFO]       = &server_in_command_rpl_myinfo;
* functions[RPL_ENDOFWHO]     = &server_in_command_rpl_endofwho;
* functions[RPL_ENDOFWHOIS]   = &server_in_command_rpl_endofwhois;
* functions[RPL_INFO]         = &server_in_command_rpl_info;
* functions[RPL_WHOISUSER]    = &server_in_command_rpl_whoisuser;
* functions[RPL_WHOISCHANNELS]= &server_in_command_rpl_whoischannels;
* functions[RPL_WHOISOPERATOR]= &server_in_command_rpl_whoisoperator;
* functions[RPL_WHOISSERVER]  = &server_in_command_rpl_whoisserver;
* functions[RPL_WHOISIDLE]    = &server_in_command_rpl_whoisidle;
* functions[RPL_CHANNELMODEIS]= &server_in_command_rpl_channelmodeis;
* functions[RPL_ENDOFNAMES]   = &server_in_command_rpl_endofnames;
* functions[RPL_LIST]         = &server_in_command_rpl_list;
* functions[RPL_LISTEND]      = &server_in_command_rpl_listend;
* functions[RPL_NAMREPLY]     = &server_in_command_rpl_namreply;
* @endcode
* <br><h2>Listado de errores parseados:</h2>
* @code
* functions[ERR_CANNOTSENDTOCHAN]       = &server_in_command_err_cannotsendtochan;
* functions[ERR_ALREADYREGISTRED]       = &server_in_command_err_alreadyregistred;
* functions[ERR_NONICKNAMEGIVEN]        = &server_in_command_err_nonicknamegiven;
* functions[ERR_ERRONEUSNICKNAME]       = &server_in_command_err_erroneusnickname;
* functions[ERR_NICKNAMEINUSE]          = &server_in_command_err_nicknameinuse;
* functions[ERR_NICKCOLLISION]          = &server_in_command_err_nickcollision;
* functions[ERR_UNAVAILRESOURCE]        = &server_in_command_err_unavailresource;
* functions[ERR_RESTRICTED]             = &server_in_command_err_restricted;
* functions[ERR_PASSWDMISMATCH]         = &server_in_command_err_passwdmismatch;
* functions[ERR_BANNEDFROMCHAN]         = &server_in_command_err_bannedfromchan;
* functions[ERR_CHANNELISFULL]          = &server_in_command_err_channelisfull;
* functions[ERR_CHANOPRIVSNEEDED]       = &server_in_command_err_chanoprivsneeded;
* functions[ERR_INVITEONLYCHAN]         = &server_in_command_err_inviteonlychan;
* functions[ERR_NOCHANMODES]            = &server_in_command_err_nochanmodes;
* functions[ERR_NOSUCHCHANNEL]          = &server_in_command_err_nosuchchannel;
* functions[ERR_UNKNOWNMODE]            = &server_in_command_err_unknownmode;
* functions[ERR_NOMOTD]                 = &server_in_command_err_nomotd;
* functions[ERR_NOSUCHNICK]             = &server_in_command_err_nosuchnick;
* @endcode
* <h1>Parseo de envíos</h1>
* <h2>Cliente a Servidor</h2>
* <ul>
*	<li>@subpage client_pre_out_function</li>
* <li>@subpage client_execute_out_function</li>
* </ul>
*
* <h2>[Cliente a Servidor] Listado de comandos parseados:</h2>
* @warning Este listado de comandos se compone de los comandos que el
* cliente envía al servidor.
*
* @code
* functions[UNICK]      = &server_out_command_nick;
* functions[UJOIN]      = &server_out_command_join;
* functions[UNAMES]     = &server_out_command_names;
* functions[ULIST]      = &server_out_command_list;
* functions[UPART]      = &server_out_command_part;
* functions[UMODE]      = &server_out_command_mode;
* functions[UKICK]      = &server_out_command_kick;
* functions[PRIVMSG]    = &server_out_command_privmsg;
* functions[UWHOIS]     = &server_out_command_whois;
* functions[UINVITE]    = &server_out_command_invite;
* functions[UTOPIC]     = &server_out_command_topic;
* functions[UME]        = &server_out_command_me;
* functions[UMSG]       = &server_out_command_msg;
* functions[UNOTICE]    = &server_out_command_notice;
* functions[UIGNORE]    = &server_out_command_ignore;
* functions[UWHO]       = &server_out_command_who;
* functions[UWHOWAS]    = &server_out_command_whowas;
* functions[UMOTD]      = &server_out_command_motd;
* functions[UAWAY]      = &server_out_command_away;
* functions[UPING]      = &server_out_command_ping;
* @endcode
*/

/**
* @page client_function_ping Hilo de PING-PONG
* @section synopsis_1 Synopsis
* @code
*	#include <G-2313-06-P2_client_common_functions.h>
*
*   void* client_function_ping(void *arg);
* @endcode
* @section descripcion_1 Descripción
*
* Inicia el hilo que controla el método PING-PONG mediante el
* cual el cliente es capaz de saber si el servidor sigue encendido
* o no.
* <br>Funciona de la siguiente manera:
* <ul><li><b>Envío de PING:</b> El hilo envía un mensaje PING al servidor y se
* mantiene a la espera.</li>
* <li><b>Recepción de PONG:</b> El hilo espera recibir una respuesta PONG
* del servidor, en caso de no recibirla da por finalizada la conexión y
* desconecta al cliente del servidor.</li>
* </ul>
* Hemos imitado el funcionamiento de un cliente real de IRC mediante el
* uso de este protocolo. El funcionamiento del PING-PONG es básico y
* necesario para el buen desarrollo de una sesión de chat mediante IRC,
* para ello el cliente realiza una comprobación de PING cada 30 segundos.
* Mediante un hilo del cliente enviamos cada 30 segundos un PING al servidor,
* el cliente espera recibir PONG en un periodo de tiempo determinado.
* En caso de que el cliente no reciba el PONG de parte del servidor se procede a
* desconectar la sesión actual ya que se interpreta que el servidor
* a dejado de estar disponible.
* @section return_1 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_1 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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

/**
* @page client_function_response Hilo de recepción de datos
* @section synopsis_2 Synopsis
* @code
*	#include <G-2313-06-P2_client_common_functions.h>
*
*   void* client_function_response(void *arg);
* @endcode
* @section descripcion_2 Descripción
*
* Inicia el hilo que controla el método de recepción de datos del
* servidor mediante el cual el cliente recibe los mensajes que el servidor
* le envía. Desde este hilo se procesa cada mensaje y se envían a la función
* adecuada para procesarlos:
* @code
*   client_pre_in_function(resultado);
* @endcode
* Es importante destacar que hemos tenido especial consideración con las
* llegadas del servidor al cliente, para ello hemos diseñado un hilo específico y
* exclusivo para la recepción de dichas llegadas a través del socket.
* Los mensajes son recibidos y preprocesados, una vez que el mensaje se considera
* íntegro es enviado a la función de preprocesado de las entradas del
* servidor (detallado más arriba).
* @section return_2 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_2 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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

void client_show_error_main(char* msg){
  char *channelActual;
  channelActual = IRCInterface_ActiveChannelName();
  if((channelActual!=NULL)&&(strcmp(channelActual, "System")!=0)){
    IRCInterface_WriteChannel(IRCInterface_ActiveChannelName(), "*", msg);
  } else {
    IRCInterface_WriteSystem(NULL, msg);
  }
}

/**
* @page client_pre_in_function Preprocesado de entradas
* @section synopsis_3 Synopsis
* @code
*	#include <G-2313-06-P2_client_common_functions.h>
*
*   void client_pre_in_function(char* command);
* @endcode
* @section descripcion_3 Descripción
*
* Recibe como parámetro un mensaje del servidor, lo desfragmenta
* mediante la función IRC_CommandQuery para saber exactamente qué comando
* es el que se ha recibido y se lo envía a la función que controla
* el procesamiento de los comandos mediante un array de punteros a función.
* @code
*   client_execute_in_function(IRC_CommandQuery(command), command);
* @endcode
*
* @section return_3 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_3 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void client_pre_in_function(char* command){
  client_execute_in_function(IRC_CommandQuery(command), command);
}

/**
* @page client_execute_in_function Parseo de entradas
* @section synopsis_4 Synopsis
* @code
*	#include <G-2313-06-P2_client_common_functions.h>
*
*   void client_execute_in_function(long functionName, char* command);
* @endcode
* @section descripcion_4 Descripción
*
* Recibe como parámetro un mensaje desfragmentado y lo pasa por un array
* de punteros a función a la función correspondiente. En caso de no tener
* ninguna función disponible para el comando se le enviará a la función
* por defecto.
* <br><h2>Listado de comandos parseados:</h2>
* @code
* functions[NICK]             = &server_in_command_nick;
* functions[JOIN]             = &server_in_command_join;
* functions[PART]             = &server_in_command_part;
* functions[MODE]             = &server_in_command_mode;
* functions[TOPIC]            = &server_in_command_topic;
* functions[KICK]             = &server_in_command_kick;
* functions[PRIVMSG]          = &server_in_command_privmsg;
* functions[PING]             = &server_in_command_ping;
* functions[PONG]             = &server_in_command_pong;
* @endcode
* <br><h2>Listado de respuestas parseadas:</h2>
* @code
* functions[RPL_WELCOME]      = &server_in_command_rpl_welcome;
* functions[RPL_CREATED]      = &server_in_command_rpl_created;
* functions[RPL_YOURHOST]     = &server_in_command_rpl_yourhost;
* functions[RPL_LUSERCLIENT]  = &server_in_command_rpl_luserclient;
* functions[RPL_LUSERME]      = &server_in_command_rpl_luserme;
* functions[RPL_MOTDSTART]    = &server_in_command_rpl_motdstart;
* functions[RPL_MOTD]         = &server_in_command_rpl_motd;
* functions[RPL_ENDOFMOTD]    = &server_in_command_rpl_endofmotd;
* functions[RPL_WHOREPLY]     = &server_in_command_rpl_whoreply;
* functions[RPL_AWAY]         = &server_in_command_rpl_away;
* functions[RPL_NOWAWAY]      = &server_in_command_rpl_nowaway;
* functions[RPL_TOPIC]        = &server_in_command_rpl_topic;
* functions[RPL_NOTOPIC]      = &server_in_command_rpl_notopic;
* functions[RPL_YOUREOPER]    = &server_in_command_rpl_youroper;
* functions[RPL_LUSEROP]      = &server_in_command_rpl_luserop;
* functions[RPL_LUSERCHANNELS]= &server_in_command_rpl_luserchannels;
* functions[RPL_YOURESERVICE] = &server_in_command_rpl_youreservice;
* functions[RPL_MYINFO]       = &server_in_command_rpl_myinfo;
* functions[RPL_ENDOFWHO]     = &server_in_command_rpl_endofwho;
* functions[RPL_ENDOFWHOIS]   = &server_in_command_rpl_endofwhois;
* functions[RPL_INFO]         = &server_in_command_rpl_info;
* functions[RPL_WHOISUSER]    = &server_in_command_rpl_whoisuser;
* functions[RPL_WHOISCHANNELS]= &server_in_command_rpl_whoischannels;
* functions[RPL_WHOISOPERATOR]= &server_in_command_rpl_whoisoperator;
* functions[RPL_WHOISSERVER]  = &server_in_command_rpl_whoisserver;
* functions[RPL_WHOISIDLE]    = &server_in_command_rpl_whoisidle;
* functions[RPL_CHANNELMODEIS]= &server_in_command_rpl_channelmodeis;
* functions[RPL_ENDOFNAMES]   = &server_in_command_rpl_endofnames;
* functions[RPL_LIST]         = &server_in_command_rpl_list;
* functions[RPL_LISTEND]      = &server_in_command_rpl_listend;
* functions[RPL_NAMREPLY]     = &server_in_command_rpl_namreply;
* @endcode
* <br><h2>Listado de errores parseados:</h2>
* @code
* functions[ERR_CANNOTSENDTOCHAN]       = &server_in_command_err_cannotsendtochan;
* functions[ERR_ALREADYREGISTRED]       = &server_in_command_err_alreadyregistred;
* functions[ERR_NONICKNAMEGIVEN]        = &server_in_command_err_nonicknamegiven;
* functions[ERR_ERRONEUSNICKNAME]       = &server_in_command_err_erroneusnickname;
* functions[ERR_NICKNAMEINUSE]          = &server_in_command_err_nicknameinuse;
* functions[ERR_NICKCOLLISION]          = &server_in_command_err_nickcollision;
* functions[ERR_UNAVAILRESOURCE]        = &server_in_command_err_unavailresource;
* functions[ERR_RESTRICTED]             = &server_in_command_err_restricted;
* functions[ERR_PASSWDMISMATCH]         = &server_in_command_err_passwdmismatch;
* functions[ERR_BANNEDFROMCHAN]         = &server_in_command_err_bannedfromchan;
* functions[ERR_CHANNELISFULL]          = &server_in_command_err_channelisfull;
* functions[ERR_CHANOPRIVSNEEDED]       = &server_in_command_err_chanoprivsneeded;
* functions[ERR_INVITEONLYCHAN]         = &server_in_command_err_inviteonlychan;
* functions[ERR_NOCHANMODES]            = &server_in_command_err_nochanmodes;
* functions[ERR_NOSUCHCHANNEL]          = &server_in_command_err_nosuchchannel;
* functions[ERR_UNKNOWNMODE]            = &server_in_command_err_unknownmode;
* functions[ERR_NOMOTD]                 = &server_in_command_err_nomotd;
* functions[ERR_NOSUCHNICK]             = &server_in_command_err_nosuchnick;
* @endcode
*
* @section return_4 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_4 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void client_execute_in_function(long functionName, char* command){
  FunctionCallBack functions[IRC_MAX_COMMANDS];
  int i;
  for(i=0; i<IRC_MAX_COMMANDS; i++){
    functions[i]=NULL;
  }
  /* Definir lista de funciones para cada comando*/
  functions[NICK]             = &server_in_command_nick;
  functions[JOIN]             = &server_in_command_join;
  functions[PART]             = &server_in_command_part;
  functions[MODE]             = &server_in_command_mode;
  functions[TOPIC]            = &server_in_command_topic;
  functions[KICK]             = &server_in_command_kick;
  functions[PRIVMSG]          = &server_in_command_privmsg;
  functions[PING]             = &server_in_command_ping;
  functions[PONG]             = &server_in_command_pong;

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
  functions[RPL_AWAY]         = &server_in_command_rpl_away;
  functions[RPL_NOWAWAY]      = &server_in_command_rpl_nowaway;
  functions[RPL_TOPIC]        = &server_in_command_rpl_topic;
  functions[RPL_NOTOPIC]      = &server_in_command_rpl_notopic;
  functions[RPL_YOUREOPER]    = &server_in_command_rpl_youroper;
  functions[RPL_LUSEROP]      = &server_in_command_rpl_luserop;
  functions[RPL_LUSERCHANNELS]= &server_in_command_rpl_luserchannels;
  functions[RPL_YOURESERVICE] = &server_in_command_rpl_youreservice;
  functions[RPL_MYINFO]       = &server_in_command_rpl_myinfo;
  functions[RPL_ENDOFWHO]     = &server_in_command_rpl_endofwho;
  functions[RPL_ENDOFWHOIS]   = &server_in_command_rpl_endofwhois;
  functions[RPL_INFO]         = &server_in_command_rpl_info;
  functions[RPL_WHOISUSER]    = &server_in_command_rpl_whoisuser;
  functions[RPL_WHOISCHANNELS]= &server_in_command_rpl_whoischannels;
  functions[RPL_WHOISOPERATOR]= &server_in_command_rpl_whoisoperator;
  functions[RPL_WHOISSERVER]  = &server_in_command_rpl_whoisserver;
  functions[RPL_WHOISIDLE]    = &server_in_command_rpl_whoisidle;
  functions[RPL_CHANNELMODEIS]= &server_in_command_rpl_channelmodeis;
  functions[RPL_ENDOFNAMES]   = &server_in_command_rpl_endofnames;
  functions[RPL_LIST]         = &server_in_command_rpl_list;
  functions[RPL_LISTEND]      = &server_in_command_rpl_listend;
  functions[RPL_NAMREPLY]     = &server_in_command_rpl_namreply;


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

/**
* @page client_pre_out_function Preprocesado de salidas
* @section synopsis_5 Synopsis
* @code
*	#include <G-2313-06-P2_client_common_functions.h>
*
*   void client_pre_out_function(char* command);
* @endcode
* @section descripcion_5 Descripción
*
* Recibe como parámetro un mensaje del cliente, lo desfragmenta
* mediante la función IRC_CommandQuery para saber exactamente qué comando
* es el que se ha recibido y se lo envía a la función que controla
* el procesamiento de los comandos mediante un array de punteros a función.
* @code
*   client_execute_out_function(IRC_CommandQuery(command), command);
* @endcode
*
* @section return_5 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_5 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void client_pre_out_function(char* command){
  syslog(LOG_INFO, "[CLIENTE] COMANDO %ld", IRCUser_CommandQuery(command));
  client_execute_out_function(IRCUser_CommandQuery(command), command);
}

/**
* @page client_execute_out_function Parseo de salidas
* @section synopsis_6 Synopsis
* @code
*	#include <G-2313-06-P2_client_common_functions.h>
*
*   void client_execute_out_function(long functionName, char* command);
* @endcode
* @section descripcion_6 Descripción
*
* Recibe como parámetro un mensaje desfragmentado y lo pasa por un array
* de punteros a función a la función correspondiente. En caso de no tener
* ninguna función disponible para el comando se le enviará a la función
* por defecto.
* <br><h2>Listado de comandos parseados:</h2>
* @code
* functions[UNICK]      = &server_out_command_nick;
* functions[UJOIN]      = &server_out_command_join;
* functions[UNAMES]     = &server_out_command_names;
* functions[ULIST]      = &server_out_command_list;
* functions[UPART]      = &server_out_command_part;
* functions[UMODE]      = &server_out_command_mode;
* functions[UKICK]      = &server_out_command_kick;
* functions[PRIVMSG]    = &server_out_command_privmsg;
* functions[UWHOIS]     = &server_out_command_whois;
* functions[UINVITE]    = &server_out_command_invite;
* functions[UTOPIC]     = &server_out_command_topic;
* functions[UME]        = &server_out_command_me;
* functions[UMSG]       = &server_out_command_msg;
* functions[UNOTICE]    = &server_out_command_notice;
* functions[UIGNORE]    = &server_out_command_ignore;
* functions[UWHO]       = &server_out_command_who;
* functions[UWHOWAS]    = &server_out_command_whowas;
* functions[UMOTD]      = &server_out_command_motd;
* functions[UAWAY]      = &server_out_command_away;
* functions[UPING]      = &server_out_command_ping;
* @endcode
*
* @section return_6 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_6 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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
  functions[UAWAY]      = &server_out_command_away;
  functions[UPING]      = &server_out_command_ping;

  /* Llamar a la funcion del argumento */
  if((functionName<0)||(functionName>IRC_MAX_COMMANDS)||(functions[functionName]==NULL)){
    if(command[0]=='/'){
      IRCInterface_ErrorDialog("Ese comando no existe, introduce un nuevo comando. Puedes consultar el helper en la ventana del chat.");
      client_show_error_main("****************************");
      client_show_error_main("Comandos disponibles:");
      client_show_error_main("[01] /JOIN [<#canal>]");
      client_show_error_main("[02] /NICK [nick]");
      client_show_error_main("[03] /NAMES [<#canal>]");
      client_show_error_main("[04] /LIST [<#canal>]");
      client_show_error_main("[05] /PART [<#canal>]");
      client_show_error_main("[06] /PART [<#canal>]");
      client_show_error_main("[07] /MODE [<#canal> <flags> [<args>]]");
      client_show_error_main("[08] /KICK [<#canal> <client> [<message>]]");
      client_show_error_main("[09] /PRIVMSG [<target> <message>]");
      client_show_error_main("[10] /WHOIS [<server>] <nicknames>");
      client_show_error_main("[11] /INVITE <target> <#canal>");
      client_show_error_main("[12] /TOPIC <#canal> [<topic>]");
      client_show_error_main("[13] /ME");
      client_show_error_main("[14] /MSG <mensaje>");
      client_show_error_main("[15] /NOTICE <target> <mensaje>");
      client_show_error_main("[16] /IGNORE <target>");
      client_show_error_main("[17] /WHO [<target>]");
      client_show_error_main("[18] /WHOWAS [<target>]");
      client_show_error_main("[19] /MOTD [<server>]");
      client_show_error_main("[20] /AWAY [<mensaje>]");
      client_show_error_main("[21] /PING [<target>]");
      client_show_error_main("****************************");
    } else {
      (*functions[PRIVMSG])(command);
    }
  } else {
    (*functions[functionName])(command);
  }
}
