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
#include <pthread.h>
#include <dirent.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "dropboxUtil.h"
const SSL_METHOD *method;
SSL_CTX *ctx;
PFILA2 clientsList;
pthread_mutex_t mutexDevicesRegister = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexClientRegister = PTHREAD_MUTEX_INITIALIZER;

struct serverLoopParams {
	int socket;
	SSL* ssl;
};

typedef struct serverLoopParams ARGS;
void sync_server();
void receive_file(char* file, int socket,SSL* ssl, CLIENT* user);
void timeServer(SSL* ssl);
void send_file(char*file, int socket, SSL* ssl, CLIENT* user);
