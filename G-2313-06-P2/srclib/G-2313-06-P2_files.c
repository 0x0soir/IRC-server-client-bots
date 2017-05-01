#include "../includes/G-2313-06-P2_files.h"

void *server_especial_ficheros(void *vmsg){
  	char *msg;
  	char *nick = NULL, *filename = NULL, *server = NULL;
  	int length, port;
  	int sck, i, brecv = 0;
  	char data[4096];
  	FILE *f;

  	msg = (char *) vmsg;

  	// Obtenemos los datos de la nueva conexion
  	sscanf (msg, "\001FS %ms %ms %d %ms %d", &nick, &filename, &length, &server, &port);

  	// Preguntamos si se desea recibir el archivo
  	if (IRCInterface_ReceiveDialogThread(nick, filename) == FALSE){
  		free (nick);
  		free (filename);
  		free (server);
  		free (vmsg);
  		return NULL;
  	}

  	// Conectamos con el usuario que nos envia el archivo
  	sck = tcp_connect (server, port);
  	if (sck < 0){
  		IRCInterface_ErrorDialogThread("No se pudo establecer la conexion para recibir el fichero.");
  		IRCInterface_WriteSystemThread("System", "Error recibiendo archivo");

  		g_print ("ERRORNO: %s; sck: %d\n", strerror (errno), sck);

  		free (nick);
  		free (filename);
  		free (server);
  		free (vmsg);
  		return NULL;
  	}

  	// Enviamos un mensaje de confirmacion que sera ignorado.
  	if (tcp_send (sck, "\002GO") < 0){
  		IRCInterface_ErrorDialogThread ("No se pudo enviar el mensaje de handshake.");
  		IRCInterface_WriteSystemThread("System", "Error recibiendo archivo");

  		free (nick);
  		free (filename);
  		free (server);
  		free (vmsg);
  		return NULL;
  	}

  	// Abrimos un archivo para guardar los datos
  	f = fopen (filename, "w");
  	if (f == NULL){
  		IRCInterface_ErrorDialogThread ("No se pudo crear el fichero en el sistema de archivos local.");
  		IRCInterface_WriteSystemThread("System", "Error recibiendo archivo");

  		free (nick);
  		free (filename);
  		free (server);
  		free (vmsg);
  		return NULL;
  	}

  	// Recibimos y guardamos los datos
  	for (i = 0; i < length; brecv = 0){
  		bzero (data, 4096);
  		brecv = tcp_receive (sck, data, 4096);
  		if (brecv < 1){
  			continue;
  		}
  		i += brecv;
  		fprintf(f, "%.*s", brecv, data);
  	}

  	fclose (f);

  	// Notificamos el final de la transferencia
  	IRCInterface_ErrorDialogThread ("Transferencia de fichero completada.");

  	tcp_disconnect (sck);

  	free (nick);
  	free (filename);
  	free (server);
  	free (vmsg);
		return (void *) TRUE;
}
