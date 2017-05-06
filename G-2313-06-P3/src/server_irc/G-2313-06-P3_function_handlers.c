#include "../../includes/server_irc/G-2313-06-P3_function_handlers.h"

extern bool server_ssl;

/*! @page server_commands Comandos
*
* <p>Esta sección incluye las funciones de comandos utilizadas
* para parsear los comandos recibidos del IRC. A estas funciones accede
* el array de punteros a función descrito en <b>server_execute_function</b>.</p>
* @section cabeceras3 Cabeceras
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h.h>
* </code>
* @section functions3 Funciones implementadas
* <p>Se incluyen las siguientes funciones:
* <ul>
* <li>@subpage server_command_nick</li>
* <li>@subpage server_command_user</li>
* <li>@subpage server_command_join</li>
* <li>@subpage server_command_quit</li>
* <li>@subpage server_command_ping</li>
* <li>@subpage server_command_list</li>
* <li>@subpage server_command_privmsg</li>
* <li>@subpage server_command_part</li>
* <li>@subpage server_command_names</li>
* <li>@subpage server_command_kick</li>
* <li>@subpage server_command_mode</li>
* <li>@subpage server_command_away</li>
* <li>@subpage server_command_whois</li>
* <li>@subpage server_command_topic</li>
* <li>@subpage server_command_motd</li>
* </ul></p>
*/

/**
* @page server_command_nick server_command_nick
* @brief Función handler del comando NICK del IRC.
* @section synopsis_nick Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_nick(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_nick Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>IRCMsg_ErrNoNickNameGiven:</b> Comprueba que el nick no sea nulo.</li>
* <li><b>IRCMsg_ErrErroneusNickName:</b> Comprueba que el nick no sea erróneo
* (máx. 9 carácteres).</li>
* <li><b>IRCMsg_ErrNickNameInUse:</b> Comprueba que el nick no esté siendo
* utilizado por otro usuario.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta de dos
* formas posibles:</p>
* <ul>
* <li><b>Actualiza nick existente:</b> Es decir, en caso de que se esté
* actualizando el nick de un usuario existente, se actualiza el valor utilizando
* la función <b>server_channels_update_nick</b>.
* <br>Condición necesaria: El valor de register_status tiene que ser 2 (es
* decir, usuario <b>registrado</b> correctamente de forma previa).
* </li>
* <li><b>Guarda nick para registro:</b> Es decir, en caso de que no se haya
* producido un registro previo, se guarda el valor del nick para usarlo
* posteriormente en el comando USER.
* <br>Condición necesaria: El valor de register_status tiene que ser distinto
* de 2 (es decir, usuario <b>no registrado</b> correctamente de forma previa).
* </li>
* </ul>
* <p>En ambos casos se devuelve un mensaje generado por <b>IRCMsg_Nick</b> y
* libera los recursos utilizados en la función (punteros).
* @section return_nick Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_nick Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_nick(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *nick = NULL, *prefix = NULL, *msg = NULL;
  int response;
  if(IRCParse_Nick(command, &prefix, &nick, &msg)!=IRC_OK){
    if(IRCMsg_ErrNoNickNameGiven(&msg, prefix, nick)==IRC_OK){
      if(server_ssl){
        enviar_datos_SSL(ssl, msg);
      } else {
        send(desc, msg, strlen(msg), 0);
      }
    }
  } else if(strlen(nick) > 9){
    if(IRCMsg_ErrErroneusNickName(&msg, prefix, nick, nick)==IRC_OK){
      if(server_ssl){
        enviar_datos_SSL(ssl, msg);
      } else {
        send(desc, msg, strlen(msg), 0);
      }
    }
  } else if((response=server_users_find_by_nick(nick))==true){
    if(IRCMsg_ErrNickNameInUse(&msg, prefix, nick, nick)==IRC_OK){
      syslog(LOG_INFO, "[DESC: %d] NICK: %s ya se está usando, idiota (Valor de control: %d)", desc, nick, *register_status);
      if(server_ssl){
        enviar_datos_SSL(ssl, msg);
      } else {
        send(desc, msg, strlen(msg), 0);
      }
    }
  } else {
    if(*register_status==2){
      response = server_channels_update_nick(nick_static, nick);
      if(response==0){
        if(IRCMsg_Nick(&msg, "ip.servidor", NULL, nick)==IRC_OK){
          syslog(LOG_INFO, "[DESC: %d] NICK: UPDATED OK", desc);
          if(server_ssl){
            enviar_datos_SSL(ssl, msg);
          } else {
            send(desc, msg, strlen(msg), 0);
          }
          strcpy(nick_static, nick);
        }
      }
    } else {
      if(IRCMsg_Nick(&msg, "ip.servidor", NULL, nick)==IRC_OK){
        syslog(LOG_INFO, "[DESC: %d] NICK: OK", desc);
        if(server_ssl){
          // if(enviar_datos_SSL(ssl, msg)>1){
          //   syslog(LOG_INFO, "[SSL] Mensaje enviado OK");
          // } else {
          //   syslog(LOG_INFO, "[SSL] Error al enviar mensaje");
          // }
        } else {
          syslog(LOG_INFO, "[NO SSL] Enviando mensaje en nick");
          send(desc, msg, strlen(msg), 0);
        }
        strcpy(nick_static, nick);
        *register_status = 1;
      }
    }
  }
  /* Comprobar si usuario esta siendo usado por otro*/
  if(nick){
    free(nick);
    nick = NULL;
  }
  if(prefix){
    free(prefix);
    prefix = NULL;
  }
  if(msg){
    free(msg);
    msg = NULL;
  }
}

/**
* @page server_command_user server_command_user
* @brief Función handler del comando USER del IRC.
* @section synopsis_user Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_user(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_user Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>register_status:</b> Comprueba que el valor sea igual a 1 (es decir,
* el usuario ha parseado el NICK de forma correcta previamente).</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Creación de usuario</b> Es decir, añade el usuario a la lista
* de usuarios y lo registra de forma correcta mediante la función del TAD
* <b>IRCTADUser_New</b>.
* <br>Condición necesaria: El valor de register_status tiene que ser 1 (es
* decir, nick <b>parseado</b> correctamente de forma previa).
* </li>
* <li><b>Devuelve mensaje:</b> Es decir, en caso de que se haya registrado
* de forma correcta, se devuelve al usuario un mensaje de bienvenida mediante
* la función del TAD <b>IRCMsg_RplWelcome</b> mediante el descriptor del socket
* y la función <b>send()</b>.
* <br>Condición necesaria: La función <b>IRCMsg_RplWelcome</b> debe devolver IRC_OK.
* </li>
* </ul>
* <p>Tras finalizar este flujo, la función actualiza el valor del puntero de
* <b>register_status</b> a 2, es decir, confirma que el usuario se ha registrado de forma
* correcta para futuras comprobaciones.
* @section return_user Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_user Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_user(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *user = NULL, *realname = NULL, *prefix = NULL, *host = NULL, *IP = NULL, *msg = NULL;
  struct sockaddr_in addr;
  struct hostent *he;
  socklen_t size_address = sizeof(addr);
  getpeername(desc, (struct sockaddr *)&addr, &size_address);
  IP = inet_ntoa(addr.sin_addr);
  he = gethostbyaddr((char *)&addr.sin_addr, sizeof(addr.sin_addr), AF_INET);
  if((*register_status)==1){
    syslog(LOG_INFO, "[DESC: %d] -----> EXECUTE USER: %s", desc, nick_static);
    syslog(LOG_INFO, "[DESC: %d] USER: Valor de control previo %d", desc, *register_status);
    if(IRCParse_User(command, &prefix, &user, &host, &IP, &realname)==IRC_OK){
      if (IRCTADUser_New(user, nick_static, realname, NULL, he->h_name, IP, desc)==IRC_OK){
        if(IRCMsg_RplWelcome(&msg, "ip.servidor", nick_static, nick_static, user, "localhost")==IRC_OK){
          if(server_ssl){
            enviar_datos_SSL(ssl, msg);
          } else {
            send(desc, msg, strlen(msg), 0);
          }
          IRC_MFree(1, &msg);
          *register_status = 2;
        }
      }
    }
  }
  syslog(LOG_INFO, "[DESC: %d] EXECUTE USER: Valor de control posterior %d", desc, *register_status);
  IRC_MFree(6, &user, &realname, &prefix, &host, &IP, &msg);
}

/**
* @page server_command_join server_command_join
* @brief Función handler del comando JOIN del IRC.
* @section synopsis_join Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_join(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_join Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando JOIN sea correcto,
* en caso contrario devuelve un mensaje de error generado por la función del TAD
* <b>IRCMsg_ErrNeedMoreParams</b> que se envía mediante el descriptor del socket
* del cliente recibido por el parámetro <b>int desc</b>.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Comprueba el modo del canal:</b> Es decir, comprueba que el canal no
* esté protegido por una clave de acceso mediante la función <b>IRCTADChan_GetModeInt</b>.
* En caso de que si tenga clave, se comprueba que se haya introducido una clave
* correcta, en caso de ser incorrecta se devuelve un mensaje generado con la función
* del TAD <b>IRCMsg_ErrBadChannelKey</b>.
* </li>
* <li><b>Realiza el JOIN en el canal:</b> Es decir, asigna el canal al usuario
* de forma correcta, se devuelve al usuario un mensaje de bienvenida mediante
* la función del TAD <b>IRCMsg_RplWelcome</b> mediante el descriptor del socket
* y la función <b>send()</b>.
* <li><b>Devuelve mensaje:</b> Es decir, en caso de que se haya registrado
* de forma correcta, se devuelve al usuario un mensaje de bienvenida mediante
* la función del TAD <b>IRCMsg_Join</b> mediante el descriptor del socket
* y la función <b>send()</b>.
* <br>Condición necesaria: En caso de tener clave, debe ser correcta.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_join Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_join Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_join(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *key = NULL, *prefix = NULL, *msg = NULL, *channel = NULL, *unknown_user = NULL, *unknown_nick = NULL, *unknown_real = NULL, *server = NULL;
  long unknown_id = 0, find_id = 0, find_creationTS, find_actionTS, creationTS, actionTS, numberOfUsers;
  char *host, *IP, *away, *modeFlag = malloc(sizeof(char)*2), **arrayNicks;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int i, find_socket = 0, response, logic_comparator, flag_login_possible = 0;
  if (IRCParse_Join(command, &prefix, &channel, &key, &msg) != IRC_OK){
  	IRCMsg_ErrNeedMoreParams(&msg, "ip.servidor", nick_static, command);
  	send(desc, msg, strlen(msg), 0);
  } else {
    if((response=server_channels_find_by_name(channel))==0){
      strcpy(modeFlag, "o");
    }
    logic_comparator = IRCTADChan_GetModeInt(channel);
    if((logic_comparator&IRCMODE_CHANNELPASSWORD)==IRCMODE_CHANNELPASSWORD){
      syslog(LOG_INFO, "JOIN: NECESITA CLAVE");
      if(IRCTADChan_TestPassword(channel, key)!=IRC_OK){
        if(IRCMsg_ErrBadChannelKey(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
        }
        IRC_MFree(1, &msg);
      } else {
        syslog(LOG_INFO, "JOIN: CLAVE CORRECTA");
        flag_login_possible = 1;
      }
    } else {
      flag_login_possible = 1;
    }
    if(flag_login_possible){
      syslog(LOG_INFO, "JOIN: HACE JOIN EN CANAL: %s CON CLAVE: %s", channel, key);
      if(IRCTAD_Join(channel, nick_static, modeFlag, key)==IRC_OK){
        if(IRCTADUser_GetData(&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, &desc, &creationTS, &actionTS, &away)==IRC_OK){
          syslog(LOG_INFO, "JOIN: %s %s", server, host);
          if(IRC_Prefix(&prefix, nick_static, unknown_user, NULL, host)==IRC_OK){
            syslog(LOG_INFO, "JOIN PREFIX: %s", prefix);
        		if(IRCMsg_Join(&msg, prefix + 1, NULL, NULL, channel)==IRC_OK){
              /* Enviar aviso a todos los usuarios */
              IRCTAD_ListNicksOnChannelArray(channel, &arrayNicks, &numberOfUsers);
              for(i=0; i< numberOfUsers; i++){
                if(IRCTADUser_GetData(&find_id, &find_user, &arrayNicks[i], &find_real, &find_host, &find_ip,
                  &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
                  send(find_socket, msg, strlen(msg), 0);
                  IRC_MFree(5, &find_user, &find_real, &find_host, &find_ip, &find_away);
                  find_socket = 0;
                  find_id = 0;
                }
              }
              IRCTADChan_FreeList(arrayNicks, numberOfUsers);
            }
            IRC_MFree(1, &msg);
          }
        }
        IRC_MFree(5, &unknown_user, &unknown_nick, &unknown_real, &server, &host, &IP, &away);
      }
    }
  }
  IRC_MFree(5, &msg, &key, &modeFlag, &prefix, &channel);
}

/**
* @page server_command_quit server_command_quit
* @brief Función handler del comando QUIT del IRC.
* @section synopsis_quit Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_quit(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_quit Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando QUIT sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Elimina al usuario:</b> Es decir, elimina los datos del usuario
* existentes en el TAD mediante la propia función del TAD <b>IRCTAD_Quit</b>.
* En caso de que se realice de forma correcta, se devuelve un mensaje
* de confirmación al descriptor del cliente mediante la función del TAD
* <b>IRCMsg_Quit</b>.
* </li>
* <li><b>Cierre del socket:</b> Es decir, cierra la conexión del descriptor
* del socket del cliente mediante la función <b>close()</b>.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_quit Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_quit Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_quit(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *user = NULL, *realname = NULL, *prefix = NULL, *host = NULL, *IP = NULL, *msg = NULL, *msg2 = NULL;
  syslog(LOG_INFO, "-----> EXECUTE QUIT: %s", nick_static);
  if(IRCParse_Quit(command, &prefix, &msg2)==IRC_OK){
    IRCTAD_Quit(nick_static);
    if(IRCMsg_Quit(&msg, prefix, msg2)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      syslog(LOG_INFO, "MSG: %s", msg);
      close(desc);
    }
  }
  *register_status = 0;
  IRC_MFree(7, &user, &realname, &host, &IP, &prefix, &msg, &msg2);
}
/**
* @page server_command_ping server_command_ping
* @brief Función handler del comando PING del IRC.
* @section synopsis_ping Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_ping(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_ping Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando PING sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Recibe ping:</b> Es decir, recibe de un usuario el comando PING
* con el que el usuario espera recibir un PONG con la misma cadena recibida.
* </li>
* <li><b>Envío pong:</b> Es decir, se devuelve al usuario el PONG generado
* por la función del TAD <b>IRCMsg_Pong</b> al descriptor del socket
* del cliente utilizando la función <b>send()</b>.</li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_ping Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_ping Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_ping(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *ping = NULL, *pong = NULL, *prefix = NULL, *msg = NULL;
  if(IRCParse_Ping(command, &prefix, &ping, &pong, &msg)==IRC_OK){
  	if(IRCMsg_Pong(&command, "ip.servidor", ping, pong, ping)==IRC_OK){
  		send(desc, command, strlen(command), 0);
  	}
  }
  IRC_MFree(4, &ping, &pong, &prefix, &msg);
}

/**
* @page server_command_list server_command_list
* @brief Función handler del comando LIST del IRC.
* @section synopsis_list Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_list(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_list Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando LIST sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Recupera la lista de canales:</b> Es decir, solicita un array de canales
* al TAD utilizando la función <b>IRCTADChan_GetList</b> a la que se le pasan
* los parámetros <b>char *** channels</b> y <b>int *size</b> para tener los datos.
* </li>
* <li><b>Devuelve mensaje de inicio:</b> Es decir, se devuelve al usuario un mensaje
* generado por la función del TAD <b>IRCMsg_RplListStart</b> utilizando el
* descriptor del socket del cliente y la función <b>send()</b>.</li>
* <li><b>Genera la lista de canales:</b> Es decir, se devuelve al usuario la lista
* de canales utilizando el descriptor del socket del cliente y la función <b>send()</b>.
* Para ello, se realizan el siguiente flujo sobre la lista adquirida de canales:
* <ul>
* <li><b>Comprobación del modo del canal:</b> Antes de enviar el canal se comprueba el modo,
* únicamente se enviará la lista de canales que sean públicos, nunca los secretos.</li>
* <li><b>Nombre del canal:</b> Se devuelve el nombre del canal.</li>
* <li><b>Número de usuarios:</b> Junto al nombre del canal se devuelve el número
* de usuarios que están en ese momento en el canal.</li>
* <li><b>Topic del canal:</b> Junto al nombre del canal y el número de usuarios,
* se devuelve también el topic del canal actual.</li>
* </ul>
* </li>
* <li><b>Devuelve mensaje de fin:</b> Es decir, se devuelve al usuario un mensaje
* generado por la función del TAD <b>IRCMsg_RplListEnd</b> indicándole que se ha
* finalizado con la comunicación de la lista de canales.</li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_list Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_list Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_list(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, **channels = NULL, *topic = NULL, visible[50];
  long size;
  int i;
  strcpy(visible, "");
  IRCTADChan_GetList(&channels, &size, NULL);
  if(IRCMsg_RplListStart(&msg, "ip.servidor", nick_static)==IRC_OK){
    send(desc, msg, strlen(msg), 0);
    for (i = 0; i < size; i++) {
      if (IRCTADChan_GetModeInt(channels[i])!=IRCMODE_SECRET) {
        if(IRCTAD_GetTopic(channels[i], &topic)==IRC_OK){
          sprintf(visible, "%ld", IRCTADChan_GetNumberOfUsers(channels[i]));
          IRCMsg_RplList(&msg, "ip.servidor", nick_static, channels[i], visible, topic);
          send(desc, msg, strlen(msg), 0);
          IRC_MFree(1, &msg);
        }
      }
    }
    if(IRCMsg_RplListEnd(&msg, "ip.servidor", nick_static)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
  }
  IRCTADChan_FreeList(channels, size);
  IRC_MFree(3, &prefix, &msg, &topic);
}

/**
* @page server_command_privmsg server_command_privmsg
* @brief Función handler del comando PRIVMSG del IRC.
* @section synopsis_privmsg Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_privmsg(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_privmsg Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando PRIVMSG sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Si el objetivo es un canal:</b> Si el objetivo del mensaje es un canal
* se recupera la lista de usuarios de dicho canal mediante la función del TAD
* <b>IRCTAD_ListNicksOnChannelArray</b> y se envía el mensaje a todos los usuarios
* de dicho canal exceptuando al usuario que envía el mensaje.
* </li>
* <li><b>Si el objetivo es un usuario:</b> Si el objetivo del mensaje es un usuario
* se envía un mensaje al usuario destinatario (target) recuperando su información
* mediante la función del TAD <b>IRCTADUser_GetData</b>. Se comprueba si el usuario
* al que se desea enviar el mensaje está <b>AWAY</b> con la función del TAD
* <b>IRCTADUser_GetAway</b>, en caso afirmativo se devuelve un mensaje al usuario
* que envía el mensaje informándole de que el usuario destinatario está ausente.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_privmsg Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_privmsg Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_privmsg(char* command, int desc, SSL *ssl, char *nick_static, int* register_status){
  char *prefix = NULL, *msgtarget = NULL, *msg = NULL, *awayMsg = NULL, *msgSend = NULL, *msgSend2 = NULL, *tmpUser = NULL;
  long find_id = 0, find_creationTS, find_actionTS, numberOfUsers;
  char **arrayNicks;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int i, find_socket = 0;
  if(IRCParse_Privmsg(command, &prefix, &msgtarget, &msg)==IRC_OK){
    if(msgtarget[0]=='#'){
      /* Es canal */
      IRCTAD_ListNicksOnChannelArray(msgtarget, &arrayNicks, &numberOfUsers);
      for(i=0; i< numberOfUsers; i++){
        if(strcmp(nick_static, arrayNicks[i])!=0){
          tmpUser = server_return_user(nick_static);
          if(IRCTADUser_GetData(&find_id, &find_user, &arrayNicks[i], &find_real, &find_host, &find_ip, &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
            if(IRC_Prefix(&prefix, nick_static, tmpUser, NULL, "LOCALHOST")==IRC_OK){
              if(IRCMsg_Privmsg(&msgSend, prefix+1, msgtarget, msg)==IRC_OK){
                syslog(LOG_INFO, "-----> EXECUTE PRIVMSG: Socket: %d", find_socket);
                send(find_socket, msgSend, strlen(msgSend), 0);
              }
              IRC_MFree(6, &find_user, &find_real, &find_host, &find_ip, &find_away, &msgSend);
              find_socket = find_id = 0;
            }
          }
          IRC_MFree(2, &tmpUser, &prefix);
        }
      }
      IRCTADChan_FreeList(arrayNicks, numberOfUsers);
    } else {
      /* Es usuario */
      if(IRCTADUser_GetData(&find_id, &find_user, &msgtarget, &find_real, &find_host, &find_ip, &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
        if(IRC_Prefix(&prefix, nick_static, find_user, NULL, "LOCALHOST")==IRC_OK){
          if(IRCMsg_Privmsg(&msgSend2, prefix+1, msgtarget, msg)==IRC_OK){
            if(IRCTADUser_GetAway(0, NULL, msgtarget, NULL, &awayMsg)==IRC_OK){
              if(awayMsg!=NULL){
                if(IRCMsg_RplAway(&msgSend, "ip.servidor", nick_static, msgtarget, awayMsg)==IRC_OK){
                  send(desc, msgSend, strlen(msgSend), 0);
                }
                if(msgSend){
                  free(msgSend);
                  msgSend = NULL;
                }
              }
            }
            syslog(LOG_INFO, "-----> EXECUTE PRIVMSG: Socket: %d", find_socket);
            send(find_socket, msgSend2, strlen(msgSend2), 0);
            IRC_MFree(8, &find_user, &find_real, &find_host, &find_ip, &find_away, &prefix, &msgSend2, &awayMsg);
            find_socket = find_id = 0;
          }
        }
      } else {
        IRCMsg_ErrNoSuchNick(&msgSend, "ip.servidor", nick_static, msgtarget);
        send(desc, msgSend, strlen(msgSend), 0);
        if(msgSend){
          free(msgSend);
          msgSend = NULL;
        }
      }
    }
  }
  IRC_MFree(3, &prefix, &msgtarget, &msg);
}

/**
* @page server_command_part server_command_part
* @brief Función handler del comando PART del IRC.
* @section synopsis_part Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_part(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_part Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando PART sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Comprobación del canal:</b> Se comprueba que el canal exista en el TAD
* obteniendo la lista de canales, en caso de que no exista se devuelve un mensaje
* de error al usuario.
* </li>
* <li><b>Salida del canal:</b> Si el canal existe, se abandona el canal por parte
* del usuario utilizando la función del TAD <b>IRCTAD_Part</b> y se devuelve un
* mensaje de confirmación al usuario.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_part Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_part Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_part(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, **channels, *target_channel, *unknown_user = NULL, *unknown_nick = NULL, *unknown_real = NULL, *server = NULL;
  long unknown_id = 0, find_id = 0, find_creationTS, find_actionTS, creationTS, actionTS, numberOfUsers;
  char *host, *IP, *away, **arrayNicks;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int i, find_socket = 0, response;
  long size;
  syslog(LOG_INFO, "-----> EXECUTE PART: %s", command);
  if(IRCParse_Part(command, &prefix, &target_channel, &msg)==IRC_OK){
    IRCTADChan_GetList(&channels, &size, NULL);
  	if(size < 1) {
      IRCMsg_ErrNoSuchChannel(&msg, "ip.servidor", nick_static, target_channel);
      send(desc, msg, strlen(msg), 0);
      free(msg);
    } else {
      IRCTAD_ListNicksOnChannelArray(target_channel, &arrayNicks, &numberOfUsers);
      if(IRCTAD_Part(target_channel, nick_static)==IRC_OK){
        if(IRCTADUser_GetData(&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, &desc, &creationTS, &actionTS, &away)==IRC_OK){
          syslog(LOG_INFO, "PART: %s %s", server, host);
          if(IRC_Prefix(&prefix, nick_static, unknown_user, NULL, host)==IRC_OK){
            syslog(LOG_INFO, "PART PREFIX: %s", prefix);
        		if(IRCMsg_Part(&msg, prefix + 1, target_channel, msg)==IRC_OK){
              /* Enviar aviso a todos los usuarios */
              for(i=0; i< numberOfUsers; i++){
                if(IRCTADUser_GetData(&find_id, &find_user, &arrayNicks[i], &find_real, &find_host, &find_ip,
                  &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
                  send(find_socket, msg, strlen(msg), 0);
                  IRC_MFree(5, &find_user, &find_real, &find_host, &find_ip, &find_away);
                  find_socket = 0;
                  find_id = 0;
                }
              }
            }
            IRC_MFree(1, &msg);
          }
        }
        IRC_MFree(5, &unknown_user, &unknown_nick, &unknown_real, &server, &host, &IP, &away);
      } else {
        IRCMsg_ErrNoSuchChannel(&msg, "ip.servidor", nick_static, target_channel);
        send(desc, msg, strlen(msg), 0);
        free(msg);
      }
      IRCTADChan_FreeList(arrayNicks, numberOfUsers);
    }
    IRCTADChan_FreeList(channels, size);
  }
}

/**
* @page server_command_names server_command_names
* @brief Función handler del comando NAMES del IRC.
* @section synopsis_names Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_names(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_names Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando NAMES sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Lista de usuarios del canal:</b> Se solicita la lista de usuarios en el
* canal mediante la función del TAD <b>IRCTAD_ListNicksOnChannelArray</b>. Se
* devuelven todos los usuarios del canal comprobando además el modo del usuario
* en dicho canal, en caso de ser el operador se coloca un @ al comienzo del nombre.
* </li>
* <li><b>Mensaje final:</b> Se envía un mensaje de confirmación al usuario
* para indicarle de que se ha finalizado el envío de la lista de usuarios del
* canal mediante la función del TAD <b>IRCMsg_RplEndOfNames</b>.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_names Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_names Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_names(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *channel, *target, **arrayNicks, string_nicks[100], type[7] = "=";
  long numberOfUsers;
  int i;
  syslog(LOG_INFO, "-----> EXECUTE NAMES: %s", command);
  memset(string_nicks, 0, 100);
  if(IRCParse_Names(command, &prefix, &channel, &target)==IRC_OK){
    if(IRCTAD_ListNicksOnChannelArray(channel, &arrayNicks, &numberOfUsers)==IRC_OK){
      for(i = 0; i < numberOfUsers; i++) {
        if (i > 0){
          strcat(string_nicks, " ");
        }
        if((IRCTAD_GetUserModeOnChannel(channel, arrayNicks[i])&2)==2){
          strcat(string_nicks, "@");
        }
        strcat(string_nicks, arrayNicks[i]);
      }
      IRCTADChan_FreeList(arrayNicks, numberOfUsers);
    }
    if(strcmp(string_nicks, "")!=0){
      if(IRCMsg_RplNamReply(&msg, "ip.servidor", nick_static, type, channel, string_nicks)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
        free(msg);
      }
    }
    if(IRCMsg_RplEndOfNames(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
      free(msg);
    }
  }
}

/**
* @page server_command_kick server_command_kick
* @brief Función handler del comando KICK del IRC.
* @section synopsis_kick Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_kick(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_kick Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando KICK sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Modo del usuario en el canal:</b> Se comprueba los permisos del usuario
* para verificar que sea el operador del canal, en caso afirmativo se procede a
* expulsar al usuario destinatario del kick mediante la función del TAD
* <b>IRCTAD_KickUserFromChannel</b>. En caso de que no sea operador, se devuelve
* al usuario un mensaje de error generado por la función del TAD
* <b>IRCMsg_ErrChanOPrivsNeeded</b>.
* </li>
* <li><b>Mensaje final:</b> Se envía un mensaje de confirmación al usuario
* para indicarle de que se ha realizado la expulsión de forma correcta
* mediante la función del TAD <b>IRCMsg_Kick</b>.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_kick Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_kick Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_kick(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *channel, *nick, *comment;
  long response, find_id = 0, find_creationTS, find_actionTS;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int find_socket = 0;
  syslog(LOG_INFO, "-----> EXECUTE KICK: %s", command);
  if(IRCParse_Kick(command, &prefix, &channel, &nick, &comment)==IRC_OK){
    if((IRCTAD_GetUserModeOnChannel(channel, nick_static)&2)==2){
      response = IRCTAD_KickUserFromChannel(channel, nick);
      if(response==IRC_OK){
        if(IRCTADUser_GetData(&find_id, &find_user, &nick, &find_real, &find_host, &find_ip, &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
          if(IRCMsg_Kick(&msg, "ip.servidor", channel, nick, comment)==IRC_OK){
            send(find_socket, msg, strlen(msg), 0);
            free(msg);
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
        if(prefix)
          free(prefix);
        find_user = find_real = find_host = find_away = NULL;
        find_socket = find_id = 0;
      } else if(response==IRCERR_NOVALIDUSER){
        if(IRCMsg_ErrNoLogin(&msg, "ip.servidor", nick_static, nick)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
        }
      } else if(response==IRCERR_NOVALIDCHANNEL){
        if(IRCMsg_ErrNoSuchChannel(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
        }
      }
    } else {
      if(IRCMsg_ErrChanOPrivsNeeded(&msg, "ip.servidor", nick_static, channel)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
        free(msg);
      }
    }
  }
}

/**
* @page server_command_mode server_command_mode
* @brief Función handler del comando MODE del IRC.
* @section synopsis_mode Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_mode(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_mode Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando MODE sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Información del modo actual del canal:</b> Si no se especifica un MODE
* el comando devuelve el modo actual del canal al usuario. Se han contemplado
* tres modos (los más importantes) a la hora de realizar el comando:
* <b>IRCMODE_TOPICOP</b>, <b>IRCMODE_SECRET</b> y <b>IRCMODE_CHANNELPASSWORD</b>.
* </li>
* <li><b>Modificar modo del canal:</b> La otra opción del comando es actualizar el
* modo de un canal, para ello se introduce una string representando al modo (
* +s para canal secreto, +k para añadir una clave de acceso y +t para proteger
* el cambio del TOPIC únicamente al operador del canal). El modo se actualiza
* utilizando la función del TAD <b>IRCTAD_Mode</b>.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_mode Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_mode Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_mode(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *channel = NULL, *mode = NULL, *user = NULL, *mode_arg = NULL;
  long response = 0, logic_comparator;
  long find_id = 0, find_creationTS, find_actionTS;
  char *find_nick = NULL, *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int find_socket = 0;
  syslog(LOG_INFO, "-----> EXECUTE MODE: %s", command);
  if((response = IRCParse_Mode(command, &prefix, &channel, &mode, &user))==IRC_OK){
    if(IRCTADUser_GetData(&find_id, &find_user, &find_nick, &find_real, &find_host, &find_ip, &desc, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
      if(IRC_Prefix(&prefix, find_nick, find_user, NULL, find_host)==IRC_OK){
        syslog(LOG_INFO, "-----> Prefix: %s", prefix);
      }
      IRC_MFree(6, &find_nick, &find_user, &find_real, &find_host, &find_ip, &find_away);
    }
    if(mode){
      if(
        (strcmp(mode, "+s")==0)||
        (strcmp(mode, "-s")==0)||
        (strcmp(mode, "+t")==0)||
        (strcmp(mode, "-t")==0)||
        (strcmp(mode, "+n")==0)||
        (strcmp(mode, "-n")==0)||
        (strcmp(mode, "+i")==0)||
        (strcmp(mode, "-i")==0)||
        (strcmp(mode, "+m")==0)||
        (strcmp(mode, "-m")==0)||
        (strcmp(mode, "+p")==0)||
        (strcmp(mode, "-p")==0)
      ){
        if(IRCTAD_Mode(channel, nick_static, mode)==IRC_OK){
          if(IRCMsg_Mode(&msg, prefix + 1, channel, mode, user)==IRC_OK){
            syslog(LOG_INFO, "-----> EXECUTE MODE: Modo reconocido");
            send(desc, msg, strlen(msg), 0);
            free(msg);
            msg = NULL;
          }
        }
      } else if((strcmp(mode, "\\+k")==0)||(strcmp(mode, "+k")==0)){
        mode_arg = malloc(sizeof(user)+6);
        strcpy(mode_arg, "");
        strcat(mode_arg, "+k ");
        strcat(mode_arg, user);
        if(IRCTAD_Mode(channel, nick_static, mode_arg)==IRC_OK){
          if(IRCMsg_Mode(&msg, prefix + 1, channel, mode, user)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
            free(msg);
            msg = NULL;
          }
        }
      } else {
        if(IRCMsg_ErrUnknownMode(&msg, &prefix + 1, nick_static, mode, channel)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
          msg = NULL;
        }
      }
    } else {
      logic_comparator = IRCTADChan_GetModeInt(channel);
      if((logic_comparator&IRCMODE_TOPICOP)==IRCMODE_TOPICOP){
        if(IRCMsg_RplChannelModeIs(&msg, prefix + 1, nick_static, channel, "+t")==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
          msg = NULL;
        }
      } else if((logic_comparator&IRCMODE_SECRET)==IRCMODE_SECRET){
        if(IRCMsg_RplChannelModeIs(&msg, prefix + 1, nick_static, channel, "+s")==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
          msg = NULL;
        }
      } else if((logic_comparator&IRCMODE_CHANNELPASSWORD)==IRCMODE_CHANNELPASSWORD){
        if(IRCMsg_RplChannelModeIs(&msg, prefix + 1, nick_static, channel, "\\+k")==IRC_OK){
          send(desc, msg, strlen(msg), 0);
          free(msg);
          msg = NULL;
        }
      }
    }
  } else if (response==IRCERR_NOSTRING){
    syslog(LOG_INFO, "-----> EXECUTE MODE: IRCERR_NOSTRING");
  } else if (response==IRCERR_ERRONEUSCOMMAND){
    syslog(LOG_INFO, "-----> EXECUTE MODE: IRCERR_ERRONEUSCOMMAND");
  }
  if(channel){
    free(channel);
    channel = NULL;
  }
  if(mode){
    free(mode);
    mode = NULL;
  }
  if(user){
    free(user);
    user = NULL;
  }
  if(mode_arg){
    free(mode_arg);
    mode_arg = NULL;
  }
  if(prefix){
    free(prefix);
    prefix = NULL;
  }
}

/**
* @page server_command_away server_command_away
* @brief Función handler del comando AWAY del IRC.
* @section synopsis_away Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_away(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_away Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando AWAY sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Activa el modo away:</b> Si se especifica un mensaje de AWAY, la función
* asigna el mensaje y el modo away al usuario y le devuelve un mensaje de confirmación
* utilizando la función del TAD <b>IRCMsg_RplNowAway</b>.
* </li>
* <li><b>Desactiva el modo away:</b> Si no se especifica un mensaje de AWAY, la función
* elimina el mensaje y el modo away al usuario y le devuelve un mensaje de confirmación
* utilizando la función del TAD <b>IRCMsg_RplUnaway</b>.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_away Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_away Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_away(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *msg_away = NULL;
  syslog(LOG_INFO, "-----> EXECUTE AWAY: %s", command);
  if(IRCParse_Away(command, &prefix, &msg_away)==IRC_OK){
    server_channels_update_away(nick_static, msg_away);
    if(msg_away!=NULL){
      if(IRCMsg_RplNowAway(&msg, "ip.servidor", nick_static)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
      }
      IRC_MFree(1, &msg);
    } else {
      if(IRCMsg_RplUnaway(&msg, "ip.servidor", nick_static)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
      }
      IRC_MFree(1, &msg);
    }
  }
  IRC_MFree(2, &prefix, &msg_away);
}

/**
* @page server_command_whois server_command_whois
* @brief Función handler del comando WHOIS del IRC.
* @section synopsis_whois Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_whois(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_whois Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando WHOIS sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Devolución de información del usuario:</b> Si se especifica un usuario,
* la función devuelve la información de dicho usuario mediante un mensaje generado
* con la función del TAD <b>IRCMsg_RplWhoIsUser</b>.
* </li>
* <li><b>Mensaje de error:</b> Si no se especifica un usuario para el WHOIS, la función
* devuelve al usuario un mensaje de confirmación utilizando la función
* del TAD <b>IRCMsg_ErrNoNickNameGiven</b>.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_whois Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_whois Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_whois(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *target = NULL, *maskarray = NULL, *awayMsg = NULL, **arrayChannels = NULL, *stringsWhois = NULL;
  long find_id = 0, find_creationTS, find_actionTS, numberOfChannels = 0;
  char *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int find_socket = 0, i;
  syslog(LOG_INFO, "-----> EXECUTE WHOIS: %s", command);
  if(IRCParse_Whois(command, &prefix, &target, &maskarray)==IRC_OK){
    if(IRCTADUser_GetAway(0, NULL, maskarray, NULL, &awayMsg)==IRC_OK){
      if(awayMsg!=NULL){
        if(IRCMsg_RplAway(&msg, "ip.servidor", nick_static, maskarray, awayMsg)==IRC_OK){
          send(desc, msg, strlen(msg), 0);
        }
        if(msg){
          free(msg);
          msg = NULL;
        }
      } else {
        if(IRCTADUser_GetData(&find_id, &find_user, &maskarray, &find_real, &find_host, &find_ip,
          &find_socket, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
          if(IRCTAD_ListChannelsOfUserArray(find_user, maskarray, &arrayChannels, &numberOfChannels)==IRC_OK){
            syslog(LOG_INFO, "WHOIS: LISTA DE CANALES OK");
            if(IRCMsg_RplWhoIsUser(&msg, "ip.servidor", nick_static, maskarray, find_user, find_host, find_real)==IRC_OK){
              syslog(LOG_INFO, "WHOIS: mensaje ok");
              send(desc, msg, strlen(msg), 0);
            }
            if(msg){
              free(msg);
              msg = NULL;
            }
            stringsWhois = (char *) malloc(numberOfChannels*51*sizeof(char));
            strcpy(stringsWhois, "");

            for(i=0; i<numberOfChannels; i++){
              if(i>0){
                strcat(stringsWhois, " ");
              }
              if((IRCTAD_GetUserModeOnChannel(arrayChannels[i], maskarray)&2)==2){
                strcat(stringsWhois, "@");
              }
              strcat(stringsWhois, arrayChannels[i]);
            }

            if(IRCMsg_RplWhoIsChannels(&msg, "ip.servidor", nick_static, maskarray, stringsWhois)==IRC_OK){
              send(desc, msg, strlen(msg), 0);
            }
            IRC_MFree(1, &msg);

            if(IRCMsg_RplEndOfWhoIs(&msg, "ip.servidor", nick_static, maskarray)==IRC_OK){
              send(desc, msg, strlen(msg), 0);
            }
            IRC_MFree(2, &msg, &stringsWhois);
          } else {
            syslog(LOG_INFO, "WHOIS: ERROR AL OBTENER CANALES");
          }
          IRCTADChan_FreeList(arrayChannels, numberOfChannels);
          IRC_MFree(5, &find_user, &find_real, &find_ip, &find_host, &find_away);
          find_socket = find_id = 0;
          syslog(LOG_INFO, "WHOIS: USUARIO ENCONTRADO");
        }
      }
    }
  } else {
    if(IRCMsg_ErrNoNickNameGiven(&msg, "ip.servidor", nick_static)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
    if(msg){
      free(msg);
      msg = NULL;
    }
  }
  if(target){
    free(target);
    target = NULL;
  }
  if(maskarray){
    free(maskarray);
    maskarray = NULL;
  }
  if(awayMsg){
    free(awayMsg);
    awayMsg = NULL;
  }
}

/**
* @page server_command_topic server_command_topic
* @brief Función handler del comando TOPIC del IRC.
* @section synopsis_topic Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_topic(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_topic Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando TOPIC sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Actualizar el topic del canal:</b> Si se especifica un mensaje de TOPIC,
* la función asigna el nuevo topic al canal y le devuelve un mensaje de confirmación
* al usuario utilizando la función del TAD <b>IRCMsg_Topic</b>.
* </li>
* <li><b>Devuelve el topic del canal:</b> Si no se especifica un mensaje de TOPIC, la función
* devuelve el mensaje topic al usuario mediante un mensaje de confirmación
* utilizando la función del TAD <b>IRCMsg_RplTopic</b> o error en caso de que no exista
* un topic en el canal usando la función del TAD <b>IRCMsg_RplNoTopic</b>.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_topic Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_topic Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_topic(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *channel = NULL, *topic = NULL;
  long find_id = 0, find_creationTS, find_actionTS;
  char *find_nick = NULL, *find_user = NULL, *find_real = NULL, *find_host = NULL, *find_ip = NULL, *find_away = NULL;
  int find_socket = 0;
  syslog(LOG_INFO, "-----> EXECUTE TOPIC: %s", command);
  if(IRCParse_Topic(command, &prefix, &channel, &topic)==IRC_OK){
    if(IRCTADUser_GetData(&find_id, &find_user, &find_nick, &find_real, &find_host, &find_ip, &desc, &find_creationTS, &find_actionTS, &find_away)==IRC_OK){
      if(IRC_Prefix(&prefix, find_nick, find_user, NULL, find_host)==IRC_OK){
        syslog(LOG_INFO, "-----> Prefix: %s", prefix);
      }
      IRC_MFree(6, &find_nick, &find_user, &find_real, &find_host, &find_ip, &find_away);
    }
    syslog(LOG_INFO, "TOPIC a");
    if(topic&&channel){
      syslog(LOG_INFO, "TOPIC b");
      if((IRCTADChan_GetModeInt(channel)&IRCMODE_TOPICOP)==IRCMODE_TOPICOP){
        if(((IRCTAD_GetUserModeOnChannel(channel, nick_static)&1)==1)||((IRCTAD_GetUserModeOnChannel(channel, nick_static)&2)==2)){
          if(IRCTAD_SetTopic(channel, nick_static, topic)==IRC_OK){
            if(IRCMsg_Topic(&msg, prefix + 1, channel, topic)==IRC_OK){
              send(desc, msg, strlen(msg), 0);
            }
          }
        } else {
          if(IRCMsg_ErrChanOPrivsNeeded(&msg, prefix + 1, nick_static, channel)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
          }
        }
      } else {
        if(IRCTAD_SetTopic(channel, nick_static, topic)==IRC_OK){
          if(IRCMsg_Topic(&msg, prefix + 1, channel, topic)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
          }
        }
      }
    } else {
      syslog(LOG_INFO, "TOPIC c");
      if(IRCTAD_GetTopic(channel, &topic)==IRC_OK){
        if(!topic){
          if(IRCMsg_RplNoTopic(&msg, prefix + 1, nick_static, channel)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
          }
        } else {
          if(IRCMsg_RplTopic(&msg, prefix + 1, nick_static, channel, topic)==IRC_OK){
            send(desc, msg, strlen(msg), 0);
          }
        }
      }
    }
  }
  IRC_MFree(4, &prefix, &channel, &topic, &msg);
}

/**
* @page server_command_motd server_command_motd
* @brief Función handler del comando MOTD del IRC.
* @section synopsis_motd Synopsis
* <code>
* \b #include \b <G-2313-06-P3_function_handlers.h>
* <br>
* <b>void server_command_motd(char* command, int desc, SSL *ssl, char * nick_static, int* register_status)</b>
* </code>
* @section descripcion_motd Descripción
*
* <p>Esta función ejecuta el handler del comando en cuestión, para ello realiza
* una serie de comprobaciones previas:</p>
* <ul>
* <li><b>Parse:</b> Comprueba que el parseo del comando MOTD sea correcto.</li>
* </ul>
* <p>Una vez realizadas las comprobaciones, dicha función se ejecuta con el
* siguiente flujo:</p>
* <ul>
* <li><b>Devuelve el MOTD:</b> Se devuelve el mensaje diario (MOTD) al usuario
* utilizando la función del TAD <b>IRCMsg_RplMotd</b>.
* </li>
* </ul>
* <p>Al finalizar el flujo de la función se liberan los recursos utilizados.</p>
* @section return_motd Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors_motd Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_command_motd(char* command, int desc, SSL *ssl, char * nick_static, int* register_status){
  char *prefix = NULL, *msg = NULL, *target = NULL;
  syslog(LOG_INFO, "-----> EXECUTE MOTD: %s", command);
  if(IRCParse_Motd(command, &prefix, &target)==IRC_OK){
    if(IRCMsg_RplMotdStart(&msg, "ip.servidor", nick_static, "¡BIENVENIDO!")==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
    IRC_MFree(1, &msg);

    if(IRCMsg_RplMotd(&msg, "ip.servidor", nick_static, "¡BIENVENIDO!")==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
    IRC_MFree(1, &msg);

    if(IRCMsg_RplEndOfMotd(&msg, "ip.servidor", nick_static)==IRC_OK){
      send(desc, msg, strlen(msg), 0);
    }
    IRC_MFree(1, &msg);
  }
  IRC_MFree(2, &prefix, &target);
}
