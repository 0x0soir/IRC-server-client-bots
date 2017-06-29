#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#ifndef CLIENT_MESSAGE_MAXSIZE

#define CLIENT_MESSAGE_MAXSIZE  16192

#endif

#ifndef _BOT_H
#define _BOT_H

void run_siri();
void* client_function_response(void *arg);
int connect_server();
void send_nick();
void send_user();
void send_join();

#endif
