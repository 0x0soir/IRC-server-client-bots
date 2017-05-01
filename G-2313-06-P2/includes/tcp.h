#ifndef _TCP_H
#define _TCP_H

int tcp_connect (char *server, int port);
int tcp_listen (int port, int max_conn);
int tcp_send (int fd, char *msg);
int tcp_receive (int fd, char *msg, int len);
int tcp_disconnect (int fd);

#endif
