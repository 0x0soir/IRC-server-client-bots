#include "../includes/G-2313-06-BOT_bot.h"

char *server = NULL;
int port = NULL;
char *nick = NULL;
int socket_desc;
int alive = 1;
arraylist* channelsJoin = NULL;
arraylist* usersSpyTerms = NULL;

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

int main(int argc, char ** argv){
	int status, options;
	status = options = 0;
	void (*prev_fn)(int);

	//server_daemon();

	fprintf(stderr, ANSI_COLOR_CYAN "############################################\n" ANSI_COLOR_RESET);
	fprintf(stderr, ANSI_COLOR_CYAN "###\t BOT ESPIA - JORGE PARRILLA\t ###\n" ANSI_COLOR_RESET);
	fprintf(stderr, ANSI_COLOR_CYAN "############################################\n" ANSI_COLOR_RESET);
	while ((options = getopt(argc, argv, "a:n:")) != -1) {
		switch (options) {
		case 'a':
			server = optarg;
			break;
		case 'n':
			nick = optarg;
			break;
		default:
			status = 1;
			break;
		}
	}

	if (status || !server || !nick) {
		fprintf(stderr, "Uso: %s -a direccion[:puerto] -n nick\n", argv[0]);
		fprintf(stderr, "[!] Ejemplo: %s -a metis.ii.uam.es:6667 -n 0x0soir\n", argv[0]);
		exit(EXIT_FAILURE);
	} else {
		server = strtok(server, ":");
		port = atoi(strtok(NULL, " "));
		fprintf(stderr, "|| Direccion: \t%s\n", server);
		fprintf(stderr, "|| Puerto: \t%d\n", port);
		fprintf(stderr, "|| Nick: \t%s\n", nick);
		run_siri();
	}

	return EXIT_SUCCESS;
}
