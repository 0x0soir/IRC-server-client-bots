#include "server.h"

/* Estado del servidor (controla los whiles mediante signals) */
static bool server_status = true;
/* Identificador global para el desc. del socket */
static int server_socket_desc;

/*
 * Function:  server_start
 * ----------------------------------------
 *  Crear el socket master que recibira todas las peticiones de los
 *  clientes del servidor IRC.
 *
 *  returns: descriptor del socket
 */
int server_start(){
    int socket_desc, opt=TRUE;
    struct sockaddr_in Direccion;

    syslog (LOG_INFO, "Creando socket...");
    if ( (socket_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        syslog(LOG_ERR, "ERROR creando socket");
        exit(EXIT_FAILURE);
    }

    /* Activar multiconexion del socket */
    if( setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    server_socket_desc = socket_desc;

    Direccion.sin_family=AF_INET; /* TCP/IP */
    Direccion.sin_port=htons(SERVER_PORT); /* Asignar el puerto */
    Direccion.sin_addr.s_addr=htonl(INADDR_ANY); /* Aceptar todas las IPs */
    bzero((void *)&(Direccion.sin_zero), 8);

    syslog (LOG_INFO, "Socket conectado en puerto %d", SERVER_PORT);
    if (bind (socket_desc, (struct sockaddr *)&Direccion, sizeof(Direccion)) < 0){
      syslog(LOG_ERR, "ERROR conectando socket");
      exit(EXIT_FAILURE);
    }

    syslog (LOG_INFO, "Escuchando conexiones...");
    if (listen (socket_desc, MAX_CONNECTIONS) < 0){
      syslog(LOG_ERR, "ERROR escuchando conexiones");
      exit(EXIT_FAILURE);
    }
    return socket_desc;
}

/*
 * Function:  server_accept_connection
 * ----------------------------------------
 *  Inicia el bucle de aceptación de conexiones remotas al socket creando
 *  en la funcion anterior (server_start)
 *
 *  socket_desc: descriptor de conexion del socket
 *
 *  returns: void
 */
void server_accept_connection(int socket_desc){
	int client_socket;
	socklen_t len;
	struct sockaddr Conexion;

	len = sizeof(Conexion);

	while(server_status){
		client_socket = accept(socket_desc, &Conexion, &len);
		if (client_socket < 0){
		    syslog(LOG_ERR, "ERROR aceptando conexiones");
        exit(EXIT_FAILURE);
		}

		syslog(LOG_ERR, "-- Conexion recibida en el socket");
		/*Se lanza el servicio y se espera a que acabe, de momento se comenta*/
		server_start_communication(client_socket);
	}
  close(socket_desc);
	return;
}

/*
 * Function:  server_start_communication
 * ----------------------------------------
 *  Inicia la traza de comunicacion con el cliente (recepcion y envio de msg)
 *
 *  socket_desc: descriptor de conexion del socket
 *
 *  returns: void
 */
void server_start_communication(int socket_desc){
  int pid, val_read;
  char str[2000];
  pid = fork();
  if (pid < 0) exit(EXIT_FAILURE);
  if (pid == 0) return;
  while(server_status)
  {
    bzero(str, 2000);
    val_read = recv(socket_desc, str, 2000, 0);
    if(isClosedSocket(val_read, str)){
      syslog (LOG_INFO, "Cierra conexion");
      break;
    }
    syslog(LOG_INFO, "Mensaje recibido: %s", str);
    send(socket_desc, str, strlen(str), 0);
    syslog(LOG_INFO, "Mensaje enviado");
  }
  syslog(LOG_INFO, "Servicio Cliente: Fin servicio");
  exit(0);
}

/*
 * Function:  server_exit
 * ----------------------------------------
 *  Cierra el socket y termina el servidor
 *
 *  returns: EXIT_SUCCESS
 */
void server_exit(int handler){
  syslog(LOG_INFO, "CIERRA SERVIDOR");
  server_status = false;
  close(server_socket_desc);
  exit(EXIT_SUCCESS);
}

/*
 * Function:  isClosedSocket
 * ----------------------------------------
 *  Comprueba si el socket del cliente está cerrado
 *
 *  val_read: return de la funcion recv
 *  str[]: bloque leido de entrada del cliente
 *
 *  returns: true/false dependiendo de si esta cerrada o no
 */
int isClosedSocket(int val_read, char str[]){
  return (val_read==0)||(str[0]=='\0');
}

/*
 * Function:  server_daemon
 * ----------------------------------------
 *  Inicia el proceso en modo daemon (hace fork, mata al hijo y continua padre)
 *
 *  returns: FAILURE or SUCCESS
 */
void server_daemon(){
  int pid;
  pid = fork();
  if(pid<0){
    exit(EXIT_FAILURE);
  }
  if(pid>0){
    exit(EXIT_SUCCESS);
  }
}

int main(){
  server_daemon();
  struct sigaction act;
  act.sa_handler = server_exit;
  sigaction(SIGINT, &act, NULL);
  server_accept_connection(server_start());
  return 0;
}