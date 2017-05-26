#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "dropboxUtil.h"

void sync_server();
void receive_file(char *file, int socket, CLIENT* user);
void send_file(char *file, int socket, CLIENT* user);
