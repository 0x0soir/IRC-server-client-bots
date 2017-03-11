#include "../includes/server.h"

/* Estado del servidor (controla los whiles mediante signals) */
static bool server_status = true;
/* Identificador global para el desc. del socket */
static int server_socket_desc;

thread_pool_t *pool = NULL;

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
    if(setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
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

void *task_callback(void *arg)
{
    int t_pool_id;
    int sum = 0;
    syslog(LOG_ERR, "TASK: Entra en task");
    for(t_pool_id = 0; t_pool_id < 100; t_pool_id++)
    {
        sum += t_pool_id;
    }
    syslog(LOG_ERR, "TASK: %d", sum);
    return NULL;
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
  task_t *t;
	socklen_t len;
	struct sockaddr Conexion;

	len = sizeof(Conexion);

	while(server_status){
		client_socket = accept(socket_desc, &Conexion, &len);
		if (client_socket < 0){
		    syslog(LOG_ERR, "ERROR aceptando conexiones");
        exit(EXIT_FAILURE);
		}

    syslog(LOG_ERR, "-- Conexion recibida en el socket (SD padre: %d, SD hijo: %d)", socket_desc, client_socket);
		/*Se lanza el servicio y se espera a que acabe, de momento se comenta*/
    syslog(LOG_ERR, "POOL: Creando hilo al cliente...");
    t = task_create();
    syslog(LOG_ERR, "POOL: Task creado");
    task_init(t, server_start_communication, client_socket);
    syslog(LOG_ERR, "POOL: Task iniciado");
    task_add(pool, t);
    syslog(LOG_ERR, "POOL: Fin crear tarea");

	}
  syslog(LOG_ERR, "ACCEPT CONNECTION: LIMPIAR");
  free(t);
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
void *server_start_communication(int socket_desc){
  int val_read, *register_status;
  char str[2000], *command=NULL, *unpipeline_response=NULL, *nick;
  nick = malloc(sizeof(char) * 9);
  register_status = malloc(sizeof(int));
  syslog (LOG_INFO, "Inicia conexion con... %d", socket_desc);
  while(server_status)
  {
    memset(str, 0, 2000);
    val_read = recv(socket_desc, str, 2000, 0);
    if(isClosedSocket(val_read, str)){
      syslog (LOG_INFO, "Cierra conexion");
      break;
    }
    /* Comando ya parseado -> Ejecuto */
    unpipeline_response=IRC_UnPipelineCommands(str, &command);
    syslog(LOG_INFO, "COMMAND: %s | PIPE: %ld", command, IRC_CommandQuery(command));
    server_execute_function(IRC_CommandQuery(command), command, socket_desc, nick, register_status);
    syslog(LOG_INFO, "PROCESADO");
    free(command);
    /* Parseo comando siguiente -> Ejecuto */
	  while(unpipeline_response!=NULL){
		 	unpipeline_response=IRC_UnPipelineCommands(unpipeline_response, &command);
      syslog(LOG_INFO, "COMMAND: %s | PIPE: %ld", command, IRC_CommandQuery(command));
      server_execute_function(IRC_CommandQuery(command), command, socket_desc, nick, register_status);
      free(command);
		}
    syslog(LOG_INFO, "Mensaje enviado");
  }
  syslog(LOG_INFO, "Servicio Cliente: Fin servicio %d", socket_desc);
  /* Cerrar conexion con el usuario y liberar el hilo */
	close(socket_desc);
  free(register_status);
  free(unpipeline_response);
  free(nick);
}

void server_execute_function(long functionName, char* command, int desc, char* nick, int* register_status){
  FunctionCallBack functions[IRC_MAX_USER_COMMANDS];
  int i;
  for(i=0; i<IRC_MAX_USER_COMMANDS; i++){
    functions[i]=NULL;
  }
  /* Definir lista de funciones para cada comando*/
  functions[NICK] = &server_command_function_nick;
  functions[USER] = &server_command_function_user;
  functions[JOIN] = &server_command_function_join;
  /* Llamar a la funcion del argumento */
  if ((functionName<0)||(functionName>IRC_MAX_USER_COMMANDS)||(functions[functionName]==NULL)){
    syslog(LOG_INFO, "NO EXISTE EL MANEJADOR DE LA FUNCION");
  } else {
    functions[functionName](command, desc, nick, register_status);
  }
}

/*
 * Function:  server_exit
 * ----------------------------------------
 *  Cierra el socket y termina el servidor
 *
 *  returns: EXIT_SUCCESS
 */
void server_exit(){
  server_status = false;
  close(server_socket_desc);
  thread_pool_delete(pool);
  free(pool);
  syslog(LOG_INFO, "TODO LIBERADO");
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

void server_start_pool(){
  /* Inicializar thread pool */
  pool = thread_pool_create(MIN_POOL_THREADS);
  syslog(LOG_ERR, "POOL INICIADA: (%d hilos base)", MIN_POOL_THREADS);
  thread_pool_init(pool);
}

int main(){
  /*struct sigaction act;
  act.sa_handler = server_exit;
  sigaction(SIGKILL, &act, NULL);*/
  /*server_daemon();*/
  signal(SIGINT, server_exit);
  server_start_pool();
  server_accept_connection(server_start());
  return 0;
}
