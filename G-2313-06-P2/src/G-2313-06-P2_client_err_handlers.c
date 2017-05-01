#include "../includes/G-2313-06-P2_client_err_handlers.h"

extern int socket_desc;
extern char* nick_cliente;

/* ERR FUNCTIONS */
void server_in_command_err_cannotsendtochan(char* command){
  char *prefix, *msg, *parse_nick, *parse_channel;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR BANNED");
  IRCParse_ErrCanNotSendToChan(command, &prefix, &parse_nick, &parse_channel, &msg);
  IRCInterface_WriteChannelThread(parse_channel, "*", msg);
  IRCInterface_ErrorDialogThread("No se ha podido enviar el mensaje, es posible que hayas sido baneado.");
  IRC_MFree(3, &prefix, &parse_nick, &parse_channel, &msg);
}
