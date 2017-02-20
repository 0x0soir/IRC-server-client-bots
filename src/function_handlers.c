#include "function_handlers.h"

int server_command_user_function(int i){
  syslog(LOG_INFO, "FUNCION DE USER EJECUTADA");
  return 0;
}
