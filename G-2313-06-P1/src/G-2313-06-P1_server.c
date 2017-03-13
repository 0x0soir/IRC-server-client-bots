#include "../includes/G-2313-06-P1_server.h"


/*! @mainpage P1 - Servidor IRC
*
* Funciones generales de conexión al servidor
*
* Esta sección incluye:
* - @subpage server_connection
* - @subpage server_common_functions
* - @subpage server_commands
*/

/*! @page server_connection Conexión del servidor
*
* <p>Esta sección incluye las funciones de conexión, inicialización del socket,
* listen de nuevos clientes, parseo de comandos de IRC, declaración y manejo
* del array de punteros a funciones para el parseo de comandos, inicalización
* del ThreadPool para los hilos que se asignarán a los clientes y el proceso
* de daemonización del servidor.</p>
* @section cabeceras Cabeceras
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* </code>
* @section variables Variables definidas
* <p>Se utilizan las siguientes variables definidas en G-2313-06-P1_server.h
* <p><code>
* #define MIN_POOL_THREADS        3
* <br>#define MAX_CONNECTIONS         100
* <br>#define SERVER_PORT             6667
* <br>#define CLIENT_MESSAGE_MAXSIZE  8096
* </code></p></p>
* @section functions Funciones implementadas
* <p>Se incluyen las siguientes funciones:
* <ul>
* <li>@subpage server_start</li>
* <li>@subpage server_accept_connection</li>
* <li>@subpage server_start_communication</li>
* <li>@subpage server_execute_function</li>
* <li>@subpage server_exit</li>
* <li>@subpage server_check_socket_status</li>
* <li>@subpage server_daemon</li>
* <li>@subpage server_start_pool</li>
* </ul></p>
*/


static bool server_status = true;
static int server_socket_desc;

thread_pool_t *pool = NULL;

/**
* @page server_start server_start
* @brief Inicia el servidor (TCP)
* @section synopsis2 Synopsis
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* <br>
* \b int \b server_start \b ()
* </code>
* @section descripcion2 Descripción
*
* Inicializa el socket principal del servidor, para ello se genera una llamada
* a <b>socket()</b> para iniciar el socket. A dicha función se le pasan como
* parámetros valores adecuados para la inicialización:
* <ul>
* <li>AF_INET</li>
* <li>SOCK_STREAM</li>
* </ul>
* Tras generar el identificador del socket con la llamada anterior, se asignan
* parámetros adicionales (como SOL_SOCKET y SO_REUSEADDR), además se genera
* la estructura sockaddr_in de dirección, en la que se especifican ciertos parámetros:
* <ul>
* <li><b>sin_family:</b> Se indica que la conexión es TCP/IP.</li>
* <li><b>sin_port:</b> Se indica que el puerto por defecto es 6667.</li>
* <li><b>sin_addr:</b> Se indica que cualquier IP es admitida al conectar (INADDR_ANY).</li>
* </ul>
* Tras generar la configuración básica del socket, se utilizan las funciones bind() y listen()
* para finalizar con la conexión del socket principal al que accederán los clientes
* mediante el puerto por defecto:
* <ul>
* <li><b>bind():</b> Realiza una unión entre el identificador del socket generado y
* el struct de Direccion mencionado anteriormente para asignar los parametros
* del struct a ese socket.</li>
* <li><b>listen():</b> Indica al socket que debe admitir conexiones en el identificador
* del socket generado anteriormente y además le pasa el parámetros de conexiones
* máximas que en este caso está fijado por MAX_CONNECTIONS (100).</li>
* </ul>
* @section return2 Valores devueltos
* <ul>
* <li><b>int</b> Descriptor del master socket generado para atender las peticiones.</li>
* </ul>
*
* @section authors2 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
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

    syslog(LOG_INFO, "Socket conectado en puerto %d", SERVER_PORT);
    if (bind(socket_desc, (struct sockaddr *)&Direccion, sizeof(Direccion)) < 0){
      syslog(LOG_ERR, "ERROR conectando socket");
      exit(EXIT_FAILURE);
    }

    syslog (LOG_INFO, "Escuchando conexiones...");
    if (listen(socket_desc, MAX_CONNECTIONS) < 0){
      syslog(LOG_ERR, "ERROR escuchando conexiones");
      exit(EXIT_FAILURE);
    }
    return socket_desc;
}

/**
* @page server_accept_connection server_accept_connection
* @brief Acepta las conexiones recibidas en el socket.
* @section synopsis3 Synopsis
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* <br>
* <b>void server_accept_connection(int socket_desc)</b>
* </code>
* @section descripcion3 Descripción
*
* Acepta las conexiones recibidas mediante el descriptor <b>socket_desc</b> recibido
* como parámetro de la función.
* Inicia un bucle que realiza un <b>accept()</b> a los clientes recibidos mediante
* el descriptor del socket. Esta función genera un identificador único para los
* clientes (un descriptor del socket). En caso encontrar un error, finaliza la
* ejecución del servidor.
* <p>Tras recibir al cliente y asignarle un descriptor, genera una tarea (task) para
* dicho cliente utilizando la librería generada para tal efecto (ThreadPool).
* Esta tarea se añade a un hilo de ejecución exclusivo para el cliente.</p>

* @section return3 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors3 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
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
    t = task_create();
    task_init(t, server_start_communication, client_socket);
    task_add(pool, t);
	}
  syslog(LOG_ERR, "ACCEPT CONNECTION: LIMPIAR");
  free(t);
	return;
}

/**
* @page server_start_communication server_start_communication
* @brief Ejecuta el control (main) principal para cada hilo.
* @section synopsis4 Synopsis
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* <br>
* <b>void *server_start_communication(int socket_desc)</b>
* </code>
* @section descripcion4 Descripción
*
* <p>Contiene el código que ejecutan los hijos al recibir la conexión de un
* cliente en <b>server_accept_connection()</b>. Declara varias variables
* que se pasan como parámetros al resto de funciones utilizadas para el
* parseo de comandos:</p>
* <ul>
* <li><b>char *nick:</b> Almacena el nick del personaje, se reserva memoria y se pasa
* como puntero a las funciones de parseo de comandos en la que la función destinada
* al comando nick utilizará un <b>strcpy</b> para actualizar dicho valor.</li>
* <li><b>int *register_status:</b> Entero que representa el estado actual del cliente
* del hilo, sirve de control para las funciones de parseo y cierre de socket:
* <ul>
* <li>0: Indica que no se ha efectuado un registro válido del usuario (comando NICK y
* USER).</li>
* <li>1: Indica que se ha parseado correctamente el nick y es posible realizar
* el registro del usuario (comando USER).</li>
* <li>2: Indica que se ha registrado correctamente usuario y es posible realizar
* cualquier otro comando del IRC. Sirve de control para no ejecutar otros comandos
* hasta que el usuario no envíe los comandos válidos (NICK y USER) para el registro.
* A su vez, esto permitirá que en caso de cerrarse el socket se comprobará que si
* el valor de esta variable es 2 se utilizará la función del TAD para eliminar
* al usuario de la lista.</li>
* </ul>
* </li>
* </ul>
* <p>Contiene un bucle que comprueba si el socket sigue abierto y parsea los comandos
* del usuario mediante las funciones apropiadas para ello (<b>IRC_UnPipelineCommands
* y IRC_CommandQuery</b>). Dichos comandos parseados envían a la función
* <b>server_execute_function</b> que contiene un array de punteros a función
* para redirigir cada comando a su función apropiada.</p>
* <p>Una vez se detecta que el socket se ha cerrado, se elimina al usuario del TAD
* en caso de que <b>register_status</b> tenga el valor adecuado (2) y se liberan
* los punteros reservados para este hilo.</p>
*
* @section return4 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors4 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void *server_start_communication(int socket_desc){
  int val_read, *register_status = malloc(sizeof(int));
  char str[2000], *command=NULL, *unpipeline_response=NULL, *nick = malloc(sizeof(char) * 10);
  syslog (LOG_INFO, "Inicia conexion con... %d", socket_desc);
  *register_status = 0;
  while(server_status)
  {
    if(server_check_socket_status(socket_desc)){
      syslog (LOG_INFO, "[DESC: %d] Detectado ausente, desconectando...", socket_desc);
      break;
    }
    memset(str, 0, 2000);
    val_read = recv(socket_desc, str, 2000, 0);
    if((val_read<1)||(str[0]=='\0')){
      syslog (LOG_INFO, "[DESC: %d] Detectado ausente, desconectando...", socket_desc);
      break;
    }
    /* Comando ya parseado -> Ejecuto */
    unpipeline_response=IRC_UnPipelineCommands(str, &command);
    syslog(LOG_INFO, "COMMAND: %s | PIPE: %ld", command, IRC_CommandQuery(command));
    server_execute_function(IRC_CommandQuery(command), command, socket_desc, nick, register_status);
    syslog(LOG_INFO, "PROCESADO");
    if(command){
      free(command);
      command = NULL;
    }
	  while(unpipeline_response!=NULL){
		 	unpipeline_response=IRC_UnPipelineCommands(unpipeline_response, &command);
      syslog(LOG_INFO, "COMMAND: %s | PIPE: %ld", command, IRC_CommandQuery(command));
      server_execute_function(IRC_CommandQuery(command), command, socket_desc, nick, register_status);
      if(command){
        free(command);
        command = NULL;
      }
		}
    syslog(LOG_INFO, "[DESC: %d] Mensaje enviado", socket_desc);
  }
  if(*register_status>0){
    syslog(LOG_INFO, "[DESC: %d] FIN CLIENTE: No recibido QUIT, se borran datos de: %s", socket_desc, nick);
    IRCTAD_Quit(nick);
    if(server_users_find_by_nick(nick)){
      syslog(LOG_INFO, "[DESC: %d] FIN CLIENTE: Sigue existiendo el usuario: %s", socket_desc, nick);
    } else {
      syslog(LOG_INFO, "[DESC: %d] FIN CLIENTE: Cliente eliminado correctamente: %s", socket_desc, nick);
    }
    syslog(LOG_INFO, "[DESC: %d] FIN CLIENTE: Cierra desc", socket_desc);
    /* Cerrar conexion con el usuario y liberar el hilo */
  	close(socket_desc);
  }
  if(register_status){
    free(register_status);
    register_status = NULL;
  }
  if(unpipeline_response){
    free(unpipeline_response);
    unpipeline_response = NULL;
  }
  if(nick){
    free(nick);
    nick = NULL;
  }
  syslog(LOG_INFO, "--> FIN CLIENTE: MUERE EL HILO");
  pthread_exit(NULL);
  return NULL;
}

/**
* @page server_execute_function server_execute_function
* @brief Envía a los comandos a las funciones apropiadas por punteros.
* @section synopsis5 Synopsis
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* <br>
* <b>void server_execute_function(long functionName, char* command, int desc, char* nick, int* register_status)</b>
* </code>
* @section descripcion5 Descripción
*
* <p>Genera un array de punteros a función para enviar a los comandos a su
* función de parseo y procesamiento. Utiliza el index del TAD como índice
* del array y reserva sitio en el array mediante la variable del TAD
* <b>IRC_MAX_USER_COMMANDS</b>:</p>
* <code>
* FunctionCallBack functions[IRC_MAX_USER_COMMANDS];
* <br>int i;
* <br>char *msg;
* <br>for(i=0; i<IRC_MAX_USER_COMMANDS; i++){
* <br>  functions[i]=NULL;
* <br>}
* <br>functions[NICK] = & server_command_nick;
* <br>functions[USER] = & server_command_user;
* <br>...</code>
* <p>Una vez generado el array con todos los comandos implementados, envía
* dicho comando a su función pasándole como parámetro las variables adecuadas
* para ello:</p>
* <ul>
* <li><b>char *command:</b> String que contiene el comando completo recibido.</li>
* <li><b>int *desc:</b> Entero que representa el descriptor del socket del cliente.</li>
* <li><b>char *nick:</b> String que contiene el nick del cliente del hilo.</li>
* <li><b>int *register_status:</b> Entero que representa el estado del registro.</li>
* </ul>
* <p>La llamada a la función se hace de la siguiente manera:</p>
* <code>
* (*functions[functionName])(command, desc, nick, register_status);
* </code>
*
* @section return5 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors5 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_execute_function(long functionName, char* command, int desc, char* nick, int* register_status){
  FunctionCallBack functions[IRC_MAX_USER_COMMANDS];
  int i;
  char *msg;
  for(i=0; i<IRC_MAX_USER_COMMANDS; i++){
    functions[i]=NULL;
  }
  /* Definir lista de funciones para cada comando*/
  functions[NICK] = &server_command_nick;
  functions[USER] = &server_command_user;
  functions[JOIN] = &server_command_join;
  functions[QUIT] = &server_command_quit;
  functions[PING] = &server_command_ping;
  functions[LIST] = &server_command_list;
  functions[PRIVMSG] = &server_command_privmsg;
  functions[PART] = &server_command_part;
  functions[NAMES] = &server_command_names;
  functions[KICK] = &server_command_kick;
  functions[MODE] = &server_command_mode;
  functions[AWAY] = &server_command_away;
  functions[WHOIS] = &server_command_whois;
  functions[TOPIC] = &server_command_topic;
  functions[MOTD] = &server_command_motd;
  /* Llamar a la funcion del argumento */
  if ((functionName<0)||(functionName>IRC_MAX_USER_COMMANDS)||(functions[functionName]==NULL)){
    if(*register_status>0){
      if(IRCMsg_ErrUnKnownCommand(&msg, "ip.servidor", nick, command)==IRC_OK){
        send(desc, msg, strlen(msg), 0);
        free(msg);
      }
    }
  } else {
    (*functions[functionName])(command, desc, nick, register_status);
  }
}

/**
* @page server_exit server_exit
* @brief Finaliza el servidor IRC y cierra el socket.
* @section synopsis6 Synopsis
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* <br>
* <b>void server_exit()</b>
* </code>
* @section descripcion6 Descripción
*
* <p>Una vez se finaliza el servidor (se recibe la señal adecuada para ello)
* se procede a finalizar el servicio actual. Para ello se finaliza el bucle
* de <b>server_accept_connection()</b> actualizando la variable global
* <b>server_status=false</b> que provoca una interrupción en el while
* que acepta las conexiones de los clientes.</p>
* <p>Además, se finaliza el socket con la función <b>close(server_socket_desc)</b>,
* tras esto se realiza una llamada la función <b>thread_pool_delete(pool)</b>
* para eliminar todos los hilos generados en el ThreadPool y se liberan
* los recursos para proceder a la salida del proceso general.</p>
*
* @section return6 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors6 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_exit(){
  server_status = false;
  close(server_socket_desc);
  thread_pool_delete(pool);
  free(pool);
  syslog(LOG_INFO, "TODO LIBERADO");
  exit(EXIT_SUCCESS);
}

/**
* @page server_check_socket_status server_check_socket_status
* @brief Comprueba si el socket del cliente sigue abierto.
* @section Synopsis
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* <br>
* <b>int server_check_socket_status(int socket_desc)</b>
* </code>
* @section descripcion Descripción
*
* <p>Es una función simple que comprueba mediante la función <b>select()</b>
* si el socket del cliente recibido como parámetro <b>socket_desc</b> sigue
* abierto o no. Para ello se hace una selección de dicho socket y si devuelve
* error o salta el timeout se indica que el socket está cerrado y en la función
* <b>server_start_communication</b> se liberan los recursos necesarios.</p>
*
* @section return Valores devueltos
* <ul>
* <li><b>int</b> Devuelve el valor verdadero/falso sobre el estado del socket.</li>
* </ul>
*
* @section authors Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
int server_check_socket_status(int socket_desc){
  int activity;
  fd_set readfds;
  struct timeval timeout;
  FD_ZERO(&readfds);
  FD_SET(socket_desc, &readfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = 10000;
  syslog(LOG_INFO, "COMPROBANDO SOCKET CON SELECT...");
  activity = select(socket_desc, &readfds, NULL, NULL, &timeout);
  if ((activity < 0) && (errno!=EINTR)){
    syslog(LOG_INFO, "Socket cerrado");
    return 1;
  } else {
    syslog(LOG_INFO, "Socket abierto");
    return 0;
  }
}

/**
* @page server_daemon server_daemon
* @brief Daemoniza el servidor.
* @section synopsis7 Synopsis
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* <br>
* <b>void server_daemon()</b>
* </code>
* @section descripcion7 Descripción
*
* <p>Crea un nuevo proceso mediante un <b>fork()</b> y provoca el proceso de
* damonización por la que el servidor deja de depender de la terminal actual
* desde la que se ejecuta el servidor y pasa a ejecutarse en segundo plano.</p>
*
* @section return7 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors7 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
void server_daemon(){
  pid_t pid;
	pid = fork();
	if (pid < 0) exit(EXIT_FAILURE);
	if (pid > 0) exit(EXIT_SUCCESS);
	umask(0);
	syslog(LOG_ERR, "Iniciando servidor DAEMON...");
	if (setsid()< 0) {
		syslog (LOG_ERR, "Error creando el nuevo SID del proceso hijo");
		exit(EXIT_FAILURE);
	}
	if ((chdir("/")) < 0) {
		syslog (LOG_ERR, "Error cambiando el directorio actual de trabajo =\"/\"");
		exit(EXIT_FAILURE);
	}
	syslog(LOG_INFO, "Cerrando los descriptores base...");
	close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO);
	return;
}

/**
* @page server_start_pool server_start_pool
* @brief Inicializa el ThreadPool con los hilos.
* @section synopsis8 Synopsis
* <code>
* \b #include \b <G-2313-06-P1_server.h>
* <br>
* <b>void server_start_pool()</b>
* </code>
* @section descripcion8 Descripción
*
* <p>Inicializa el ThreadPool con los hilos seleccionados por defecto mediante
* la variable <b>MIN_POOL_THREADS</b>. Para ello ejecuta una llamada la función
* de la siguiente manera:</p>
* <code>
* pool = thread_pool_create(MIN_POOL_THREADS);
* <br>thread_pool_init(pool);
* </code>
* @section return8 Valores devueltos
* <ul>
* <li><b>void</b> No devuelve nada.</li>
* </ul>
*
* @section authors8 Autores
* <ul>
* <li>Jorge Parrilla Llamas (jorge.parrilla@estudiante.uam.es)</li>
* <li>Javier de Marco Tomás (javier.marco@estudiante.uam.es)</li>
* </ul>
*/
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
  server_daemon();
  signal(SIGINT, server_exit);
  server_start_pool();
  server_accept_connection(server_start());
  return 0;
}
