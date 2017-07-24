#include "dropboxUtil.h"
#include <dirent.h>
char fileListBuffer[1000];

const SSL_METHOD *method;
SSL_CTX *ctx;

 int connect_server(char *host, int port);
 void sync_client();
 void send_file(char *file, int socket, SSL* ssl);
 void get_file(char *file, int socket, SSL* ssl);
 void print_file_list(int socket, SSL* ssl);
 void close_connection();
