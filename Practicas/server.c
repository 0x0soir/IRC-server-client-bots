#include "server.h"

int server_start(void){
    int socket_id, opt=TRUE;
    struct sockaddr_in Direccion;

    syslog (LOG_INFO, "Creando socket...");
    if ( (socket_id = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        syslog(LOG_ERR, "ERROR creando socket");
        exit(EXIT_FAILURE);
    }

    /* Socket multiconexion */
    if( setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    Direccion.sin_family=AF_INET; /* TCP/IP family */
    Direccion.sin_port=htons(NFC_SERVER_PORT); /* Asigning port */
    Direccion.sin_addr.s_addr=htonl(INADDR_ANY); /* Accept all adresses */
    bzero((void *)&(Direccion.sin_zero), 8);

    /*Hay que hacer un bucle para conectar con todos los clientes que quieran no solo uno*/
    syslog (LOG_INFO, "Socket conectado en puerto %d", NFC_SERVER_PORT);
    if (bind (socket_id, (struct sockaddr *)&Direccion, sizeof(Direccion)) < 0){
        syslog(LOG_ERR, "ERROR conectando socket");
        exit(EXIT_FAILURE);
    }
    /*Hay que hacer un bucle para cada binding para escuchar todo el rato al mismo cliente*/
    syslog (LOG_INFO, "Escuchando conexiones...");
        if (listen (socket_id, MAX_CONNECTIONS) < 0){
            syslog(LOG_ERR, "ERROR escuchando conexiones");
            exit(EXIT_FAILURE);
        }
    return socket_id;
}

void server_accept_connection(int socket_id){
	int client_socket;
	socklen_t len;
	struct sockaddr Conexion;

	len = sizeof(Conexion);

	while(TRUE){
		client_socket = accept(socket_id, &Conexion, &len);
		if (client_socket < 0){
		    syslog(LOG_ERR, "ERROR aceptando conexiones");
        exit(EXIT_FAILURE);
		}
		syslog(LOG_ERR, "-- Conexion recibida en el socket");
		/*Se lanza el servicio y se espera a que acabe, de momento se comenta*/
		server_start_communication(client_socket);
	}
  close(socket_id);
	return;
}

void server_start_communication(int connval){
    int pid, val_read;
    char str[100];
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid == 0) return;
    while(TRUE)
    {
        bzero(str,100);
        val_read = recv(connval, str, 100, 0);
        if((val_read==0)||(str[0]=='\0')){
          syslog (LOG_INFO, "Cierra conexion");
          break;
        }
        syslog(LOG_INFO, "Mensaje recibido: %s", str);
        send(connval, str, strlen(str), 0);
        syslog(LOG_INFO, "Mensaje enviado");
    }
    syslog (LOG_INFO, "Servicio Cliente: Fin servicio");
}

int main(){
    server_accept_connection(server_start());
    return 0;
}
