#include "dropboxUtil.h"
#include <dirent.h>
PFILA2 fileList;

 int connect_server(char *host, int port);
 void sync_client();
 void send_file(char *file, int socket);
 void get_file(char *file, int socket);
 void print_file_list(int socket);
 void close_connection();
