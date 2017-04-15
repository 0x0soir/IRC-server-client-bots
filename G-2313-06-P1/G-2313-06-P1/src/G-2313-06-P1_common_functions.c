#include "../includes/G-2313-06-P1_common_functions.h"


/*! @page server_common_functions Funciones auxiliares
*
* <p>Esta sección incluye las funciones auxiliares utilizadas por los comandos
* para obtener datos de los usuarios como por ejemplo el nick, username, etc.
* Además incluye funciones para buscar coincidencias por un descriptor del socket
* de un cliente o si un canal existe con un nombre dado.</p>
* @section cabeceras2 Cabeceras
* <code>
* \b #include \b <G-2313-06-P1_common_functions.h>
* </code>
* @section functions2 Funciones implementadas
* <p>Se incluyen las siguientes funciones:
* <ul>
* <li>@subpage server_users_find_by_nick</li>
* <li>@subpage server_users_find_by_socket</li>
* <li>@subpage server_channels_find_by_name</li>
* <li>@subpage server_channels_update_nick</li>
* <li>@subpage server_channels_update_away</li>
* <li>@subpage server_return_user</li>
* </ul></p>
*/

/**
* @page server_users_find_by_nick server_users_find_by_nick
* @brief Busca un usuario por un nick dado.
* @section synopsis_server_users_find_by_nick Synopsis
* <code>
* \b #include \b <G-2313-06-P1_common_functions.h>
* <br>
* <b>int server_users_find_by_nick(char* data)</b>
* </code>
* @section descripcion_server_users_find_by_nick Descripción
*
* <p>Utiliza la función <b>IRCTADUser_GetData</b> del TAD para buscar
* un usuario que cumpla las características de nick recibidas como parámetro
* <b>char *data</b>. Devuelve un valor booleano.</p>
*
* @section return_server_users_find_by_nick Valores devueltos
* <ul>
* <li><b>int</b> Devuelve valor verdado/falso dependiendo de la búsqueda.</li>
* </ul>
*
* @section authors_server_users_find_by_nick Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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

/**
* @page server_users_find_by_socket server_users_find_by_socket
* @brief Busca un usuario por un socket dado.
* @section synopsis_server_users_find_by_socket Synopsis
* <code>
* \b #include \b <G-2313-06-P1_common_functions.h>
* <br>
* <b>int server_users_find_by_socket(int sockdesc)</b>
* </code>
* @section descripcion_server_users_find_by_socket Descripción
*
* <p>Utiliza la función <b>IRCTADUser_GetData</b> del TAD para buscar
* un usuario que cumpla las características de socket recibidas como parámetro
* <b>int sockdesc</b>.</p>
*
* @section return_server_users_find_by_socket Valores devueltos
* <ul>
* <li><b>int</b> Devuelve valor verdado/falso dependiendo de la búsqueda.</li>
* </ul>
*
* @section authors_server_users_find_by_socket Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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

/**
* @page server_channels_find_by_name server_channels_find_by_name
* @brief Busca un canal por un nombre dado.
* @section synopsis_server_users_find_by_name Synopsis
* <code>
* \b #include \b <G-2313-06-P1_common_functions.h>
* <br>
* <b>int server_channels_find_by_name(char *channel)</b>
* </code>
* @section descripcion_server_channels_find_by_name Descripción
*
* <p>Utiliza la función <b>IRCTADUser_GetData</b> del TAD para buscar
* un canal que cumpla las características de nombre recibidas como parámetro
* <b>char *channel</b>.</p>
*
* @section return_server_channels_find_by_name Valores devueltos
* <ul>
* <li><b>int</b> Devuelve valor verdado/falso dependiendo de la búsqueda.</li>
* </ul>
*
* @section authors_server_channels_find_by_name Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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

/**
* @page server_channels_update_nick server_channels_update_nick
* @brief Actualiza el nick de un usuario.
* @section synopsis_server_channels_update_nick Synopsis
* <code>
* \b #include \b <G-2313-06-P1_common_functions.h>
* <br>
* <b>int server_channels_update_nick(char *nick_old, char* nick_new)</b>
* </code>
* @section descripcion_server_channels_update_nick Descripción
*
* <p>Actualiza el nick de un usuario mediante la función del TAD
* <b>IRCTADUser_Set</b> para actualizar datos de los usuarios.
* </p><p>Comprueba previamente que el usuario exista mediante la función
* <b>IRCTADUser_GetData</b> utilizando los parámetros recibidos
* <b>char *nick_old</b> y <b>char *nick_new</b>.</p>
*
* @section return_server_channels_update_nick Valores devueltos
* <ul>
* <li><b>int</b> Devuelve valor verdado/falso dependiendo de la búsqueda.</li>
* </ul>
*
* @section authors_server_channels_update_nick Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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

/**
* @page server_channels_update_away server_channels_update_away
* @brief Actualiza el mensaje away de un usuario.
* @section synopsis_server_channels_update_away Synopsis
* <code>
* \b #include \b <G-2313-06-P1_common_functions.h>
* <br>
* <b>int server_channels_update_away(char* nick, char* away)</b>
* </code>
* @section descripcion_server_channels_update_away Descripción
*
* <p>Actualiza el mensaje away de un usuario mediante la función del TAD
* <b>IRCTADUser_SetAway</b> para actualizar los away de los usuarios mediante
* el parámetro recibido <b>char *away</b>.
* </p><p>Comprueba previamente que el usuario exista mediante la función
* <b>IRCTADUser_GetData</b> utilizando el parámetro recibido
* <b>char *nick</b>.</p>
*
* @section return_server_channels_update_away Valores devueltos
* <ul>
* <li><b>int</b> Devuelve valor verdado/falso dependiendo de la búsqueda.</li>
* </ul>
*
* @section authors_server_channels_update_away Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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

/**
* @page server_return_user server_return_user
* @brief Devuelve el username de un usuario.
* @section synopsis_server_return_user Synopsis
* <code>
* \b #include \b <G-2313-06-P1_common_functions.h>
* <br>
* <b>char* server_return_user(char* nick)</b>
* </code>
* @section descripcion_server_return_user Descripción
*
* <p>Devuelve el username de un usuario utilizando la función del TAD
* <b>IRCTADUser_GetData</b> para obtener información de los usuarios mediante
* el parámetro recibido <b>char *nick</b>.
* </p>
*
* @section return_server_return_user Valores devueltos
* <ul>
* <li><b>char*</b> Devuelve un puntero al username del usuario.</li>
* </ul>
*
* @section authors_server_return_user Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
char* server_return_user(char* nick){
  char *unknown_user = NULL, *unknown_real = NULL, *host = NULL, *IP = NULL, *away = NULL;
  long unknown_id = 0, creationTS, actionTS;
  int socket = NULL;
  IRCTADUser_GetData(&unknown_id, &unknown_user, &nick, &unknown_real, &host, &IP, &socket, &creationTS, &actionTS, &away);
  IRC_MFree(4, &unknown_real, &host, &IP, &away);
  return unknown_user;
}
