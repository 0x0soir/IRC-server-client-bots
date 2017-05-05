#include <stdio.h>
#include "../includes/G-2313-06-P3_ssl.h"

int main(int argc, char ** argv){
	if (argv[1]==NULL)
		return 0;
	else if(strcmp(argv[1],"c")==0){
		ssl_start_client();
	}
	else if(strcmp(argv[1],"s")==0){
		ssl_start_server();
	}
	else
		return 0;
	return 1;
}
