#include "../includes/G-2313-06-P2_files.h"

extern int socket_desc;
extern char* nick_cliente;

/**
* @page server_especial_enviar_ficheros Enviar ficheros
* @section synopsis_file Synopsis
* @code
*	#include <G-2313-06-P2_files.h>
*
*   void *server_especial_enviar_ficheros(void *vrecv);
* @endcode
* @section descripcion_file Descripción
*
* Inicia la transferencia de ficheros de un cliente a otro, para ello
* ejecuta un socket mediante el que se solicita la transferencia. Si el
* usuario B acepta, se inicia la transferencia del fichero:
*
* Este punto de uno de los más importantes para el cliente ya que nos permite
* la transferencia de ficheros entre los distintos clientes del canal IRC.
* Funciona de forma sencilla pero intensa, es decir, se requiere el uso de un
* socket entre los dos clientes de forma directa ya que como se menciona en el
* enunciado de la práctica se debe implementar de esta manera para evitar costes
* excesivos en el servidor.
* Por ello, cuando un cliente inicia la transferencia de un fichero a otro
* cliente se crea un socket único para la petición de transferencia del fichero.
* Supongamos que un usuario A desea enviar un fichero a un usuario B,
* podemos resumir el protocol de transferencia de ficheros de la siguiente forma:
* <ul>
* <li>1) El usuario A envía la petición de transferencia al usuario B.</li>
* <li>2) El usuario B recibe la petición de transferencia del fichero y responde
* con una confirmación o rechazo a dicha transferencia.</li>
* <li>3) El usuario A recibe la confirmación/rechazo de la petición de
* transferecia.</li>
* <li>4) En caso de que se haya confirmado, el usuario A crea un nuevo socket
* con el usuario B únicamente destinado a la transferencia del fichero.
* En caso de rechazo, se cierra el socket generado previamente para la
* confirmación y se da por finalizada la transferencia.</li>
* <li>5) El usuario B recibe los datos del fichero del usuario A.</li>
* </ul>
* @section return_2 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_file Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void *server_especial_enviar_ficheros(void *vrecv){
 	srecv *sr;
 	char intimsg[512], aux[4096], ip[16];
 	struct timeval tv;
 	struct ifaddrs *addrs, *tmp;
 	int fd;
 	long ret;
 	char *comm, msg[512]="", lengthstr[128]="", portstr[6]="";
 	int recvsck, port, i;

 	sr = (srecv *) vrecv;

 	srand(time(NULL));

 	for (i = 0; i < 10; ++i){

 		port = rand() % 65000;
 		if(port < 1024){
 			port+=1024;
 		}

 		recvsck = tcp_listen (port, 1);
 		if(recvsck < 0){
 			continue;
 		}
 		break;
 	}
 	if(recvsck < 0){
 		IRCInterface_ErrorDialog("Error al abrir nuevo socket para conexión.");
 		IRCInterface_WriteSystem("System", "Error!");
 		return FALSE;
 	}

 	getifaddrs(&addrs);
 	tmp = addrs;

 	while(tmp)
 	{
 	    if(tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
 	    {
 	        struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
 	        sprintf(ip, "%s", inet_ntoa(pAddr->sin_addr));
 	    }
 	    if(strcmp (ip, "0.0.0.0") != 0 && strcmp (ip, "127.0.0.1") != 0 && strcmp (ip, "") != 0 && strlen (ip) >= 7){
 	    	break;
 	    }
 	    tmp = tmp->ifa_next;
 	}

 	freeifaddrs(addrs);

 	sprintf(lengthstr, "%lu", sr->length);
 	sprintf(portstr, "%d", port);

 	sprintf(msg, "%cFS %s %s %s %s %s", 1, nick_cliente, sr->filename, lengthstr, ip, portstr);

 	ret = IRCMsg_Privmsg(&comm, NULL, sr->nick, msg);
 	if(ret != IRC_OK){
 		IRCInterface_ErrorDialog ("Error al crear mensaje de handshake.");
 		IRCInterface_WriteSystem("System", "Error!");
 		return FALSE;
 	}

 	send(socket_desc, comm, strlen(comm), 0);
 	IRCInterface_PlaneRegisterOutMessage(comm);

 	free(comm);

 	tv.tv_sec = 60;
 	tv.tv_usec = 0;

 	setsockopt(recvsck, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

 	fd = accept (recvsck, NULL, 0);
 	if(fd < 0){
 		IRCInterface_ErrorDialogThread("No se pudo enviar el archivo. Error iniciando la comunicacion o archivo no aceptado.");
 		IRCInterface_WriteSystemThread("System", "Error enviando archivo");

 		free(sr->data);
 		free(sr);
 		return FALSE;
 	}

 	if(tcp_receive(fd, intimsg, 512) < 0){
 		IRCInterface_ErrorDialogThread("No se pudo enviar el archivo. Error en la conexion.");
 		IRCInterface_WriteSystemThread("System", "Error enviando archivo");

 		free(sr->data);
 		free(sr);
 		return FALSE;
 	}

 	for (i = 0; i < sr->length; i+=4096){
 		bzero (aux, 4096);
 		strncpy (aux, &(sr->data[i]), 4096*sizeof(char));
 		if(send(fd, aux, strlen(aux), 0)< 0){
 			IRCInterface_ErrorDialogThread("No se pudo enviar el archivo. Error durante el envio.");
 			IRCInterface_WriteSystemThread("System", "Error enviando archivo");

 			free(sr->data);
 			free(sr);
 			return FALSE;
 		}
 	}

 	IRCInterface_ErrorDialogThread("Transferencia de archivo completada.");

 	tcp_disconnect (fd);
	syslog(LOG_INFO, "[CLIENTE] [IN]: ENVIAR - PRE FREE");
	IRC_MFree(2, &sr->data, &sr);
	syslog(LOG_INFO, "[CLIENTE] [IN]: ENVIAR - POST FREE");
  return (void *) TRUE;
}

/**
* @page server_especial_recibir_ficheros Recibir ficheros
* @section synopsis_file_2 Synopsis
* @code
*	#include <G-2313-06-P2_files.h>
*
*   void *server_especial_recibir_ficheros(void *vmsg);
* @endcode
* @section descripcion_file_2 Descripción
*
* Inicia la transferencia de ficheros de un cliente a otro, para ello
* acepta o rechaza la petición que le llega del usuario A.
*
* Este punto de uno de los más importantes para el cliente ya que nos permite
* la transferencia de ficheros entre los distintos clientes del canal IRC.
* Funciona de forma sencilla pero intensa, es decir, se requiere el uso de un
* socket entre los dos clientes de forma directa ya que como se menciona en el
* enunciado de la práctica se debe implementar de esta manera para evitar costes
* excesivos en el servidor.
* Por ello, cuando un cliente inicia la transferencia de un fichero a otro
* cliente se crea un socket único para la petición de transferencia del fichero.
* Supongamos que un usuario A desea enviar un fichero a un usuario B,
* podemos resumir el protocol de transferencia de ficheros de la siguiente forma:
* <ul>
* <li>1) El usuario A envía la petición de transferencia al usuario B.</li>
* <li>2) El usuario B recibe la petición de transferencia del fichero y responde
* con una confirmación o rechazo a dicha transferencia.</li>
* <li>3) El usuario A recibe la confirmación/rechazo de la petición de
* transferecia.</li>
* <li>4) En caso de que se haya confirmado, el usuario A crea un nuevo socket
* con el usuario B únicamente destinado a la transferencia del fichero.
* En caso de rechazo, se cierra el socket generado previamente para la
* confirmación y se da por finalizada la transferencia.</li>
* <li>5) El usuario B recibe los datos del fichero del usuario A.</li>
* </ul>
* @section return_2 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada</li>
* </ul>
*
* @section authors_file_2 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void *server_especial_recibir_ficheros(void *vmsg){
  	char *msg;
  	char *nick = NULL, *filename = NULL, *server = NULL;
  	int length, port;
  	int sck, i, brecv = 0;
  	char data[4096];
  	FILE *f;

		syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - ERROR 0");
  	msg = (char *) vmsg;

		syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - ERROR - %s", msg);
  	// Obtenemos los datos de la nueva conexion
  	sscanf(msg, "\001FS %ms %ms %d %ms %d", &nick, &filename, &length, &server, &port);

  	if(IRCInterface_ReceiveDialogThread(nick, filename) == FALSE){
  		free(nick);
  		free(filename);
  		free(server);
  		free(vmsg);
  		return NULL;
  	}

  	// Conectamos con el usuario que nos envia el archivo
  	sck = tcp_connect (server, port);
  	if(sck < 0){
			syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - ERROR 1");
  		IRCInterface_ErrorDialogThread("No se pudo establecer la conexion para recibir el fichero.");
  		IRCInterface_WriteSystemThread("System", "Error recibiendo archivo");

  		free(nick);
  		free(filename);
  		free(server);
  		free(vmsg);
  		return NULL;
  	}

  	if(tcp_send(sck, "\002GO") < 0){
			syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - ERROR 2");
  		IRCInterface_ErrorDialogThread("No se pudo enviar el mensaje de handshake.");
  		IRCInterface_WriteSystemThread("System", "Error recibiendo archivo");

  		free(nick);
  		free(filename);
  		free(server);
  		free(vmsg);
  		return NULL;
  	}

  	f = fopen (filename, "w");
  	if(f == NULL){
			syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - ERROR 3");
  		IRCInterface_ErrorDialogThread("No se pudo crear el fichero en el sistema de archivos local.");
  		IRCInterface_WriteSystemThread("System", "Error recibiendo archivo");

  		free(nick);
  		free(filename);
  		free(server);
  		free(vmsg);
  		return NULL;
  	}

  	for (i = 0; i < length; brecv = 0){
  		bzero (data, 4096);
  		brecv = tcp_receive(sck, data, 4096);
  		if(brecv < 1){
  			continue;
  		}
  		i += brecv;
  		fprintf(f, "%.*s", brecv, data);
  	}

  	fclose(f);

  	IRCInterface_ErrorDialogThread("Transferencia de fichero completada.");

  	tcp_disconnect(sck);

		syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - PRE FREE");
		if(nick){
			IRC_MFree(1, &nick);
		}
		syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - FREE 1");
		if(filename){
			IRC_MFree(1, &filename);
		}
		syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - FREE 2");
		if(server){
			IRC_MFree(1, &server);
		}
		syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - FREE 3");
		if(vmsg){
			IRC_MFree(1, &vmsg);
		}
		syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - FREE 4");
		syslog(LOG_INFO, "[CLIENTE] [IN]: RECIBIR - POST FREE");
		return (void *) TRUE;
}
