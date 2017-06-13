#include "../includes/G-2313-06-BOT_tcp.h"
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

int tcp_new_socket(){
	int desc=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(desc < 0)
		return -1;
	return desc;
}

int tcp_bind(int desc, int puerto){
	struct sockaddr_in ip4addr;
	ip4addr.sin_family = AF_INET;
	ip4addr.sin_port = htons(puerto);
	ip4addr.sin_addr.s_addr=INADDR_ANY;
	/*bzero((void  *)&(ip4addr.sin_zero), 8);*/
	if(bind(desc, (struct sockaddr*)&ip4addr, sizeof(ip4addr))==-1)
		return -1;
	return 0;
}

int tcp_new_listen(int desc, int tam){
	int lstn;
	lstn = listen(desc, tam);
	if( lstn == -1)
		return -1;
	return 0;
}

int tcp_new_accept(int desc, struct sockaddr_in ip4addr){
	int acc=0;
	int addrlen =sizeof(ip4addr);
	acc = accept(desc, (struct sockaddr*)&ip4addr, (socklen_t*)&addrlen);

	if(acc == -1)
		return -1;
	return acc;
}

int tcp_new_open_connection(int desc, struct sockaddr res){
	int cnct=0;
	cnct= connect(desc, &res, sizeof(res));
	if (cnct==-1)
		return -1;
	return 0;
}

int tcp_connect(char *server, int port){
	int sck;
	struct hostent *host_addr;
	struct sockaddr_in sock_addr;

	if (server == NULL || port < 0){
		return -1;
	}

	host_addr = gethostbyname(server);
	if (host_addr == NULL){
		return -1;
	}

	sck = socket (AF_INET, SOCK_STREAM, 0);
	if (sck < 0){
		return -1;
	}

	bzero (&sock_addr, sizeof (struct sockaddr_in));

	sock_addr.sin_port = htons (port);
	sock_addr.sin_family = AF_INET;
	memcpy (&(sock_addr.sin_addr), host_addr->h_addr, host_addr->h_length);


	if (connect (sck, (struct sockaddr *) &sock_addr, sizeof (struct sockaddr_in)) < 0){
		return -1;
	}
	return sck;
}

int tcp_listen (int port, int max_conn){

	int sck;
	struct sockaddr_in sock_addr;

	if (max_conn < 1 || port < 0){
		return -1;
	}

	// Abrimos una socket TCP
	sck = socket (AF_INET, SOCK_STREAM, 0);
	if (sck < 0){
		return -1;
	}

	bzero (&sock_addr, sizeof (struct sockaddr_in));
	// Completamos la estructura de direccion para la socket
	sock_addr.sin_port = htons (port);
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = htonl (INADDR_ANY); // Acepta conexiones de cualquier direccion

	// Asignamos la direccion local a la socket
	if (bind(sck, (struct sockaddr *) &sock_addr, sizeof (struct sockaddr_in)) < 0){
		return -1;
	}

	// Comenzamos a escuchar por la socket
	if (listen (sck, max_conn) < 0){
		return -1;
	}

	return sck;

}

int tcp_send (int fd, char *msg){
	if (fd < 0 || msg == NULL){
		return -1;
	}
	return send (fd, msg, strlen(msg)*sizeof(char), 0);
}

int tcp_receive (int fd, char *msg, int len){
	if (fd < 0 || msg == NULL || len < 0){
		return -1;
	}
	return read (fd, msg, len*sizeof(char));
}

int tcp_disconnect (int fd){
	return close (fd);
}
