#include "../../includes/client_irc/G-2313-06-P3_client_err_handlers.h"

extern int socket_desc;
extern char* nick_cliente;

/* ERR FUNCTIONS */
void server_in_command_err_cannotsendtochan(char* command){
  char *prefix, *msg, *parse_nick, *parse_channel;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR BANNED");
  IRCParse_ErrCanNotSendToChan(command, &prefix, &parse_nick, &parse_channel, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_CANNOTSENDTOCHAN] No se ha podido enviar el mensaje, es posible que hayas sido baneado.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_channel, &msg);
}

void server_in_command_err_alreadyregistred(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR ALREADYREGISTRED");
  IRCParse_ErrAlreadyRegistred(command, &prefix, &parse_nick, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_ALREADYREGISTRED] No se ha podido registrar la conexión, ya existe una igual. Debes volver a iniciar sesión.");
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_err_nonicknamegiven(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR NONICKNAMEGIVEN");
  IRCParse_ErrNoNickNameGiven(command, &prefix, &parse_nick, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_NONICKNAMEGIVEN] Debes introducir un nickname.");
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_err_erroneusnickname(char* command){
  char *prefix, *msg, *parse_nick, *parse_nick_err;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_ERRONEUSNICKNAME");
  IRCParse_ErrErroneusNickName(command, &prefix, &parse_nick, &parse_nick_err, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_ERRONEUSNICKNAME] Debes introducir un nickname válido.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_nick_err, &msg);
}

void server_in_command_err_nicknameinuse(char* command){
  char *prefix, *msg, *parse_nick, *parse_nick_err;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_NICKNAMEINUSE");
  IRCParse_ErrNickNameInUse(command, &prefix, &parse_nick, &parse_nick_err, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_NICKNAMEINUSE] El nick elegido ya está siendo utilizado.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_nick_err, &msg);
}

void server_in_command_err_nickcollision(char* command){
  char *prefix, *msg, *parse_nick, *parse_nick_err, *parse_complex;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_NICKCOLLISION");
  IRCParse_ErrNickCollision(command, &prefix, &parse_nick, &parse_nick_err, &msg, &parse_complex);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_NICKCOLLISION] Existe una colisión con el nick elegido.");
  IRC_MFree(5, &prefix, &parse_nick, &parse_nick_err, &msg, &parse_complex);
}

void server_in_command_err_unavailresource(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_UNAVAILRESOURCE");
  IRCParse_ErrUnavailResource(command, &prefix, &parse_nick, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_UNAVAILRESOURCE] Recurso no disponible.");
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_err_restricted(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_RESTRICTED");
  IRCParse_ErrRestricted(command, &prefix, &parse_nick, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_RESTRICTED] Acceso restringido.");
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_err_passwdmismatch(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_PASSWDMISMATCH");
  IRCParse_ErrPasswdMismatch(command, &prefix, &parse_nick, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_PASSWDMISMATCH] Debes indicar una clave de acceso para conectarte.");
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_err_bannedfromchan(char* command){
  char *prefix, *msg, *parse_nick, *parse_chan;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_BANNEDFROMCHAN");
  IRCParse_ErrBannedFromChan(command, &prefix, &parse_nick, &parse_chan, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_BANNEDFROMCHAN] Estás baneado del canal al que quieres acceder.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_chan, &msg);
}

void server_in_command_err_channelisfull(char* command){
  char *prefix, *msg, *parse_nick, *parse_chan;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_CHANNELISFULL");
  IRCParse_ErrChannelIsFull(command, &prefix, &parse_nick, &parse_chan, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_CHANNELISFULL] El canal al que quieres acceder está lleno.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_chan, &msg);
}

void server_in_command_err_chanoprivsneeded(char* command){
  char *prefix, *msg, *parse_nick, *parse_chan;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_CHANOPRIVSNEEDED");
  IRCParse_ErrChanOPrivsNeeded(command, &prefix, &parse_nick, &parse_chan, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_CHANOPRIVSNEEDED] El canal requiere que tengas privilegios de OP.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_chan, &msg);
}

void server_in_command_err_inviteonlychan(char* command){
  char *prefix, *msg, *parse_nick, *parse_chan;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_INVITEONLYCHAN");
  IRCParse_ErrInviteOnlyChan(command, &prefix, &parse_nick, &parse_chan, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_INVITEONLYCHAN] El canal al que quieres acceder requiere invitación.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_chan, &msg);
}

void server_in_command_err_nochanmodes(char* command){
  char *prefix, *msg, *parse_nick, *parse_chan;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_NOCHANMODES");
  IRCParse_ErrNoChanModes(command, &prefix, &parse_nick, &parse_chan, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_NOCHANMODES] El canal no tiene modos.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_chan, &msg);
}

void server_in_command_err_nosuchchannel(char* command){
  char *prefix, *msg, *parse_nick, *parse_chan;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_NOSUCHCHANNEL");
  IRCParse_ErrNoSuchChannel(command, &prefix, &parse_nick, &parse_chan, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_NOSUCHCHANNEL] El canal al que quieres acceder no existe.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_chan, &msg);
}

void server_in_command_err_unknownmode(char* command){
  char *prefix, *msg, *parse_nick, *parse_chan, *parse_mode;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_UNKNOWNMODE");
  IRCParse_ErrUnknownMode(command, &prefix, &parse_nick, &parse_mode, &parse_chan, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_UNKNOWNMODE] Ese modo no existe.");
  IRC_MFree(5, &prefix, &parse_nick, &parse_chan, &parse_mode, &msg);
}

void server_in_command_err_nomotd(char* command){
  char *prefix, *msg, *parse_nick;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_NOMOTD");
  IRCParse_ErrNoMotd(command, &prefix, &parse_nick, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_NOMOTD] No se ha especificado un MOTD.");
  IRC_MFree(3, &prefix, &parse_nick, &msg);
}

void server_in_command_err_nosuchnick(char* command){
  char *prefix, *msg, *parse_nick, *parse_nick2;
  IRCInterface_PlaneRegisterInMessageThread(command);
  syslog(LOG_INFO, "[CLIENTE] [IN]: ERR_NOSUCHNICK");
  IRCParse_ErrNoSuchNick(command, &prefix, &parse_nick, &parse_nick2, &msg);
  client_show_error(msg);
  IRCInterface_ErrorDialogThread("[ERR_NOSUCHNICK] Ese nick no existe.");
  IRC_MFree(4, &prefix, &parse_nick, &parse_nick2, &msg);
}
