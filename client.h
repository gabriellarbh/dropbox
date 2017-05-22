#include "dropboxUtil.h"
int connect_server(char *host, int port);
 void sync_client();
 void send_file(char *file, int socket);
 void get_file(char *file, int socket);
 void close_connection();
