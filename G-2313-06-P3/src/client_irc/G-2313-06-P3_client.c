#include <redes2/ircxchat.h>
#include "../../includes/client_irc/G-2313-06-P3_client.h"

int socket_desc;
char* nick_cliente;
pthread_t thread_ficheros, thread_ping, thread_response;

/**
 * @defgroup IRCInterface Interface
 *
 */

/**
 * @defgroup IRCInterfaceCallbacks Callbaks
 * @ingroup IRCInterface
 *
 */

/**
 * @addtogroup IRCInterfaceCallbacks
 * Funciones que van a ser llamadas desde el interface y que deben ser implementadas por el usuario.
 * Todas estas funciones pertenecen al hilo del interfaz.
 *
 * El programador puede, por supuesto, separar todas estas funciones en múltiples ficheros a
 * efectos de desarrollo y modularización.
 *
 * <hr>
 */

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateChannelKey IRCInterface_ActivateChannelKey
 *
 * @brief Llamada por el botón de activación de la clave del canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateChannelKey (char *channel, char * key)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de la clave del canal. El segundo parámetro es
 * la clave del canal que se desea poner. Si es NULL deberá impedirse la activación
 * con la función implementada a tal efecto. En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a activar la clave.
 * @param[in] key clave para el canal indicado.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_ActivateChannelKey(char *channel, char *key)
{
  char *msg = NULL;
  if(channel&&key){
  	IRCMsg_Mode(&msg, NULL, channel, "+k", key);
  	IRCInterface_PlaneRegisterOutMessage(msg);

  	send(socket_desc, msg, strlen(msg), 0);

  	IRCInterface_SetChannelKey(key);

  	IRC_MFree(1, &msg);
  }else {
    IRCInterface_ErrorDialogThread("Para activar la clave del canal debes unirte a un canal primero e introducir una clave válida.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateExternalMessages IRCInterface_ActivateExternalMessages
 *
 * @brief Llamada por el botón de activación de la recepción de mensajes externos.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateExternalMessages (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de la recepción de mensajes externos.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la recepción de mensajes externos.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_ActivateExternalMessages(char *channel)
{
  char *msg = NULL;

  if(channel){
  	IRCMsg_Mode(&msg, NULL, channel, "+n", NULL);
  	IRCInterface_PlaneRegisterOutMessage(msg);

  	send(socket_desc, msg, strlen(msg), 0);

  	IRCInterface_SetExternalMessages();

  	IRC_MFree(1, &msg);
  }else {
    IRCInterface_ErrorDialogThread("Para activar los mensajes externos del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateInvite IRCInterface_ActivateInvite
 *
 * @brief Llamada por el botón de activación de canal de sólo invitación.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateInvite (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de canal de sólo invitación.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la invitación.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_ActivateInvite(char *channel)
{
  char *msg = NULL;

	IRCMsg_Mode(&msg, NULL, channel, "+i", NULL);
	IRCInterface_PlaneRegisterOutMessage(msg);

	send(socket_desc, msg, strlen(msg), 0);

	IRCInterface_SetInvite();

	IRC_MFree(1, &msg);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateModerated IRCInterface_ActivateModerated
 *
 * @brief Llamada por el botón de activación de la moderación del canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateModerated (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de la moderación del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la moderación.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_ActivateModerated(char *channel)
{
  char *msg = NULL;

  if(channel){
  	IRCMsg_Mode(&msg, NULL, channel, "+m", NULL);
  	IRCInterface_PlaneRegisterOutMessage(msg);

  	send(socket_desc, msg, strlen(msg), 0);

  	IRCInterface_SetModerated();

  	IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para activar la moderación del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateNicksLimit IRCInterface_ActivateNicksLimit
 *
 * @brief Llamada por el botón de activación del límite de usuarios en el canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateNicksLimit (char *channel, int * limit)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación del límite de usuarios en el canal. El segundo es el
 * límite de usuarios que se desea poner. Si el valor es 0 se sobrentiende que se desea eliminar
 * este límite.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará el límite de usuarios.
 * @param[in] limit límite de usuarios en el canal indicado.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_ActivateNicksLimit(char *channel, int limit)
{
  char *msg;
  char aux[10];

  if (channel && limit > 0) {
    sprintf(aux, "%d", limit);
    IRCMsg_Mode(&msg, NULL, channel, "+l", aux);

    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRCInterface_SetNicksLimit(limit);
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialog("Para activar el límite de nicks debes unirte a un canal e introducir un valor superior a 0.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivatePrivate IRCInterface_ActivatePrivate
 *
 * @brief Llamada por el botón de activación del modo privado.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivatePrivate (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación del modo privado.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar la privacidad.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_ActivatePrivate(char *channel)
{
  char *msg = NULL;

	IRCMsg_Mode(&msg, NULL, channel, "+p", NULL);
	IRCInterface_PlaneRegisterOutMessage(msg);

	send(socket_desc, msg, strlen(msg), 0);

	IRCInterface_SetPrivate();

	IRC_MFree(1, &msg);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateProtectTopic IRCInterface_ActivateProtectTopic
 *
 * @brief Llamada por el botón de activación de la protección de tópico.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateProtectTopic (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de la protección de tópico.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar la protección de tópico.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_ActivateProtectTopic(char *channel)
{
  char *msg = NULL;

  if(channel){
  	IRCMsg_Mode(&msg, NULL, channel, "+t", NULL);
  	IRCInterface_PlaneRegisterOutMessage(msg);

  	send(socket_desc, msg, strlen(msg), 0);

  	IRCInterface_SetProtectTopic();

  	IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para activar la protección del tema del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ActivateSecret IRCInterface_ActivateSecret
 *
 * @brief Llamada por el botón de activación de canal secreto.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateSecret (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de activación de canal secreto.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar el estado de secreto.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_ActivateSecret(char *channel)
{
  char *msg = NULL;

  if(channel){
  	IRCMsg_Mode(&msg, NULL, channel, "+s", NULL);
  	IRCInterface_PlaneRegisterOutMessage(msg);

  	send(socket_desc, msg, strlen(msg), 0);

  	IRCInterface_SetSecret();

  	IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para activar el secreto del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_BanNick IRCInterface_BanNick
 *
 * @brief Llamada por el botón "Banear".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_BanNick (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Banear". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a realizar el baneo. En principio es un valor innecesario.
 * @param[in] nick nick del usuario que va a ser baneado
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_BanNick(char *channel, char *nick)
{
  char *msg = NULL;

  if(channel && nick){
  	IRCMsg_Mode(&msg, NULL, channel, "+b", nick);
  	IRCInterface_PlaneRegisterOutMessage(msg);

  	send(socket_desc, msg, strlen(msg), 0);

  	IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para expulsar a alguien del canal debes unirte a un canal primero y seleccionar a un usuario de la lista.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_Connect IRCInterface_Connect
 *
 * @brief Llamada por los distintos botones de conexión.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	long IRCInterface_Connect (char *nick, char * user, char * realname, char * password, char * server, int port, boolean ssl)
 * @endcode
 *
 * @description
 * Función a implementar por el programador.
 * Llamada por los distintos botones de conexión. Si implementará la comunicación completa, incluido
 * el registro del usuario en el servidor.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leída.
 *
 *
 * @param[in] nick nick con el que se va a realizar la conexíón.
 * @param[in] user usuario con el que se va a realizar la conexión.
 * @param[in] realname nombre real con el que se va a realizar la conexión.
 * @param[in] password password del usuario si es necesaria, puede valer NULL.
 * @param[in] server nombre o ip del servidor con el que se va a realizar la conexión.
 * @param[in] port puerto del servidor con el que se va a realizar la conexión.
 * @param[in] ssl puede ser TRUE si la conexión tiene que ser segura y FALSE si no es así.
 *
 * @retval IRC_OK si todo ha sido correcto (debe devolverlo).
 * @retval IRCERR_NOSSL si el valor de SSL es TRUE y no se puede activar la conexión SSL pero sí una
 * conexión no protegida (debe devolverlo).
 * @retval IRCERR_NOCONNECT en caso de que no se pueda realizar la comunicación (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

long IRCInterface_Connect(char *nick, char *user, char *realname, char *password, char *server, int port, boolean ssl)
{
  struct sockaddr_in socket_address;
  struct hostent *server_info;
  char *command, *msgNick, *msgUser;
  syslog (LOG_INFO, "[CLIENTE] Inicia conexion");
  if(!nick||!user||!realname||!server){
    return IRCERR_NOCONNECT;
  } else {
    /* Se crea el socket que vamos a usar */
    if((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
      syslog(LOG_ERR, "[CLIENTE] ERROR creando socket");
      return IRCERR_NOCONNECT;
    } else {
      syslog(LOG_ERR, "[CLIENTE] Socket creado OK");
    }

    server_info = gethostbyname(server);
    if(server_info == NULL) {
      syslog(LOG_ERR, "[CLIENTE] ERROR obteniendo hostname");
      return IRCERR_NOCONNECT;
    } else {
      syslog(LOG_ERR, "[CLIENTE] Hostname obtenido OK");
    }

    bzero((char *) &socket_address, sizeof (socket_address));
    socket_address.sin_family = AF_INET;
    bcopy((char *) server_info->h_addr, (char *) &socket_address.sin_addr.s_addr, server_info->h_length);
    socket_address.sin_port = htons(port);

    if(connect(socket_desc, (struct sockaddr*) &socket_address, sizeof (socket_address)) < 0) {
      syslog(LOG_ERR, "[CLIENTE] ERROR conectando con el servidor");
      return IRCERR_NOCONNECT;
    } else {
      syslog(LOG_ERR, "[CLIENTE] Conexion con servidor OK");
    }
    pthread_create(&thread_response, NULL, client_function_response, NULL);

    /* Ya conectados, parseamos nick/user y lo enviamos */
    IRCMsg_Nick(&msgNick, NULL, nick, NULL);
    IRCMsg_User(&msgUser, NULL, user, server, realname);
    /*IRCMsg_Pass(&msgPass, NULL, password);*/
    syslog(LOG_ERR, "[CLIENTE] Password: %s", password);
    IRC_PipelineCommands(&command, msgNick, msgUser, NULL);
    syslog(LOG_ERR, "[CLIENTE] Registro pipelined: %s", command);

    if(send(socket_desc, command, strlen(command), 0) < 0) {
        return IRCERR_NOCONNECT;
    }
    IRCInterface_PlaneRegisterOutMessage(command);

    if(!nick_cliente){
      syslog(LOG_INFO, "[CLIENTE] Nuevo nick almacenado");
      nick_cliente = malloc(sizeof(nick));
      strcpy(nick_cliente, nick);
    }

    syslog(LOG_ERR, "[CLIENTE] Socket desc: %d", socket_desc);
    free(command);
  }

  pthread_create(&thread_ping, NULL, client_function_ping, NULL);

	return IRC_OK;
}


/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateChannelKey IRCInterface_DeactivateChannelKey
 *
 * @brief Llamada por el botón de desactivación de la clave del canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateChannelKey (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de la clave del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la clave.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_DeactivateChannelKey(char *channel)
{
  char *msg;

  if (channel) {
    IRCMsg_Mode(&msg, NULL, channel, "-k", NULL);

    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRCInterface_UnsetChannelKey();
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para desactivar esta opción del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateExternalMessages IRCInterface_DeactivateExternalMessages
 *
 * @brief Llamada por el botón de desactivación de la recepción de mensajes externos.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateExternalMessages (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de la recepción de mensajes externos.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a deactivar la recepción de mensajes externos.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_DeactivateExternalMessages(char *channel)
{
  char *msg;

  if (channel) {
    IRCMsg_Mode(&msg, NULL, channel, "-n", NULL);

    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRCInterface_UnsetExternalMessages();
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para desactivar esta opción del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateInvite IRCInterface_DeactivateInvite
 *
 * @brief Llamada por el botón de desactivación de canal de sólo invitación.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateInvite (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de canal de sólo invitación.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la invitación.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_DeactivateInvite(char *channel)
{
  char *msg;

  if (channel) {
    IRCMsg_Mode(&msg, NULL, channel, "-i", NULL);

    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRCInterface_UnsetInvite();
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para desactivar esta opción del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateModerated IRCInterface_DeactivateModerated
 *
 * @brief Llamada por el botón de desactivación  de la moderación del canal.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateModerated (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación  de la moderación del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la moderación.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_DeactivateModerated(char *channel)
{
  char *msg;

  if (channel) {
    IRCMsg_Mode(&msg, NULL, channel, "-m", NULL);

    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRCInterface_UnsetModerated();
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para desactivar esta opción del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateNicksLimit IRCInterface_DeactivateNicksLimit
 *
 * @brief Llamada por el botón de desactivación de la protección de tópico.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateNicksLimit (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación  del límite de usuarios en el canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar el límite de usuarios.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_DeactivateNicksLimit(char *channel)
{
  char *msg;

  if (channel) {
    IRCMsg_Mode(&msg, NULL, channel, "-l", NULL);

    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRCInterface_UnsetNicksLimit();
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para desactivar esta opción del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivatePrivate IRCInterface_DeactivatePrivate
 *
 * @brief Llamada por el botón de desactivación del modo privado.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivatePrivate (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación del modo privado.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @param[in] channel canal sobre el que se va a desactivar la privacidad.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_DeactivatePrivate(char *channel)
{
  char *msg;

  if (channel) {
    IRCMsg_Mode(&msg, NULL, channel, "-p", NULL);

    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRCInterface_UnsetPrivate();
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para desactivar esta opción del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateProtectTopic IRCInterface_DeactivateProtectTopic
 *
 * @brief Llamada por el botón de desactivación de la protección de tópico.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateProtectTopic (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de la protección de tópico.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la protección de tópico.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_DeactivateProtectTopic(char *channel)
{
  char *msg = NULL;

  if(channel){
  	IRCMsg_Mode(&msg, NULL, channel, "-t", NULL);
  	IRCInterface_PlaneRegisterOutMessage(msg);

  	send(socket_desc, msg, strlen(msg), 0);

  	IRCInterface_UnsetProtectTopic();

  	free(msg);
  } else {
    IRCInterface_ErrorDialogThread("Para desactivar esta opción del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DeactivateSecret IRCInterface_DeactivateSecret
 *
 * @brief Llamada por el botón de desactivación de canal secreto.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateSecret (char *channel)
 * @endcode
 *
 * @description
 * Llamada por el botón de desactivación de canal secreto.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la propiedad de canal secreto.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_DeactivateSecret(char *channel)
{
  char *msg;

  if (channel) {
    IRCMsg_Mode(&msg, NULL, channel, "-s", NULL);

    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);

    IRCInterface_UnsetSecret();
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para desactivar esta opción del canal debes unirte a un canal primero.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_DisconnectServer IRCInterface_DisconnectServer
 *
 * @brief Llamada por los distintos botones de desconexión.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	boolean IRCInterface_DisconnectServer (char * server, int port)
 * @endcode
 *
 * @description
 * Llamada por los distintos botones de desconexión. Debe cerrar la conexión con el servidor.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.

 * @param[in] server nombre o ip del servidor del que se va a realizar la desconexión.
 * @param[in] port puerto sobre el que se va a realizar la desconexión.
 *
 * @retval TRUE si se ha cerrado la conexión (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

boolean IRCInterface_DisconnectServer(char *server, int port)
{
  char *msg;

  IRCMsg_Quit(&msg, NULL, "Leaving");

  if(send(socket_desc, msg, strlen(msg), 0) < 0){
    return FALSE;
  }
  IRCInterface_PlaneRegisterOutMessage(msg);
  pthread_cancel(thread_ping);
  pthread_cancel(thread_response);
  shutdown(socket_desc, 2);
  socket_desc = 0;

  IRC_MFree(2, &msg, &nick_cliente);
	return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_ExitAudioChat IRCInterface_ExitAudioChat
 *
 * @brief Llamada por el botón "Cancelar" del diálogo de chat de voz.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ExitAudioChat (char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Parar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función cierrala comunicación. Evidentemente tiene que
 * actuar sobre el hilo de chat de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario que solicita la parada del chat de audio.
 *
 * @retval TRUE si se ha cerrado la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

boolean IRCInterface_ExitAudioChat(char *nick)
{
	return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_GiveOp IRCInterface_GiveOp
 *
 * @brief Llamada por el botón "Op".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_GiveOp (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Op". Previamente debe seleccionarse un nick del
 * canal para darle "op" a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va dar op al usuario.
 * @param[in] nick nick al que se le va a dar el nivel de op.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_GiveOp(char *channel, char *nick)
{
  char *msg;
  if (channel && nick) {
    IRCMsg_Mode(&msg, NULL, channel, "+o", nick);
    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para hacer eso debes unirte a un canal primero y seleccionar a un usuario de la lista.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_GiveVoice IRCInterface_GiveVoice
 *
 * @brief Llamada por el botón "Dar voz".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_GiveVoice (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Dar voz". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va dar voz al usuario.
 * @param[in] nick nick al que se le va a dar voz.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_GiveVoice(char *channel, char *nick)
{
  char *msg;
  if (channel && nick) {
    IRCMsg_Mode(&msg, NULL, channel, "+v", nick);
    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para hacer eso debes unirte a un canal primero y seleccionar a un usuario de la lista.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_KickNick IRCInterface_KickNick
 *
 * @brief Llamada por el botón "Echar".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_KickNick (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Echar". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a expulsar al usuario.
 * @param[in] nick nick del usuario que va a ser expulsado.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_KickNick(char *channel, char *nick)
{
  char *msg;
  if (channel && nick) {
    IRCMsg_Kick(&msg, NULL, channel, nick, NULL);
    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para hacer eso debes unirte a un canal primero y seleccionar a un usuario de la lista.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_NewCommandText IRCInterface_NewCommandText
 *
 * @brief Llamada la tecla ENTER en el campo de texto y comandos.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_NewCommandText (char *command)
 * @endcode
 *
 * @description
 * Llamada de la tecla ENTER en el campo de texto y comandos. El texto deberá ser
 * enviado y el comando procesado por las funciones de "parseo" de comandos de
 * usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] comando introducido por el usuario.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_NewCommandText(char *command)
{
  syslog(LOG_INFO, "[CLIENTE] Parseo de: %s", command);
  client_pre_out_function(command);
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_NewTopicEnter IRCInterface_NewTopicEnter
 *
 * @brief Llamada cuando se pulsa la tecla ENTER en el campo de tópico.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_NewTopicEnter (char * topicdata)
 * @endcode
 *
 * @description
 * Llamada cuando se pulsa la tecla ENTER en el campo de tópico.
 * Deberá intentarse cambiar el tópico del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * param[in] topicdata string con el tópico que se desea poner en el canal.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_NewTopicEnter(char *topicdata)
{
  char *msg, *channelActual;
  channelActual = IRCInterface_ActiveChannelName();
  if (topicdata && channelActual) {
    IRCMsg_Topic(&msg, NULL, channelActual, topicdata);
    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para hacer eso debes unirte a un canal primero e introducir un nuevo tema.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_SendFile IRCInterface_SendFile
 *
 * @brief Llamada por el botón "Enviar Archivo".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_SendFile (char * filename, char *nick, char *data, long unsigned int length)
 * @endcode
 *
 * @description
 * Llamada por el botón "Enviar Archivo". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función como todos los demás callbacks bloquea el interface
 * y por tanto es el programador el que debe determinar si crea un nuevo hilo para enviar el archivo o
 * no lo hace.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] filename nombre del fichero a enviar.
 * @param[in] nick nick del usuario que enviará el fichero.
 * @param[in] data datos a ser enviados.
 * @param[in] length longitud de los datos a ser enviados.
 *
 * @retval TRUE si se ha establecido la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

boolean IRCInterface_SendFile(char *filename, char *nick, char *data, long unsigned int length)
{
  long thread_response;
  pthread_t tid;
  srecv *sr;

  sr = (srecv *) malloc (sizeof(srecv));
  sr->nick = nick;
  sr->filename = filename;
  sr->data = data;
  sr->length = length;

  thread_response = pthread_create(&tid, NULL, &server_especial_enviar_ficheros, (void *)sr);
  if (thread_response != 0){
  	IRCInterface_ErrorDialog("Error al lanzar el hilo de escucha y envio de fichero.");
  	IRCInterface_WriteSystem("System", "Error al enviar el fichero.");
  	return FALSE;
  }
  pthread_detach(tid);
	return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_StartAudioChat IRCInterface_StartAudioChat
 *
 * @brief Llamada por el botón "Iniciar" del diálogo de chat de voz.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_StartAudioChat (char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Iniciar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función como todos los demás callbacks bloquea el interface
 * y por tanto para mantener la funcionalidad del chat de voz es imprescindible crear un hilo a efectos
 * de comunicación de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario con el que se desea conectar.
 *
 * @retval TRUE si se ha establecido la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

boolean IRCInterface_StartAudioChat(char *nick)
{
	return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_StopAudioChat IRCInterface_StopAudioChat
 *
 * @brief Llamada por el botón "Parar" del diálogo de chat de voz.
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_StopAudioChat (char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Parar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función sólo para la comunicación que puede ser reiniciada.
 * Evidentemente tiene que actuar sobre el hilo de chat de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario con el que se quiere parar el chat de voz.
 *
 * @retval TRUE si se ha parado la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

boolean IRCInterface_StopAudioChat(char *nick)
{
	return TRUE;
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_TakeOp IRCInterface_TakeOp
 *
 * @brief Llamada por el botón "Quitar Op".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_TakeOp (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Quitar Op". Previamente debe seleccionarse un nick del
 * canal para quitarle "op" a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a quitar op al usuario.
 * @param[in] nick nick del usuario al que se le va a quitar op.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_TakeOp(char *channel, char *nick)
{
  char *msg;
  if (channel && nick) {
    IRCMsg_Mode(&msg, NULL, channel, "-o", nick);
    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para hacer eso debes unirte a un canal primero y seleccionar a un usuario de la lista.");
  }
}

/**
 * @ingroup IRCInterfaceCallbacks
 *
 * @page IRCInterface_TakeVoice IRCInterface_TakeVoice
 *
 * @brief Llamada por el botón "Quitar voz".
 *
 * @synopsis
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_TakeVoice (char *channel, char *nick)
 * @endcode
 *
 * @description
 * Llamada por el botón "Quitar voz". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se le va a quitar voz al usuario.
 * @param[in] nick nick del usuario al que se va a quitar la voz.
 *
 * @warning Esta función debe ser implementada por el alumno.
 *
 * @author
 * Eloy Anguiano (eloy.anguiano@uam.es)
 *
 *<hr>
*/

void IRCInterface_TakeVoice(char *channel, char *nick)
{
  char *msg;
  if (channel && nick) {
    IRCMsg_Mode(&msg, NULL, channel, "-v", nick);
    send(socket_desc, msg, strlen(msg), 0);
    IRCInterface_PlaneRegisterOutMessage(msg);
    IRC_MFree(1, &msg);
  } else {
    IRCInterface_ErrorDialogThread("Para hacer eso debes unirte a un canal primero y seleccionar a un usuario de la lista.");
  }
}


/***************************************************************************************************/
/***************************************************************************************************/
/**                                                                                               **/
/** MMMMMMMMMM               MMMMM           AAAAAAA           IIIIIII NNNNNNNNNN          NNNNNN **/
/**  MMMMMMMMMM             MMMMM            AAAAAAAA           IIIII   NNNNNNNNNN          NNNN  **/
/**   MMMMM MMMM           MM MM            AAAAA   AA           III     NNNNN NNNN          NN   **/
/**   MMMMM  MMMM         MM  MM            AAAAA   AA           III     NNNNN  NNNN         NN   **/
/**   MMMMM   MMMM       MM   MM           AAAAA     AA          III     NNNNN   NNNN        NN   **/
/**   MMMMM    MMMM     MM    MM           AAAAA     AA          III     NNNNN    NNNN       NN   **/
/**   MMMMM     MMMM   MM     MM          AAAAA       AA         III     NNNNN     NNNN      NN   **/
/**   MMMMM      MMMM MM      MM          AAAAAAAAAAAAAA         III     NNNNN      NNNN     NN   **/
/**   MMMMM       MMMMM       MM         AAAAA         AA        III     NNNNN       NNNN    NN   **/
/**   MMMMM        MMM        MM         AAAAA         AA        III     NNNNN        NNNN   NN   **/
/**   MMMMM                   MM        AAAAA           AA       III     NNNNN         NNNN  NN   **/
/**   MMMMM                   MM        AAAAA           AA       III     NNNNN          NNNN NN   **/
/**  MMMMMMM                 MMMM     AAAAAA            AAAA    IIIII   NNNNNN           NNNNNNN  **/
/** MMMMMMMMM               MMMMMM  AAAAAAAA           AAAAAA  IIIIIII NNNNNNN            NNNNNNN **/
/**                                                                                               **/
/***************************************************************************************************/
/***************************************************************************************************/

int conexion_ssl(char *ssl_data){
  struct sockaddr_in server;
  SSL_CTX *ctx;
  SSL *ssl;
  int desc;
  struct addrinfo hints, *res;
  char ssl_cert_1[100];
  char ssl_cert_2[100];
	char buffer[8096];
  bzero(buffer, sizeof(char)*8096);
  strcpy(ssl_cert_1, "./certs/cliente.pem");
  strcpy(ssl_cert_2, "./certs/ca.pem");

  memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

  ctx = inicializar_nivel_SSL(&desc);

  fijar_contexto_SSL(ctx, ssl_cert_1, ssl_cert_2);

  if(getaddrinfo("127.0.0.1", "6669", &hints, &res)!=0){
		return 0;
	}

  ssl=conectar_canal_seguro_SSL(ctx, desc, *(res->ai_addr));
	ERR_print_errors_fp(stdout);
	/*printf("conectar_canal_seguro_SSL ok\n");*/
	if(!evaluar_post_connectar_SSL(ssl)){
		ERR_print_errors_fp(stdout);
		return;
	}

  syslog(LOG_INFO, "[CLIENTE IRC] Enviamos Data %s\n", ssl_data);
  if (enviar_datos_SSL(ssl, "NICK yoda") < 0) {
    syslog(LOG_ERR, "[CLIENTE IRC] Send SSL failed");
    return 0;
  }
  recibir_datos_SSL(ssl, &buffer[0]);
  syslog(LOG_INFO, "[CLIENTE IRC] Recibe %s", buffer);

  // bzero(user, 512);
  // strcpy(user, "USER yoda 0 * :miservidor.com");
  if (enviar_datos_SSL(ssl, "USER yoda 0 * :miservidor.com") < 0) {
    syslog(LOG_ERR, "[CLIENTE IRC] Send SSL failed");
    return 0;
  }
  /*cerrar_canal_SSL(ctx, ssl, desc);*/
  return 1;
}

int main (int argc, char *argv[])
{
	/* La función IRCInterface_Run debe ser llamada al final      */
	/* del main y es la que activa el interfaz gráfico quedándose */
	/* en esta función hasta que se pulsa alguna salida del       */
	/* interfaz gráfico.                                          */
  bool ssl_client = false;
  int i;
  char ssl_data[2048];
  bzero(ssl_data, 2048);
  if(argc>0){
    for(i = 0; i<argc; i++){
      syslog(LOG_ERR, "[CLIENTE IRC] Args: %d - %s", argc, argv[i]);
      if((strcmp(argv[i], "-ssl")==0)||(strcmp(argv[i], "--ssl")==0)){
        ssl_client=true;
        syslog(LOG_ERR, "[CLIENTE IRC] ES SSL");
      } else if(strcmp(argv[i], "--ssldata")==0){
        ssl_client = true;
        i++;
        break;
      }
    }
    for(i; i<argc; i++){
      strcat(ssl_data, argv[i]);
      strcat(ssl_data, " ");
      syslog(LOG_INFO, " %s\n", argv[i]);
    }
    syslog(LOG_INFO, "[CLIENTE IRC] Conexion Segura %s\n", ssl_data);
  }
  if(ssl_client){
    return conexion_ssl(&ssl_data[0]);
  } else {
    IRCInterface_Run(argc, argv);
    return 0;
  }
}
